#include "audio_util.h"

#include "rtc_types.h"

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
    uint32_t src_num_channels)
{
  if (!dst_data || 
      !src_data ||
      (dst_bits_per_sample % 8 != 0) ||
      (dst_bits_per_sample < 8) ||
      (dst_bits_per_sample > 32) ||
      (src_bits_per_sample % 8 != 0) ||
      (src_bits_per_sample < 8) ||
      (src_bits_per_sample > 32) ||
      (dst_samples_per_channel < 160) ||
      (src_samples_per_channel < 160) ||
      (dst_sample_rate < 16) ||
      (src_sample_rate < 16) ||
      (dst_num_channels < 1) ||
      (src_num_channels < 1)
     )
  {
    return 0;
  }

  uint32_t result = 0;

  uint32_t src_bytes_per_sample = src_bits_per_sample / 8;
  uint32_t dst_bytes_per_sample = dst_bits_per_sample / 8;

  float sample_rate_scale = static_cast<float>(dst_sample_rate) / src_sample_rate;
  float channel_scale = static_cast<float>(dst_num_channels) / src_num_channels;

  for (uint32_t dst_sample = 0; dst_sample < dst_samples_per_channel; ++dst_sample) {
    uint32_t src_sample = static_cast<uint32_t>(dst_sample / sample_rate_scale);

    if (src_sample >= src_samples_per_channel) {
      break;
    }

    for (uint32_t dst_channel = 0; dst_channel < dst_num_channels; ++dst_channel) {
      uint32_t src_channel = static_cast<uint32_t>(dst_channel / channel_scale);
      if (src_channel >= src_num_channels) {
        break;
      }

      const uint8_t* src_sample_ptr = src_data +
          (src_sample * src_num_channels + src_channel) * src_bytes_per_sample;

      uint8_t* dst_sample_ptr = dst_data +
          (dst_sample * dst_num_channels + dst_channel) * dst_bytes_per_sample;

      if (src_bits_per_sample == dst_bits_per_sample) {
        memcpy(dst_sample_ptr, src_sample_ptr, dst_bytes_per_sample);
      } else if (src_bits_per_sample < dst_bits_per_sample) {
        int32_t sample = 0;
        memcpy(&sample, src_sample_ptr, src_bytes_per_sample);
        sample = static_cast<int32_t>(
            sample * (1 << (dst_bits_per_sample - src_bits_per_sample))
        );
        memcpy(dst_sample_ptr, &sample, dst_bytes_per_sample);
      } else {
        int32_t sample = 0;
        memcpy(&sample, src_sample_ptr, src_bytes_per_sample);
        sample = static_cast<int32_t>(
            sample / (1 << (src_bits_per_sample - dst_bits_per_sample))
        );
        memcpy(dst_sample_ptr, &sample, dst_bytes_per_sample);
      }

      result += dst_bytes_per_sample;
    }
  }

  return result;
}

}  // namespace libwebrtc
