#include "rtc_audio_source_impl.h"

namespace libwebrtc {

RTCAudioSourceImpl::RTCAudioSourceImpl(
    rtc::scoped_refptr<libwebrtc::LocalAudioSource> rtc_audio_source,
    SourceType source_type)
    : rtc_audio_source_(rtc_audio_source), source_type_(source_type) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor ";
}

RTCAudioSourceImpl::~RTCAudioSourceImpl() {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}

}  // namespace libwebrtc
