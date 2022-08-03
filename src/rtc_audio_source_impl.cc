#include "rtc_audio_source_impl.h"

namespace libwebrtc {

RTCAudioSourceImpl::RTCAudioSourceImpl(
    rtc::scoped_refptr<webrtc::AudioSourceInterface> rtc_audio_source)
    : rtc_audio_source_(rtc_audio_source) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor ";
}

RTCAudioSourceImpl::~RTCAudioSourceImpl() {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}

}  // namespace libwebrtc
