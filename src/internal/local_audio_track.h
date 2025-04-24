#ifndef INTERNAL_CUSTOM_LOCAL_AUDIO_TRACK_HXX
#define INTERNAL_CUSTOM_LOCAL_AUDIO_TRACK_HXX

#include "api/audio_options.h"
#include "api/media_stream_interface.h"
#include "api/notifier.h"
#include "api/scoped_refptr.h"
#include "call/audio_sender.h"
#include "src/internal/custom_audio_transport_impl.h"

namespace libwebrtc {

using namespace webrtc;

class LocalAudioSource : public Notifier<AudioSourceInterface>, AudioSender {
 public:
  // Creates an instance of CustomLocalAudioSource.
  static rtc::scoped_refptr<LocalAudioSource> Create(
      const cricket::AudioOptions* audio_options,
      webrtc::CustomAudioTransportImpl* audio_transport);

  SourceState state() const override { return kLive; }
  bool remote() const override { return false; }

  const cricket::AudioOptions options() const override { return options_; }

  void AddSink(AudioTrackSinkInterface* sink) override {
    webrtc::MutexLock lock(&sink_lock_);
    if (std::find(sinks_.begin(), sinks_.end(), sink) != sinks_.end()) {
      return;  // Already added.
    }
    sinks_.push_back(sink);
  }

  void RemoveSink(AudioTrackSinkInterface* sink) override {
    webrtc::MutexLock lock(&sink_lock_);
    auto it = std::remove(sinks_.begin(), sinks_.end(), sink);
    if (it != sinks_.end()) {
      sinks_.erase(it, sinks_.end());
    }
  }

  void SendAudioData(std::unique_ptr<AudioFrame> audio_frame) override {
    OnData((const void*)audio_frame->data(), 16, audio_frame->sample_rate_hz(),
           audio_frame->num_channels(), audio_frame->samples_per_channel());
  }

  void OnData(const void* audio_data, int bits_per_sample, int sample_rate,
              size_t number_of_channels, size_t number_of_frames) {
    webrtc::MutexLock lock(&sink_lock_);
    for (auto* sink : sinks_) {
      sink->OnData(audio_data, bits_per_sample, sample_rate, number_of_channels,
                   number_of_frames);
    }
  }

 protected:
  LocalAudioSource(webrtc::CustomAudioTransportImpl* audio_transport)
      : audio_transport_(audio_transport) {
    if (audio_transport_) {
      audio_transport_->AddAudioSender(this);
    }
  }
  ~LocalAudioSource() override {
    webrtc::MutexLock lock(&sink_lock_);
    if (audio_transport_) {
      audio_transport_->RemoveAudioSender(this);
    }
    sinks_.clear();
  }

 private:
  void Initialize(const cricket::AudioOptions* audio_options);
  mutable webrtc::Mutex sink_lock_;
  std::vector<AudioTrackSinkInterface*> sinks_ RTC_GUARDED_BY(sink_lock_);
  cricket::AudioOptions options_;
  webrtc::CustomAudioTransportImpl* audio_transport_;
};
}  // namespace libwebrtc

#endif  // INTERNAL_CUSTOM_LOCAL_AUDIO_TRACK_HXX
