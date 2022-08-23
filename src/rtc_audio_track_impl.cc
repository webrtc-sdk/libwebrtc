#include "rtc_audio_track_impl.h"

namespace libwebrtc {

AudioTrackImpl::AudioTrackImpl(
    rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track)
    : rtc_track_(audio_track) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor ";
  id_ = rtc_track_->id();
  kind_ = rtc_track_->kind();
}

AudioTrackImpl::~AudioTrackImpl() {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}

}  // namespace libwebrtc
