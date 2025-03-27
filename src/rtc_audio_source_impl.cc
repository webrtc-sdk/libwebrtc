#include "rtc_audio_source_impl.h"

#include "pc/local_audio_source.h"

namespace libwebrtc {

RTCAudioSourceImpl::RTCAudioSourceImpl(
    rtc::scoped_refptr<webrtc::AudioSourceInterface> rtc_audio_source)
    : rtc_audio_source_(rtc_audio_source) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor ";
}

RTCAudioSourceImpl::~RTCAudioSourceImpl() {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}

class AdaptedVirtualAudioCapturer : public webrtc::LocalAudioSource {
 public:
  AdaptedVirtualAudioCapturer() {}
  ~AdaptedVirtualAudioCapturer() {}

  void AddSink(webrtc::AudioTrackSinkInterface* sink) override {
    webrtc::MutexLock lock(&mutex_);
    sinks_.push_back(sink);
  }

  void RemoveSink(webrtc::AudioTrackSinkInterface* sink) override {
    webrtc::MutexLock lock(&mutex_);
    sinks_.erase(std::remove(sinks_.begin(), sinks_.end(), sink), sinks_.end());
  }

  void OnFrame(scoped_refptr<RTCAudioFrame> frame) {
    webrtc::MutexLock lock(&mutex_);
    for (auto sink : sinks_) {
      sink->OnData((const void*)frame->data(), 16, frame->sample_rate_hz(),
                   frame->num_channels(), frame->samples_per_channel());
    }
  }

  void OnData(const void* audio_data, int bits_per_sample, int sample_rate,
              size_t number_of_channels, size_t number_of_frames) {
    webrtc::MutexLock lock(&mutex_);
    for (auto sink : sinks_) {
      sink->OnData(audio_data, bits_per_sample, sample_rate, number_of_channels,
                   number_of_frames);
    }
  }

 private:
  mutable webrtc::Mutex mutex_;
  std::vector<webrtc::AudioTrackSinkInterface*> sinks_;
};

class VirtualAudioCapturerImpl : public VirtualAudioCapturer {
 public:
  VirtualAudioCapturerImpl() {}
  virtual ~VirtualAudioCapturerImpl() {}

  virtual void OnFrame(scoped_refptr<RTCAudioFrame> frame) override {
    adapted_source_->OnFrame(frame);
  }

  virtual void OnData(const void* audio_data, int bits_per_sample,
                      int sample_rate, size_t number_of_channels,
                      size_t number_of_frames) override {
    adapted_source_->OnData(audio_data, bits_per_sample, sample_rate,
                            number_of_channels, number_of_frames);
  }

  virtual scoped_refptr<RTCAudioSource> source() override {
    return rtc_audio_source_;
  }

 private:
  scoped_refptr<RTCAudioSourceImpl> rtc_audio_source_;
  rtc::scoped_refptr<AdaptedVirtualAudioCapturer> adapted_source_;
};

scoped_refptr<VirtualAudioCapturer> VirtualAudioCapturer::Create() {
  return scoped_refptr<VirtualAudioCapturer>(
      new RefCountedObject<VirtualAudioCapturerImpl>());
}

}  // namespace libwebrtc
