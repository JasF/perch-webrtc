/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/rtp_rtcp/source/rtp_receiver_impl.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "webrtc/modules/rtp_rtcp/interface/rtp_payload_registry.h"
#include "webrtc/modules/rtp_rtcp/interface/rtp_rtcp_defines.h"
#include "webrtc/modules/rtp_rtcp/source/rtp_receiver_strategy.h"
#include "webrtc/system_wrappers/interface/trace.h"

namespace webrtc {

using ModuleRTPUtility::GetCurrentRTP;
using ModuleRTPUtility::Payload;
using ModuleRTPUtility::RTPPayloadParser;
using ModuleRTPUtility::StringCompare;

RtpReceiver* RtpReceiver::CreateVideoReceiver(
    int id, Clock* clock,
    RtpData* incoming_payload_callback,
    RtpFeedback* incoming_messages_callback,
    RTPPayloadRegistry* rtp_payload_registry) {
  if (!incoming_payload_callback)
    incoming_payload_callback = NullObjectRtpData();
  if (!incoming_messages_callback)
    incoming_messages_callback = NullObjectRtpFeedback();
  return new RtpReceiverImpl(
      id, clock, NullObjectRtpAudioFeedback(), incoming_messages_callback,
      rtp_payload_registry,
      RTPReceiverStrategy::CreateVideoStrategy(id, incoming_payload_callback));
}

RtpReceiver* RtpReceiver::CreateAudioReceiver(
    int id, Clock* clock,
    RtpAudioFeedback* incoming_audio_feedback,
    RtpData* incoming_payload_callback,
    RtpFeedback* incoming_messages_callback,
    RTPPayloadRegistry* rtp_payload_registry) {
  if (!incoming_audio_feedback)
    incoming_audio_feedback = NullObjectRtpAudioFeedback();
  if (!incoming_payload_callback)
    incoming_payload_callback = NullObjectRtpData();
  if (!incoming_messages_callback)
    incoming_messages_callback = NullObjectRtpFeedback();
  return new RtpReceiverImpl(
      id, clock, incoming_audio_feedback, incoming_messages_callback,
      rtp_payload_registry,
      RTPReceiverStrategy::CreateAudioStrategy(id, incoming_payload_callback,
                                               incoming_audio_feedback));
}

RtpReceiverImpl::RtpReceiverImpl(int32_t id,
                         Clock* clock,
                         RtpAudioFeedback* incoming_audio_messages_callback,
                         RtpFeedback* incoming_messages_callback,
                         RTPPayloadRegistry* rtp_payload_registry,
                         RTPReceiverStrategy* rtp_media_receiver)
    : clock_(clock),
      rtp_payload_registry_(rtp_payload_registry),
      rtp_media_receiver_(rtp_media_receiver),
      id_(id),
      cb_rtp_feedback_(incoming_messages_callback),
      critical_section_rtp_receiver_(
        CriticalSectionWrapper::CreateCriticalSection()),
      last_receive_time_(0),
      last_received_payload_length_(0),
      ssrc_(0),
      num_csrcs_(0),
      current_remote_csrc_(),
      last_received_timestamp_(0),
      last_received_frame_time_ms_(0),
      last_received_sequence_number_(0),
      nack_method_(kNackOff),
      max_reordering_threshold_(kDefaultMaxReorderingThreshold),
      rtx_(false),
      ssrc_rtx_(0),
      payload_type_rtx_(-1) {
  assert(incoming_audio_messages_callback);
  assert(incoming_messages_callback);

  memset(current_remote_csrc_, 0, sizeof(current_remote_csrc_));

  WEBRTC_TRACE(kTraceMemory, kTraceRtpRtcp, id, "%s created", __FUNCTION__);
}

RtpReceiverImpl::~RtpReceiverImpl() {
  for (int i = 0; i < num_csrcs_; ++i) {
    cb_rtp_feedback_->OnIncomingCSRCChanged(id_, current_remote_csrc_[i],
                                            false);
  }
  WEBRTC_TRACE(kTraceMemory, kTraceRtpRtcp, id_, "%s deleted", __FUNCTION__);
}

RTPReceiverStrategy* RtpReceiverImpl::GetMediaReceiver() const {
  return rtp_media_receiver_.get();
}

RtpVideoCodecTypes RtpReceiverImpl::VideoCodecType() const {
  PayloadUnion media_specific;
  rtp_media_receiver_->GetLastMediaSpecificPayload(&media_specific);
  return media_specific.Video.videoCodecType;
}

int32_t RtpReceiverImpl::RegisterReceivePayload(
    const char payload_name[RTP_PAYLOAD_NAME_SIZE],
    const int8_t payload_type,
    const uint32_t frequency,
    const uint8_t channels,
    const uint32_t rate) {
  CriticalSectionScoped lock(critical_section_rtp_receiver_.get());

  // TODO(phoglund): Try to streamline handling of the RED codec and some other
  // cases which makes it necessary to keep track of whether we created a
  // payload or not.
  bool created_new_payload = false;
  int32_t result = rtp_payload_registry_->RegisterReceivePayload(
      payload_name, payload_type, frequency, channels, rate,
      &created_new_payload);
  if (created_new_payload) {
    if (rtp_media_receiver_->OnNewPayloadTypeCreated(payload_name, payload_type,
                                                     frequency) != 0) {
      WEBRTC_TRACE(kTraceError, kTraceRtpRtcp, id_,
                   "%s failed to register payload",
                   __FUNCTION__);
      return -1;
    }
  }
  return result;
}

int32_t RtpReceiverImpl::DeRegisterReceivePayload(
    const int8_t payload_type) {
  CriticalSectionScoped lock(critical_section_rtp_receiver_.get());
  return rtp_payload_registry_->DeRegisterReceivePayload(payload_type);
}

NACKMethod RtpReceiverImpl::NACK() const {
  CriticalSectionScoped lock(critical_section_rtp_receiver_.get());
  return nack_method_;
}

// Turn negative acknowledgment requests on/off.
int32_t RtpReceiverImpl::SetNACKStatus(const NACKMethod method,
                                   int max_reordering_threshold) {
  CriticalSectionScoped lock(critical_section_rtp_receiver_.get());
  if (max_reordering_threshold < 0) {
    return -1;
  } else if (method == kNackRtcp) {
    max_reordering_threshold_ = max_reordering_threshold;
  } else {
    max_reordering_threshold_ = kDefaultMaxReorderingThreshold;
  }
  nack_method_ = method;
  return 0;
}

void RtpReceiverImpl::SetRTXStatus(bool enable, uint32_t ssrc) {
  CriticalSectionScoped lock(critical_section_rtp_receiver_.get());
  rtx_ = enable;
  ssrc_rtx_ = ssrc;
}

void RtpReceiverImpl::RTXStatus(bool* enable, uint32_t* ssrc,
                            int* payload_type) const {
  CriticalSectionScoped lock(critical_section_rtp_receiver_.get());
  *enable = rtx_;
  *ssrc = ssrc_rtx_;
  *payload_type = payload_type_rtx_;
}

void RtpReceiverImpl::SetRtxPayloadType(int payload_type) {
  CriticalSectionScoped cs(critical_section_rtp_receiver_.get());
  payload_type_rtx_ = payload_type;
}

uint32_t RtpReceiverImpl::SSRC() const {
  CriticalSectionScoped lock(critical_section_rtp_receiver_.get());
  return ssrc_;
}

// Get remote CSRC.
int32_t RtpReceiverImpl::CSRCs(uint32_t array_of_csrcs[kRtpCsrcSize]) const {
  CriticalSectionScoped lock(critical_section_rtp_receiver_.get());

  assert(num_csrcs_ <= kRtpCsrcSize);

  if (num_csrcs_ > 0) {
    memcpy(array_of_csrcs, current_remote_csrc_, sizeof(uint32_t)*num_csrcs_);
  }
  return num_csrcs_;
}

int32_t RtpReceiverImpl::Energy(
    uint8_t array_of_energy[kRtpCsrcSize]) const {
  return rtp_media_receiver_->Energy(array_of_energy);
}

bool RtpReceiverImpl::IncomingRtpPacket(
    RTPHeader* rtp_header,
    const uint8_t* packet,
    int packet_length,
    PayloadUnion payload_specific,
    bool in_order) {
  // The rtp_header argument contains the parsed RTP header.
  int length = packet_length - rtp_header->paddingLength;

  // Sanity check.
  if ((length - rtp_header->headerLength) < 0) {
    WEBRTC_TRACE(kTraceError, kTraceRtpRtcp, id_,
                 "%s invalid argument",
                 __FUNCTION__);
    return false;
  }
  {
    CriticalSectionScoped cs(critical_section_rtp_receiver_.get());
    // TODO(holmer): Make rtp_header const after RTX has been broken out.
    if (rtx_) {
      if (ssrc_rtx_ == rtp_header->ssrc) {
        // Sanity check, RTX packets has 2 extra header bytes.
        if (rtp_header->headerLength + kRtxHeaderSize > packet_length) {
          return false;
        }
        // If a specific RTX payload type is negotiated, set back to the media
        // payload type and treat it like a media packet from here.
        if (payload_type_rtx_ != -1) {
          if (payload_type_rtx_ == rtp_header->payloadType &&
              rtp_payload_registry_->last_received_media_payload_type() != -1) {
            rtp_header->payloadType =
                rtp_payload_registry_->last_received_media_payload_type();
          } else {
            WEBRTC_TRACE(kTraceWarning, kTraceRtpRtcp, id_,
                         "Incorrect RTX configuration, dropping packet.");
            return false;
          }
        }
        rtp_header->ssrc = ssrc_;
        rtp_header->sequenceNumber =
          (packet[rtp_header->headerLength] << 8) +
          packet[1 + rtp_header->headerLength];
        // Count the RTX header as part of the RTP
        rtp_header->headerLength += 2;
      }
    }
  }
  int8_t first_payload_byte = 0;
  if (length > 0) {
    first_payload_byte = packet[rtp_header->headerLength];
  }
  // Trigger our callbacks.
  CheckSSRCChanged(rtp_header);

  bool is_red = false;
  bool should_reset_statistics = false;

  if (CheckPayloadChanged(rtp_header,
                          first_payload_byte,
                          is_red,
                          &payload_specific,
                          &should_reset_statistics) == -1) {
    if (length - rtp_header->headerLength == 0) {
      // OK, keep-alive packet.
      WEBRTC_TRACE(kTraceStream, kTraceRtpRtcp, id_,
                   "%s received keepalive",
                   __FUNCTION__);
      return true;
    }
    WEBRTC_TRACE(kTraceWarning, kTraceRtpRtcp, id_,
                 "%s received invalid payloadtype",
                 __FUNCTION__);
    return false;
  }

  if (should_reset_statistics) {
    cb_rtp_feedback_->ResetStatistics(ssrc_);
  }

  WebRtcRTPHeader webrtc_rtp_header;
  memset(&webrtc_rtp_header, 0, sizeof(webrtc_rtp_header));
  webrtc_rtp_header.header = *rtp_header;
  CheckCSRC(&webrtc_rtp_header);

  uint16_t payload_data_length =
    ModuleRTPUtility::GetPayloadDataLength(*rtp_header, packet_length);

  bool is_first_packet_in_frame = false;
  bool is_first_packet = false;
  {
    CriticalSectionScoped lock(critical_section_rtp_receiver_.get());
    is_first_packet_in_frame =
          last_received_sequence_number_ + 1 == rtp_header->sequenceNumber &&
          Timestamp() != rtp_header->timestamp;
    is_first_packet = is_first_packet_in_frame || last_receive_time_ == 0;
  }

  int32_t ret_val = rtp_media_receiver_->ParseRtpPacket(
      &webrtc_rtp_header, payload_specific, is_red, packet, packet_length,
      clock_->TimeInMilliseconds(), is_first_packet);

  if (ret_val < 0) {
    return false;
  }

  {
    CriticalSectionScoped lock(critical_section_rtp_receiver_.get());

    last_receive_time_ = clock_->TimeInMilliseconds();
    last_received_payload_length_ = payload_data_length;

    if (in_order) {
      if (last_received_timestamp_ != rtp_header->timestamp) {
        last_received_timestamp_ = rtp_header->timestamp;
        last_received_frame_time_ms_ = clock_->TimeInMilliseconds();
      }
      last_received_sequence_number_ = rtp_header->sequenceNumber;
    }
  }
  return true;
}

bool RtpReceiverImpl::RetransmitOfOldPacket(const RTPHeader& header,
                                            int jitter, int min_rtt) const {
  if (InOrderPacket(header.sequenceNumber)) {
    return false;
  }

  CriticalSectionScoped cs(critical_section_rtp_receiver_.get());
  uint32_t frequency_khz = header.payload_type_frequency / 1000;
  assert(frequency_khz > 0);

  int64_t time_diff_ms = clock_->TimeInMilliseconds() -
      last_receive_time_;

  // Diff in time stamp since last received in order.
  uint32_t timestamp_diff = header.timestamp - last_received_timestamp_;
  int32_t rtp_time_stamp_diff_ms = static_cast<int32_t>(timestamp_diff) /
      frequency_khz;

  int32_t max_delay_ms = 0;
  if (min_rtt == 0) {
    // Jitter standard deviation in samples.
    float jitter_std = sqrt(static_cast<float>(jitter));

    // 2 times the standard deviation => 95% confidence.
    // And transform to milliseconds by dividing by the frequency in kHz.
    max_delay_ms = static_cast<int32_t>((2 * jitter_std) / frequency_khz);

    // Min max_delay_ms is 1.
    if (max_delay_ms == 0) {
      max_delay_ms = 1;
    }
  } else {
    max_delay_ms = (min_rtt / 3) + 1;
  }
  if (time_diff_ms > rtp_time_stamp_diff_ms + max_delay_ms) {
    return true;
  }
  return false;
}

bool RtpReceiverImpl::InOrderPacket(const uint16_t sequence_number) const {
  CriticalSectionScoped cs(critical_section_rtp_receiver_.get());

  // First packet is always in order.
  if (last_receive_time_ == 0)
    return true;

  if (IsNewerSequenceNumber(sequence_number, last_received_sequence_number_)) {
    return true;
  } else {
    // If we have a restart of the remote side this packet is still in order.
    return !IsNewerSequenceNumber(sequence_number,
                                  last_received_sequence_number_ -
                                  max_reordering_threshold_);
  }
}

TelephoneEventHandler* RtpReceiverImpl::GetTelephoneEventHandler() {
  return rtp_media_receiver_->GetTelephoneEventHandler();
}

uint32_t RtpReceiverImpl::Timestamp() const {
  CriticalSectionScoped lock(critical_section_rtp_receiver_.get());
  return last_received_timestamp_;
}

int32_t RtpReceiverImpl::LastReceivedTimeMs() const {
  CriticalSectionScoped lock(critical_section_rtp_receiver_.get());
  return last_received_frame_time_ms_;
}

// Implementation note: must not hold critsect when called.
void RtpReceiverImpl::CheckSSRCChanged(const RTPHeader* rtp_header) {
  bool new_ssrc = false;
  bool re_initialize_decoder = false;
  char payload_name[RTP_PAYLOAD_NAME_SIZE];
  uint8_t channels = 1;
  uint32_t rate = 0;

  {
    CriticalSectionScoped lock(critical_section_rtp_receiver_.get());

    int8_t last_received_payload_type =
        rtp_payload_registry_->last_received_payload_type();
    if (ssrc_ != rtp_header->ssrc ||
        (last_received_payload_type == -1 && ssrc_ == 0)) {
      // We need the payload_type_ to make the call if the remote SSRC is 0.
      new_ssrc = true;

      cb_rtp_feedback_->ResetStatistics(ssrc_);

      last_received_timestamp_ = 0;
      last_received_sequence_number_ = 0;
      last_received_frame_time_ms_ = 0;

      // Do we have a SSRC? Then the stream is restarted.
      if (ssrc_ != 0) {
        // Do we have the same codec? Then re-initialize coder.
        if (rtp_header->payloadType == last_received_payload_type) {
          re_initialize_decoder = true;

          Payload* payload;
          if (!rtp_payload_registry_->PayloadTypeToPayload(
              rtp_header->payloadType, payload)) {
            return;
          }
          assert(payload);
          payload_name[RTP_PAYLOAD_NAME_SIZE - 1] = 0;
          strncpy(payload_name, payload->name, RTP_PAYLOAD_NAME_SIZE - 1);
          if (payload->audio) {
            channels = payload->typeSpecific.Audio.channels;
            rate = payload->typeSpecific.Audio.rate;
          }
        }
      }
      ssrc_ = rtp_header->ssrc;
    }
  }

  if (new_ssrc) {
    // We need to get this to our RTCP sender and receiver.
    // We need to do this outside critical section.
    cb_rtp_feedback_->OnIncomingSSRCChanged(id_, rtp_header->ssrc);
  }

  if (re_initialize_decoder) {
    if (-1 == cb_rtp_feedback_->OnInitializeDecoder(
        id_, rtp_header->payloadType, payload_name,
        rtp_header->payload_type_frequency, channels, rate)) {
      // New stream, same codec.
      WEBRTC_TRACE(kTraceError, kTraceRtpRtcp, id_,
                   "Failed to create decoder for payload type:%d",
                   rtp_header->payloadType);
    }
  }
}

// Implementation note: must not hold critsect when called.
// TODO(phoglund): Move as much as possible of this code path into the media
// specific receivers. Basically this method goes through a lot of trouble to
// compute something which is only used by the media specific parts later. If
// this code path moves we can get rid of some of the rtp_receiver ->
// media_specific interface (such as CheckPayloadChange, possibly get/set
// last known payload).
int32_t RtpReceiverImpl::CheckPayloadChanged(
  const RTPHeader* rtp_header,
  const int8_t first_payload_byte,
  bool& is_red,
  PayloadUnion* specific_payload,
  bool* should_reset_statistics) {
  bool re_initialize_decoder = false;

  char payload_name[RTP_PAYLOAD_NAME_SIZE];
  int8_t payload_type = rtp_header->payloadType;

  {
    CriticalSectionScoped lock(critical_section_rtp_receiver_.get());

    int8_t last_received_payload_type =
        rtp_payload_registry_->last_received_payload_type();
    if (payload_type != last_received_payload_type) {
      if (rtp_payload_registry_->red_payload_type() == payload_type) {
        // Get the real codec payload type.
        payload_type = first_payload_byte & 0x7f;
        is_red = true;

        if (rtp_payload_registry_->red_payload_type() == payload_type) {
          // Invalid payload type, traced by caller. If we proceeded here,
          // this would be set as |_last_received_payload_type|, and we would no
          // longer catch corrupt packets at this level.
          return -1;
        }

        // When we receive RED we need to check the real payload type.
        if (payload_type == last_received_payload_type) {
          rtp_media_receiver_->GetLastMediaSpecificPayload(specific_payload);
          return 0;
        }
      }
      *should_reset_statistics = false;
      bool should_discard_changes = false;

      rtp_media_receiver_->CheckPayloadChanged(
        payload_type, specific_payload, should_reset_statistics,
        &should_discard_changes);

      if (should_discard_changes) {
        is_red = false;
        return 0;
      }

      Payload* payload;
      if (!rtp_payload_registry_->PayloadTypeToPayload(payload_type, payload)) {
        // Not a registered payload type.
        return -1;
      }
      assert(payload);
      payload_name[RTP_PAYLOAD_NAME_SIZE - 1] = 0;
      strncpy(payload_name, payload->name, RTP_PAYLOAD_NAME_SIZE - 1);

      rtp_payload_registry_->set_last_received_payload_type(payload_type);

      re_initialize_decoder = true;

      rtp_media_receiver_->SetLastMediaSpecificPayload(payload->typeSpecific);
      rtp_media_receiver_->GetLastMediaSpecificPayload(specific_payload);

      if (!payload->audio) {
        if (VideoCodecType() == kRtpVideoFec) {
          // Only reset the decoder on media packets.
          re_initialize_decoder = false;
        } else {
          bool media_type_unchanged =
              rtp_payload_registry_->ReportMediaPayloadType(payload_type);
          if (media_type_unchanged) {
            // Only reset the decoder if the media codec type has changed.
            re_initialize_decoder = false;
          }
        }
      }
      if (re_initialize_decoder) {
        *should_reset_statistics = true;
      }
    } else {
      rtp_media_receiver_->GetLastMediaSpecificPayload(specific_payload);
      is_red = false;
    }
  }  // End critsect.

  if (re_initialize_decoder) {
    if (-1 == rtp_media_receiver_->InvokeOnInitializeDecoder(
        cb_rtp_feedback_, id_, payload_type, payload_name,
        *specific_payload)) {
      return -1;  // Wrong payload type.
    }
  }
  return 0;
}

// Implementation note: must not hold critsect when called.
void RtpReceiverImpl::CheckCSRC(const WebRtcRTPHeader* rtp_header) {
  int32_t num_csrcs_diff = 0;
  uint32_t old_remote_csrc[kRtpCsrcSize];
  uint8_t old_num_csrcs = 0;

  {
    CriticalSectionScoped lock(critical_section_rtp_receiver_.get());

    if (!rtp_media_receiver_->ShouldReportCsrcChanges(
        rtp_header->header.payloadType)) {
      return;
    }
    old_num_csrcs  = num_csrcs_;
    if (old_num_csrcs > 0) {
      // Make a copy of old.
      memcpy(old_remote_csrc, current_remote_csrc_,
             num_csrcs_ * sizeof(uint32_t));
    }
    const uint8_t num_csrcs = rtp_header->header.numCSRCs;
    if ((num_csrcs > 0) && (num_csrcs <= kRtpCsrcSize)) {
      // Copy new.
      memcpy(current_remote_csrc_,
             rtp_header->header.arrOfCSRCs,
             num_csrcs * sizeof(uint32_t));
    }
    if (num_csrcs > 0 || old_num_csrcs > 0) {
      num_csrcs_diff = num_csrcs - old_num_csrcs;
      num_csrcs_ = num_csrcs;  // Update stored CSRCs.
    } else {
      // No change.
      return;
    }
  }  // End critsect.

  bool have_called_callback = false;
  // Search for new CSRC in old array.
  for (uint8_t i = 0; i < rtp_header->header.numCSRCs; ++i) {
    const uint32_t csrc = rtp_header->header.arrOfCSRCs[i];

    bool found_match = false;
    for (uint8_t j = 0; j < old_num_csrcs; ++j) {
      if (csrc == old_remote_csrc[j]) {  // old list
        found_match = true;
        break;
      }
    }
    if (!found_match && csrc) {
      // Didn't find it, report it as new.
      have_called_callback = true;
      cb_rtp_feedback_->OnIncomingCSRCChanged(id_, csrc, true);
    }
  }
  // Search for old CSRC in new array.
  for (uint8_t i = 0; i < old_num_csrcs; ++i) {
    const uint32_t csrc = old_remote_csrc[i];

    bool found_match = false;
    for (uint8_t j = 0; j < rtp_header->header.numCSRCs; ++j) {
      if (csrc == rtp_header->header.arrOfCSRCs[j]) {
        found_match = true;
        break;
      }
    }
    if (!found_match && csrc) {
      // Did not find it, report as removed.
      have_called_callback = true;
      cb_rtp_feedback_->OnIncomingCSRCChanged(id_, csrc, false);
    }
  }
  if (!have_called_callback) {
    // If the CSRC list contain non-unique entries we will end up here.
    // Using CSRC 0 to signal this event, not interop safe, other
    // implementations might have CSRC 0 as a valid value.
    if (num_csrcs_diff > 0) {
      cb_rtp_feedback_->OnIncomingCSRCChanged(id_, 0, true);
    } else if (num_csrcs_diff < 0) {
      cb_rtp_feedback_->OnIncomingCSRCChanged(id_, 0, false);
    }
  }
}

}  // namespace webrtc
