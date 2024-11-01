#include "api/audio/audio_frame.h"
#include "include/rtc_audio_frame.h"

namespace libwebrtc {

class RTCAudioFrameImpl : public RTCAudioFrame {
 public:
  RTCAudioFrameImpl() {}
  RTCAudioFrameImpl(webrtc::AudioFrame& buffer);
  ~RTCAudioFrameImpl();

 public:
  virtual void UpdateFrame(uint32_t timestamp, const int16_t* data,
                           size_t samples_per_channel, int sample_rate_hz,
                           size_t num_channels = 1) override;

  virtual void CopyFrom(const scoped_refptr<RTCAudioFrame> src) override;

  virtual void Add(const scoped_refptr<RTCAudioFrame> frame_to_add) override;

  virtual void Mute() override;

  virtual const int16_t* data() override;

  virtual size_t samples_per_channel() override;

  virtual int sample_rate_hz() override;

  virtual size_t num_channels() override;

  virtual uint32_t timestamp() override;

 private:
  webrtc::AudioFrame buffer_;
};

}  // namespace libwebrtc
