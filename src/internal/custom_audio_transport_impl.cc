#include "src/internal/custom_audio_transport_impl.h"

namespace webrtc {

CustomAudioTransportImpl::CustomAudioTransportImpl(
    AudioMixer* mixer, AudioProcessing* audio_processing,
    AsyncAudioProcessing::Factory* async_audio_processing_factory)
    : audio_transport_impl_(mixer, audio_processing,
                            async_audio_processing_factory) {}

// TODO(bugs.webrtc.org/13620) Deprecate this function
int32_t CustomAudioTransportImpl::RecordedDataIsAvailable(
    const void* audioSamples, size_t nSamples, size_t nBytesPerSample,
    size_t nChannels, uint32_t samplesPerSec, uint32_t totalDelayMS,
    int32_t clockDrift, uint32_t currentMicLevel, bool keyPressed,
    uint32_t& newMicLevel) {
  return audio_transport_impl_.RecordedDataIsAvailable(
      audioSamples, nSamples, nBytesPerSample, nChannels, samplesPerSec,
      totalDelayMS, clockDrift, currentMicLevel, keyPressed, newMicLevel);
}

int32_t CustomAudioTransportImpl::RecordedDataIsAvailable(
    const void* audioSamples, size_t nSamples, size_t nBytesPerSample,
    size_t nChannels, uint32_t samplesPerSec, uint32_t totalDelayMS,
    int32_t clockDrift, uint32_t currentMicLevel, bool keyPressed,
    uint32_t& newMicLevel, absl::optional<int64_t> estimated_capture_time_ns) {
  return audio_transport_impl_.RecordedDataIsAvailable(
      audioSamples, nSamples, nBytesPerSample, nChannels, samplesPerSec,
      totalDelayMS, clockDrift, currentMicLevel, keyPressed, newMicLevel,
      estimated_capture_time_ns);
}

int32_t CustomAudioTransportImpl::NeedMorePlayData(
    size_t nSamples, size_t nBytesPerSample, size_t nChannels,
    uint32_t samplesPerSec, void* audioSamples, size_t& nSamplesOut,
    int64_t* elapsed_time_ms, int64_t* ntp_time_ms) {
  return audio_transport_impl_.NeedMorePlayData(
      nSamples, nBytesPerSample, nChannels, samplesPerSec, audioSamples,
      nSamplesOut, elapsed_time_ms, ntp_time_ms);
}

void CustomAudioTransportImpl::PullRenderData(
    int bits_per_sample, int sample_rate, size_t number_of_channels,
    size_t number_of_frames, void* audio_data, int64_t* elapsed_time_ms,
    int64_t* ntp_time_ms) {
  audio_transport_impl_.PullRenderData(
      bits_per_sample, sample_rate, number_of_channels, number_of_frames,
      audio_data, elapsed_time_ms, ntp_time_ms);
}

void CustomAudioTransportImpl::UpdateAudioSenders(
    std::vector<AudioSender*> senders, int send_sample_rate_hz,
    size_t send_num_channels) {
  if (senders.size() > 0) {
    std::vector<AudioSender*> snds = std::vector<AudioSender*>();
    snds.push_back(this);
    audio_transport_impl_.UpdateAudioSenders(
        std::move(snds), send_sample_rate_hz, send_num_channels);
  } else {
    std::vector<AudioSender*> snds = std::vector<AudioSender*>();
    audio_transport_impl_.UpdateAudioSenders(
        std::move(snds), send_sample_rate_hz, send_num_channels);
  }
}

void CustomAudioTransportImpl::AddAudioSender(AudioSender* sender) {
  MutexLock lock(&capture_lock_);
  audio_senders_.push_back(sender);
}

void CustomAudioTransportImpl::RemoveAudioSender(AudioSender* sender) {
  MutexLock lock(&capture_lock_);
  auto it = std::remove(audio_senders_.begin(), audio_senders_.end(), sender);
  if (it != audio_senders_.end()) {
    audio_senders_.erase(it, audio_senders_.end());
  }
}

void CustomAudioTransportImpl::SetStereoChannelSwapping(bool enable) {
  audio_transport_impl_.SetStereoChannelSwapping(enable);
}

void CustomAudioTransportImpl::SendAudioData(
    std::unique_ptr<AudioFrame> audio_frame) {
  RTC_DCHECK_GT(audio_frame->samples_per_channel_, 0);
  MutexLock lock(&capture_lock_);
  if (audio_senders_.empty()) return;

  auto it = audio_senders_.begin();
  while (++it != audio_senders_.end()) {
    auto audio_frame_copy = std::make_unique<AudioFrame>();
    audio_frame_copy->CopyFrom(*audio_frame);
    (*it)->SendAudioData(std::move(audio_frame_copy));
  }
  // Send the original frame to the first stream w/o copying.
  (*audio_senders_.begin())->SendAudioData(std::move(audio_frame));
}
}  // namespace webrtc