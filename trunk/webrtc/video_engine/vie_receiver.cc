/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/video_engine/vie_receiver.h"

#include <vector>

#include "webrtc/modules/remote_bitrate_estimator/include/remote_bitrate_estimator.h"
#include "webrtc/modules/rtp_rtcp/interface/receive_statistics.h"
#include "webrtc/modules/rtp_rtcp/interface/rtp_header_parser.h"
#include "webrtc/modules/rtp_rtcp/interface/rtp_payload_registry.h"
#include "webrtc/modules/rtp_rtcp/interface/rtp_receiver.h"
#include "webrtc/modules/rtp_rtcp/interface/rtp_rtcp.h"
#include "webrtc/modules/utility/interface/rtp_dump.h"
#include "webrtc/modules/video_coding/main/interface/video_coding.h"
#include "webrtc/system_wrappers/interface/critical_section_wrapper.h"
#include "webrtc/system_wrappers/interface/tick_util.h"
#include "webrtc/system_wrappers/interface/trace.h"

namespace webrtc {

ViEReceiver::ViEReceiver(const int32_t channel_id,
                         VideoCodingModule* module_vcm,
                         RemoteBitrateEstimator* remote_bitrate_estimator,
                         RtpFeedback* rtp_feedback)
    : receive_cs_(CriticalSectionWrapper::CreateCriticalSection()),
      channel_id_(channel_id),
      rtp_header_parser_(RtpHeaderParser::Create()),
      rtp_payload_registry_(new RTPPayloadRegistry(
          channel_id, RTPPayloadStrategy::CreateStrategy(false))),
      rtp_receiver_(RtpReceiver::CreateVideoReceiver(
          channel_id, Clock::GetRealTimeClock(), this, rtp_feedback,
          rtp_payload_registry_.get())),
      rtp_receive_statistics_(ReceiveStatistics::Create(
          Clock::GetRealTimeClock())),
      rtp_rtcp_(NULL),
      vcm_(module_vcm),
      remote_bitrate_estimator_(remote_bitrate_estimator),
      external_decryption_(NULL),
      decryption_buffer_(NULL),
      rtp_dump_(NULL),
      receiving_(false) {
  assert(remote_bitrate_estimator);
}

ViEReceiver::~ViEReceiver() {
  if (decryption_buffer_) {
    delete[] decryption_buffer_;
    decryption_buffer_ = NULL;
  }
  if (rtp_dump_) {
    rtp_dump_->Stop();
    RtpDump::DestroyRtpDump(rtp_dump_);
    rtp_dump_ = NULL;
  }
}

bool ViEReceiver::SetReceiveCodec(const VideoCodec& video_codec) {
  int8_t old_pltype = -1;
  if (rtp_payload_registry_->ReceivePayloadType(video_codec.plName,
                                                kVideoPayloadTypeFrequency,
                                                0,
                                                video_codec.maxBitrate,
                                                &old_pltype) != -1) {
    rtp_payload_registry_->DeRegisterReceivePayload(old_pltype);
  }

  return RegisterPayload(video_codec);
}

bool ViEReceiver::RegisterPayload(const VideoCodec& video_codec) {
  return rtp_receiver_->RegisterReceivePayload(video_codec.plName,
                                               video_codec.plType,
                                               kVideoPayloadTypeFrequency,
                                               0,
                                               video_codec.maxBitrate) == 0;
}

bool ViEReceiver::SetNackStatus(bool enable,
                                int max_nack_reordering_threshold) {
  return rtp_receiver_->SetNACKStatus(enable ? kNackRtcp : kNackOff,
                                      max_nack_reordering_threshold) == 0;
}

void ViEReceiver::SetRtxStatus(bool enable, uint32_t ssrc) {
  rtp_receiver_->SetRTXStatus(true, ssrc);
}

void ViEReceiver::SetRtxPayloadType(uint32_t payload_type) {
  rtp_receiver_->SetRtxPayloadType(payload_type);
}

uint32_t ViEReceiver::GetRemoteSsrc() const {
  return rtp_receiver_->SSRC();
}

int ViEReceiver::GetCsrcs(uint32_t* csrcs) const {
  return rtp_receiver_->CSRCs(csrcs);
}

int ViEReceiver::RegisterExternalDecryption(Encryption* decryption) {
  CriticalSectionScoped cs(receive_cs_.get());
  if (external_decryption_) {
    return -1;
  }
  decryption_buffer_ = new uint8_t[kViEMaxMtu];
  if (decryption_buffer_ == NULL) {
    return -1;
  }
  external_decryption_ = decryption;
  return 0;
}

int ViEReceiver::DeregisterExternalDecryption() {
  CriticalSectionScoped cs(receive_cs_.get());
  if (external_decryption_ == NULL) {
    return -1;
  }
  external_decryption_ = NULL;
  return 0;
}

void ViEReceiver::SetRtpRtcpModule(RtpRtcp* module) {
  rtp_rtcp_ = module;
}

RtpReceiver* ViEReceiver::GetRtpReceiver() const {
  return rtp_receiver_.get();
}

void ViEReceiver::RegisterSimulcastRtpRtcpModules(
    const std::list<RtpRtcp*>& rtp_modules) {
  CriticalSectionScoped cs(receive_cs_.get());
  rtp_rtcp_simulcast_.clear();

  if (!rtp_modules.empty()) {
    rtp_rtcp_simulcast_.insert(rtp_rtcp_simulcast_.begin(),
                               rtp_modules.begin(),
                               rtp_modules.end());
  }
}

bool ViEReceiver::SetReceiveTimestampOffsetStatus(bool enable, int id) {
  if (enable) {
    return rtp_header_parser_->RegisterRtpHeaderExtension(
        kRtpExtensionTransmissionTimeOffset, id);
  } else {
    return rtp_header_parser_->DeregisterRtpHeaderExtension(
        kRtpExtensionTransmissionTimeOffset);
  }
}

bool ViEReceiver::SetReceiveAbsoluteSendTimeStatus(bool enable, int id) {
  if (enable) {
    return rtp_header_parser_->RegisterRtpHeaderExtension(
        kRtpExtensionAbsoluteSendTime, id);
  } else {
    return rtp_header_parser_->DeregisterRtpHeaderExtension(
        kRtpExtensionAbsoluteSendTime);
  }
}

int ViEReceiver::ReceivedRTPPacket(const void* rtp_packet,
                                   int rtp_packet_length) {
  return InsertRTPPacket(static_cast<const int8_t*>(rtp_packet),
                         rtp_packet_length);
}

int ViEReceiver::ReceivedRTCPPacket(const void* rtcp_packet,
                                    int rtcp_packet_length) {
  return InsertRTCPPacket(static_cast<const int8_t*>(rtcp_packet),
                          rtcp_packet_length);
}

int32_t ViEReceiver::OnReceivedPayloadData(
    const uint8_t* payload_data, const uint16_t payload_size,
    const WebRtcRTPHeader* rtp_header) {
  if (rtp_header == NULL) {
    return 0;
  }
  if (vcm_->IncomingPacket(payload_data, payload_size, *rtp_header) != 0) {
    // Check this...
    return -1;
  }
  return 0;
}

bool ViEReceiver::OnRecoveredPacket(const uint8_t* rtp_packet,
                                    int rtp_packet_length) {
  RTPHeader header;
  if (!rtp_header_parser_->Parse(rtp_packet, rtp_packet_length, &header)) {
    WEBRTC_TRACE(kTraceDebug, webrtc::kTraceVideo, channel_id_,
                 "IncomingPacket invalid RTP header");
    return false;
  }
  header.payload_type_frequency = kVideoPayloadTypeFrequency;
  PayloadUnion payload_specific;
  if (!rtp_payload_registry_->GetPayloadSpecifics(header.payloadType,
                                                 &payload_specific)) {
    return false;
  }
  return rtp_receiver_->IncomingRtpPacket(&header, rtp_packet,
                                          rtp_packet_length,
                                          payload_specific, false);
}

int ViEReceiver::InsertRTPPacket(const int8_t* rtp_packet,
                                 int rtp_packet_length) {
  // TODO(mflodman) Change decrypt to get rid of this cast.
  int8_t* tmp_ptr = const_cast<int8_t*>(rtp_packet);
  unsigned char* received_packet = reinterpret_cast<unsigned char*>(tmp_ptr);
  int received_packet_length = rtp_packet_length;

  {
    CriticalSectionScoped cs(receive_cs_.get());
    if (!receiving_) {
      return -1;
    }

    if (external_decryption_) {
      int decrypted_length = kViEMaxMtu;
      external_decryption_->decrypt(channel_id_, received_packet,
                                    decryption_buffer_, received_packet_length,
                                    &decrypted_length);
      if (decrypted_length <= 0) {
        WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceVideo, channel_id_,
                     "RTP decryption failed");
        return -1;
      } else if (decrypted_length > kViEMaxMtu) {
        WEBRTC_TRACE(webrtc::kTraceCritical, webrtc::kTraceVideo, channel_id_,
                     "InsertRTPPacket: %d bytes is allocated as RTP decrytption"
                     " output, external decryption used %d bytes. => memory is "
                     " now corrupted", kViEMaxMtu, decrypted_length);
        return -1;
      }
      received_packet = decryption_buffer_;
      received_packet_length = decrypted_length;
    }

    if (rtp_dump_) {
      rtp_dump_->DumpPacket(received_packet,
                           static_cast<uint16_t>(received_packet_length));
    }
  }
  RTPHeader header;
  if (!rtp_header_parser_->Parse(received_packet, received_packet_length,
                                 &header)) {
    WEBRTC_TRACE(webrtc::kTraceDebug, webrtc::kTraceVideo, channel_id_,
                 "IncomingPacket invalid RTP header");
    return -1;
  }
  const int payload_size = received_packet_length - header.headerLength;
  remote_bitrate_estimator_->IncomingPacket(TickTime::MillisecondTimestamp(),
                                            payload_size, header);
  header.payload_type_frequency = kVideoPayloadTypeFrequency;
  bool in_order = rtp_receiver_->InOrderPacket(header.sequenceNumber);
  bool retransmitted = !in_order && IsPacketRetransmitted(header);
  rtp_receive_statistics_->IncomingPacket(header, received_packet_length,
                                          retransmitted, in_order);
  PayloadUnion payload_specific;
  if (!rtp_payload_registry_->GetPayloadSpecifics(header.payloadType,
                                                  &payload_specific)) {
    return -1;
  }
  return rtp_receiver_->IncomingRtpPacket(&header, received_packet,
                                          received_packet_length,
                                          payload_specific, in_order) ? 0 : -1;
}

int ViEReceiver::InsertRTCPPacket(const int8_t* rtcp_packet,
                                  int rtcp_packet_length) {
  // TODO(mflodman) Change decrypt to get rid of this cast.
  int8_t* tmp_ptr = const_cast<int8_t*>(rtcp_packet);
  unsigned char* received_packet = reinterpret_cast<unsigned char*>(tmp_ptr);
  int received_packet_length = rtcp_packet_length;
  {
    CriticalSectionScoped cs(receive_cs_.get());
    if (!receiving_) {
      return -1;
    }

    if (external_decryption_) {
      int decrypted_length = kViEMaxMtu;
      external_decryption_->decrypt_rtcp(channel_id_, received_packet,
                                         decryption_buffer_,
                                         received_packet_length,
                                         &decrypted_length);
      if (decrypted_length <= 0) {
        WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceVideo, channel_id_,
                     "RTP decryption failed");
        return -1;
      } else if (decrypted_length > kViEMaxMtu) {
        WEBRTC_TRACE(webrtc::kTraceCritical, webrtc::kTraceVideo, channel_id_,
                     "InsertRTCPPacket: %d bytes is allocated as RTP "
                     " decrytption output, external decryption used %d bytes. "
                     " => memory is now corrupted",
                     kViEMaxMtu, decrypted_length);
        return -1;
      }
      received_packet = decryption_buffer_;
      received_packet_length = decrypted_length;
    }

    if (rtp_dump_) {
      rtp_dump_->DumpPacket(
          received_packet, static_cast<uint16_t>(received_packet_length));
    }
  }
  {
    CriticalSectionScoped cs(receive_cs_.get());
    std::list<RtpRtcp*>::iterator it = rtp_rtcp_simulcast_.begin();
    while (it != rtp_rtcp_simulcast_.end()) {
      RtpRtcp* rtp_rtcp = *it++;
      rtp_rtcp->IncomingRtcpPacket(received_packet, received_packet_length);
    }
  }
  assert(rtp_rtcp_);  // Should be set by owner at construction time.
  return rtp_rtcp_->IncomingRtcpPacket(received_packet, received_packet_length);
}

void ViEReceiver::StartReceive() {
  CriticalSectionScoped cs(receive_cs_.get());
  receiving_ = true;
}

void ViEReceiver::StopReceive() {
  CriticalSectionScoped cs(receive_cs_.get());
  receiving_ = false;
}

int ViEReceiver::StartRTPDump(const char file_nameUTF8[1024]) {
  CriticalSectionScoped cs(receive_cs_.get());
  if (rtp_dump_) {
    // Restart it if it already exists and is started
    rtp_dump_->Stop();
  } else {
    rtp_dump_ = RtpDump::CreateRtpDump();
    if (rtp_dump_ == NULL) {
      WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceVideo, channel_id_,
                   "StartRTPDump: Failed to create RTP dump");
      return -1;
    }
  }
  if (rtp_dump_->Start(file_nameUTF8) != 0) {
    RtpDump::DestroyRtpDump(rtp_dump_);
    rtp_dump_ = NULL;
    WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceVideo, channel_id_,
                 "StartRTPDump: Failed to start RTP dump");
    return -1;
  }
  return 0;
}

int ViEReceiver::StopRTPDump() {
  CriticalSectionScoped cs(receive_cs_.get());
  if (rtp_dump_) {
    if (rtp_dump_->IsActive()) {
      rtp_dump_->Stop();
    } else {
      WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceVideo, channel_id_,
                   "StopRTPDump: Dump not active");
    }
    RtpDump::DestroyRtpDump(rtp_dump_);
    rtp_dump_ = NULL;
  } else {
    WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceVideo, channel_id_,
                 "StopRTPDump: RTP dump not started");
    return -1;
  }
  return 0;
}

// TODO(holmer): To be moved to ViEChannelGroup.
void ViEReceiver::EstimatedReceiveBandwidth(
    unsigned int* available_bandwidth) const {
  std::vector<unsigned int> ssrcs;

  // LatestEstimate returns an error if there is no valid bitrate estimate, but
  // ViEReceiver instead returns a zero estimate.
  remote_bitrate_estimator_->LatestEstimate(&ssrcs, available_bandwidth);
  if (std::find(ssrcs.begin(), ssrcs.end(), rtp_receiver_->SSRC()) !=
      ssrcs.end()) {
    *available_bandwidth /= ssrcs.size();
  } else {
    *available_bandwidth = 0;
  }
}

ReceiveStatistics* ViEReceiver::GetReceiveStatistics() const {
  return rtp_receive_statistics_.get();
}

bool ViEReceiver::IsPacketRetransmitted(const RTPHeader& header) const {
  bool rtx_enabled = false;
  uint32_t rtx_ssrc = 0;
  int rtx_payload_type = 0;
  rtp_receiver_->RTXStatus(&rtx_enabled, &rtx_ssrc, &rtx_payload_type);
  if (!rtx_enabled) {
    // Check if this is a retransmission.
    StreamStatistician::Statistics stats;
    StreamStatistician* statistician =
        rtp_receive_statistics_->GetStatistician(header.ssrc);
    if (statistician && statistician->GetStatistics(&stats, false)) {
      uint16_t min_rtt = 0;
      rtp_rtcp_->RTT(rtp_receiver_->SSRC(), NULL, NULL, &min_rtt, NULL);
      return rtp_receiver_->RetransmitOfOldPacket(header, stats.jitter,
                                                  min_rtt);
    }
  }
  return false;
}
}  // namespace webrtc
