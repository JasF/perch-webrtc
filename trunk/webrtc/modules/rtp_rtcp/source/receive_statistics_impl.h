/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_RTP_RTCP_SOURCE_RECEIVE_STATISTICS_IMPL_H_
#define WEBRTC_MODULES_RTP_RTCP_SOURCE_RECEIVE_STATISTICS_IMPL_H_

#include "webrtc/modules/rtp_rtcp/interface/receive_statistics.h"

#include <algorithm>

#include "webrtc/modules/rtp_rtcp/source/bitrate.h"
#include "webrtc/system_wrappers/interface/critical_section_wrapper.h"
#include "webrtc/system_wrappers/interface/scoped_ptr.h"

namespace webrtc {

class CriticalSectionWrapper;

class StreamStatisticianImpl : public StreamStatistician {
 public:
  explicit StreamStatisticianImpl(Clock* clock);

  virtual ~StreamStatisticianImpl() {}

  virtual bool GetStatistics(Statistics* statistics, bool reset) OVERRIDE;
  virtual void GetDataCounters(uint32_t* bytes_received,
                               uint32_t* packets_received) const OVERRIDE;
  virtual uint32_t BitrateReceived() const OVERRIDE;
  virtual void ResetStatistics() OVERRIDE;

  void IncomingPacket(const RTPHeader& rtp_header, size_t bytes,
                      bool retransmitted, bool in_order);
  void ProcessBitrate();
  virtual void LastReceiveTimeNtp(uint32_t* secs, uint32_t* frac) const;

 private:
  Clock* clock_;
  scoped_ptr<CriticalSectionWrapper> crit_sect_;
  Bitrate incoming_bitrate_;
  uint32_t ssrc_;

  // Stats on received RTP packets.
  uint32_t jitter_q4_;
  uint32_t jitter_max_q4_;
  uint32_t cumulative_loss_;
  uint32_t jitter_q4_transmission_time_offset_;

  uint32_t last_receive_time_secs_;
  uint32_t last_receive_time_frac_;
  uint32_t last_received_timestamp_;
  int32_t last_received_transmission_time_offset_;
  uint16_t received_seq_first_;
  uint16_t received_seq_max_;
  uint16_t received_seq_wraps_;
  bool first_packet_;

  // Current counter values.
  uint16_t received_packet_overhead_;
  uint32_t received_byte_count_;
  uint32_t received_retransmitted_packets_;
  uint32_t received_inorder_packet_count_;

  // Counter values when we sent the last report.
  uint32_t last_report_inorder_packets_;
  uint32_t last_report_old_packets_;
  uint16_t last_report_seq_max_;
  Statistics last_reported_statistics_;
};

class ReceiveStatisticsImpl : public ReceiveStatistics {
 public:
  explicit ReceiveStatisticsImpl(Clock* clock);

  ~ReceiveStatisticsImpl();

  // Implement ReceiveStatistics.
  virtual void IncomingPacket(const RTPHeader& header, size_t bytes,
                      bool old_packet, bool in_order) OVERRIDE;
  virtual StatisticianMap GetActiveStatisticians() const OVERRIDE;
  virtual StreamStatistician* GetStatistician(uint32_t ssrc) const OVERRIDE;

  // Implement Module.
  virtual int32_t Process() OVERRIDE;
  virtual int32_t TimeUntilNextProcess() OVERRIDE;

  void ChangeSsrc(uint32_t from_ssrc, uint32_t to_ssrc);

 private:
  typedef std::map<uint32_t, StreamStatisticianImpl*> StatisticianImplMap;

  Clock* clock_;
  scoped_ptr<CriticalSectionWrapper> crit_sect_;
  int64_t last_rate_update_ms_;
  StatisticianImplMap statisticians_;
};
}  // namespace webrtc
#endif  // WEBRTC_MODULES_RTP_RTCP_SOURCE_RECEIVE_STATISTICS_IMPL_H_
