#ifndef LIB_WEBRTC_RTC_AUDIO_PROCESSING_IMPL_HXX
#define LIB_WEBRTC_RTC_AUDIO_PROCESSING_IMPL_HXX

#include <memory>

#include "modules/audio_processing/include/audio_processing.h"
#include "rtc_audio_processing.h"

namespace libwebrtc {

class CustomProcessingAdapter;

class RTCAudioProcessingImpl : public RTCAudioProcessing {
 public:
  RTCAudioProcessingImpl();
  ~RTCAudioProcessingImpl();
  void SetCapturePostProcessing(
      RTCAudioProcessing::CustomProcessing* capture_post_processing) override;

  void SetRenderPreProcessing(
      RTCAudioProcessing::CustomProcessing* render_pre_processing) override;

  virtual rtc::scoped_refptr<webrtc::AudioProcessing> GetAudioProcessing() {
    return apm_;
  }

 private:
  CustomProcessingAdapter* capture_post_processor_;
  CustomProcessingAdapter* render_pre_processor_;
  rtc::scoped_refptr<webrtc::AudioProcessing> apm_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_AUDIO_PROCESSING_IMPL_HXX
