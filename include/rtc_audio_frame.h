#ifndef LIB_WEBRTC_RTC_AUDIO_FRAME_HXX
#define LIB_WEBRTC_RTC_AUDIO_FRAME_HXX

#include "rtc_types.h"

namespace libwebrtc {

class RTCAudioFrame : public RefCountInterface {
 public:
 LIB_WEBRTC_API static scoped_refptr<RTCAudioFrame> Create();

 LIB_WEBRTC_API static scoped_refptr<RTCAudioFrame> Create(
      uint32_t timestamp, const int16_t* data, size_t samples_per_channel,
      int sample_rate_hz, size_t num_channels = 1);

 public:
  virtual void UpdateFrame(uint32_t timestamp, const int16_t* data,
                           size_t samples_per_channel, int sample_rate_hz,
                           size_t num_channels = 1) = 0;

  virtual void CopyFrom(const scoped_refptr<RTCAudioFrame> src) = 0;

  virtual void Add(const scoped_refptr<RTCAudioFrame> frame_to_add) = 0;

  virtual void Mute() = 0;

  virtual const int16_t* data() = 0;

  virtual size_t samples_per_channel() = 0;

  virtual int sample_rate_hz() = 0;

  virtual size_t num_channels() = 0;

  virtual uint32_t timestamp() = 0;
};

}  // namespace libwebrtc

#endif
