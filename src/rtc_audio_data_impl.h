#ifndef LIB_WEBRTC_RTC_AUDIO_DATA_IMPL_HXX
#define LIB_WEBRTC_RTC_AUDIO_DATA_IMPL_HXX

#include <stdint.h>
#include <memory>

#include "rtc_base/memory/aligned_malloc.h"
#include "rtc_audio_data.h"

namespace libwebrtc {

class RTCAudioDataImpl : public RTCAudioData {
 public:
  RTCAudioDataImpl(
    uint32_t index,
    const uint8_t* data,
    uint32_t bits_per_sample,
    int sample_rate_hz,
    uint32_t num_channels
  );
  virtual ~RTCAudioDataImpl();

 public:
  uint32_t index() const override { return index_; }
  void set_index(uint32_t index) override { index_ = index; }
  const uint8_t* data() const override { return data_.get(); }
  size_t data_size() const override { return data_size_; }
  uint32_t bits_per_sample() const override { return bits_per_sample_; }
  uint32_t samples_per_channel() const override { return samples_per_channel_; }
  int sample_rate_hz() const override { return sample_rate_hz_; }
  uint32_t num_channels() const override { return num_channels_; }

  int ScaleFrom(scoped_refptr<RTCAudioData> src) override;

  int Clear(RTCAudioDataToneFrequency frequency = RTCAudioDataToneFrequency::kNone) override;

 private:
  uint32_t index_;
  uint32_t bits_per_sample_;
  uint32_t samples_per_channel_;
  int sample_rate_hz_;
  uint32_t num_channels_;
  size_t total_data_size_;
  size_t data_size_;
  std::unique_ptr<uint8_t, webrtc::AlignedFreeDeleter> data_;
}; // end class RTCAudioDataImpl

}  // namespace libwebrtc

#endif // LIB_WEBRTC_RTC_AUDIO_DATA_IMPL_HXX
