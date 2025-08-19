#ifndef LIB_WEBRTC_AUDIO_TRACK_IMPL_HXX
#define LIB_WEBRTC_AUDIO_TRACK_IMPL_HXX

#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include "common_audio/resampler/include/push_resampler.h"
#include "common_audio/vad/include/webrtc_vad.h"
#include "media/engine/webrtc_video_engine.h"
#include "media/engine/webrtc_voice_engine.h"
#include "pc/media_session.h"
#include "rtc_audio_track.h"
#include "rtc_base/logging.h"
#include "rtc_base/synchronization/mutex.h"

namespace libwebrtc {

class AudioTrackSinkAdapter : public webrtc::AudioTrackSinkInterface {
 public:
  AudioTrackSinkAdapter(AudioTrackSink* sink) : sink_(sink) {}

  void OnData(const void* audio_data, int bits_per_sample, int sample_rate,
              size_t number_of_channels, size_t number_of_frames) override {
    if (sink_) {
      sink_->OnData(audio_data, bits_per_sample, sample_rate,
                    number_of_channels, number_of_frames);
    }
  }

 private:
  AudioTrackSink* sink_;
};

class AudioTrackImpl : public RTCAudioTrack {
 public:
  AudioTrackImpl(webrtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track);

  virtual ~AudioTrackImpl();

  virtual void SetVolume(double volume) override;

  virtual const string kind() const override { return kind_; }

  virtual const string id() const override { return id_; }

  virtual bool enabled() const override { return rtc_track_->enabled(); }

  virtual bool set_enabled(bool enable) override {
    return rtc_track_->set_enabled(enable);
  }

  virtual void AddSink(AudioTrackSink* sink) override {
    webrtc::MutexLock lock(&mutex_);
    if (sinks_.find(sink) != sinks_.end()) {
      return;
    }
    auto adapter = std::make_unique<AudioTrackSinkAdapter>(sink);
    rtc_track_->AddSink(adapter.get());
    sinks_[sink] = std::move(adapter);
  }

  virtual void RemoveSink(AudioTrackSink* sink) override {
    webrtc::MutexLock lock(&mutex_);
    auto it = sinks_.find(sink);
    if (it == sinks_.end()) {
      return;
    }
    rtc_track_->RemoveSink(it->second.get());
    sinks_.erase(it);
  }

  webrtc::scoped_refptr<webrtc::AudioTrackInterface> rtc_track() {
    return rtc_track_;
  }

  virtual RTCTrackState state() const override {
    return static_cast<RTCTrackState>(rtc_track_->state());
  }

 private:
  void RemoveSinks();
  webrtc::scoped_refptr<webrtc::AudioTrackInterface> rtc_track_;
  std::map<AudioTrackSink*, std::unique_ptr<AudioTrackSinkAdapter>> sinks_;
  webrtc::Mutex mutex_;
  string id_, kind_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_AUDIO_TRACK_IMPL_HXX
