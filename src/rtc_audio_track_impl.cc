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
  RemoveSinks();
}

void AudioTrackImpl::SetVolume(double volume) {
  rtc_track_->GetSource()->SetVolume(volume);
}

void AudioTrackImpl::RemoveSinks() {
  webrtc::MutexLock lock(&mutex_);
  auto it = sinks_.begin();
  while ( it != sinks_.end()) {
    rtc_track_->RemoveSink(it->second.get());
    it++;
  }
  sinks_.clear();
}

}  // namespace libwebrtc
