#include "rtc_audio_track_impl.h"

namespace libwebrtc {

AudioTrackImpl::AudioTrackImpl(
    rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track)
    : rtc_track_(audio_track) {
  RTC_LOG(INFO) << __FUNCTION__ << ": ctor ";
  snprintf(id_, kMaxStringLength, rtc_track_->id().c_str());
  snprintf(kind_, kMaxStringLength, rtc_track_->kind().c_str());
}

AudioTrackImpl::~AudioTrackImpl() {
  RTC_LOG(INFO) << __FUNCTION__ << ": dtor ";
}

} // namespace libwebrtc
