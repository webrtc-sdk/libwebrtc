#ifndef INTERNAL_CUSTOM_AUDIO_TRANSPORT_STATE_H_
#define INTERNAL_CUSTOM_AUDIO_TRANSPORT_STATE_H_

#include <map>
#include <memory>

#include "api/sequence_checker.h"
#include "audio/audio_transport_impl.h"
#include "call/audio_sender.h"
#include "call/audio_state.h"
#include "rtc_base/containers/flat_set.h"
#include "rtc_base/ref_count.h"
#include "rtc_base/task_utils/repeating_task.h"
#include "rtc_base/thread_annotations.h"

namespace webrtc {

class CustomAudioTransportImpl : public AudioTransport, public AudioSender {
 public:
  CustomAudioTransportImpl(
      AudioMixer* mixer, AudioProcessing* audio_processing,
      AsyncAudioProcessing::Factory* async_audio_processing_factory);

  int32_t RecordedDataIsAvailable(const void* audioSamples, size_t nSamples,
                                  size_t nBytesPerSample, size_t nChannels,
                                  uint32_t samplesPerSec, uint32_t totalDelayMS,
                                  int32_t clockDrift, uint32_t currentMicLevel,
                                  bool keyPressed,
                                  uint32_t& newMicLevel) override;

  int32_t RecordedDataIsAvailable(
      const void* audioSamples, size_t nSamples, size_t nBytesPerSample,
      size_t nChannels, uint32_t samplesPerSec, uint32_t totalDelayMS,
      int32_t clockDrift, uint32_t currentMicLevel, bool keyPressed,
      uint32_t& newMicLevel,
      absl::optional<int64_t> estimated_capture_time_ns) override;

  int32_t NeedMorePlayData(size_t nSamples, size_t nBytesPerSample,
                           size_t nChannels, uint32_t samplesPerSec,
                           void* audioSamples, size_t& nSamplesOut,
                           int64_t* elapsed_time_ms,
                           int64_t* ntp_time_ms) override;

  void PullRenderData(int bits_per_sample, int sample_rate,
                      size_t number_of_channels, size_t number_of_frames,
                      void* audio_data, int64_t* elapsed_time_ms,
                      int64_t* ntp_time_ms) override;

  virtual void UpdateAudioSenders(std::vector<AudioSender*> senders,
                                  int send_sample_rate_hz,
                                  size_t send_num_channels);

  void AddAudioSender(AudioSender* sender);

  void RemoveAudioSender(AudioSender* sender);

  void SetStereoChannelSwapping(bool enable);

  void SendAudioData(std::unique_ptr<AudioFrame> audio_frame) override;

 private:
  webrtc::AudioTransportImpl audio_transport_impl_;
  mutable Mutex capture_lock_;
  std::vector<AudioSender*> audio_senders_ RTC_GUARDED_BY(capture_lock_);
};

}  // namespace libwebrtc

#endif  // INTERNAL_CUSTOM_AUDIO_TRANSPORT_STATE_H_
