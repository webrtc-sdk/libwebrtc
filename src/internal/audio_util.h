/**
 * File provided for Reference Use Only by isoft (c) 2025.
 * Copyright (c) isoft. All rights reserved.
 * 
 */

#ifndef LIB_WEBRTC_AUDIO_UTIL_HXX
#define LIB_WEBRTC_AUDIO_UTIL_HXX

#include <inttypes.h>

namespace libwebrtc {

uint32_t AudioScale(
  uint8_t* dst_data,
  uint32_t dst_bits_per_sample,
  uint32_t dst_samples_per_channel,
  int dst_sample_rate,
  uint32_t dst_num_channels,
  const uint8_t* src_data,
  uint32_t src_bits_per_sample,
  uint32_t src_samples_per_channel,
  int src_sample_rate,
  uint32_t src_num_channels
);

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_AUDIO_UTIL_HXX