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

void AudioTrackImpl::SetVolume(double volume) {
  rtc_track_->GetSource()->SetVolume(volume);
}

void AudioTrackImpl::AddAudioSink(
    RTCAudioRenderer<scoped_refptr<RTCAudioFrame>>* sink) {
  webrtc::MutexLock lock(&mutex_);
  renderers_.push_back(sink);
}

void AudioTrackImpl::RemoveAudioSink(
    RTCAudioRenderer<scoped_refptr<RTCAudioFrame>>* sink) {
  webrtc::MutexLock lock(&mutex_);
  renderers_.erase(
      std::remove_if(
          renderers_.begin(), renderers_.end(),
          [sink](const RTCAudioRenderer<scoped_refptr<RTCAudioFrame>>* sink_) {
            return sink_ == sink;
          }),
      renderers_.end());
}

void AudioTrackImpl::OnData(const void* audio_data, int bits_per_sample,
                            int sample_rate, size_t number_of_channels,
                            size_t number_of_frames) {
  webrtc::MutexLock lock(&mutex_);
  for (auto sink : renderers_) {
    sink->OnData(audio_data, bits_per_sample, sample_rate,
                                number_of_channels, number_of_frames);
  }
}

}  // namespace libwebrtc
