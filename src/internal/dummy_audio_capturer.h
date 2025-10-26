#ifndef INTERNAL_DUMMY_AUDIO_CAPTURER_H_
#define INTERNAL_DUMMY_AUDIO_CAPTURER_H_

#include <list>

#include "api/media_stream_interface.h"
#include "api/task_queue/task_queue_factory.h"
#include "rtc_base/thread.h"
#include "rtc_types.h"
#include "rtc_audio_data.h"
#include "rtc_dummy_audio_source.h"

namespace webrtc {
namespace internal {

using DummyAudioSourceObserver = libwebrtc::DummyAudioSourceObserver;
using RTCCaptureState = libwebrtc::RTCCaptureState;
using RTCAudioData = libwebrtc::RTCAudioData;
using RTCAudioDataPtr = libwebrtc::scoped_refptr<libwebrtc::RTCAudioData>;

/**
 * class DummyAudioCapturer
 */
class DummyAudioCapturer : public AudioSourceInterface {
 public:
  DummyAudioCapturer(
    TaskQueueFactory* task_queue_factory,
    rtc::Thread* signaling_thread,
    int bits_per_sample,
    int sample_rate_hz,
    size_t number_of_channels,
    const std::string name
  );

  virtual ~DummyAudioCapturer();

  virtual void SetVolume(double volume) override {}

  virtual void RegisterAudioObserver(AudioObserver* observer) override {}
  virtual void UnregisterAudioObserver(AudioObserver* observer) override {}

  virtual void RegisterObserver(ObserverInterface* observer) override {}
  virtual void UnregisterObserver(ObserverInterface* observer) override {}

  virtual const cricket::AudioOptions options() const override {
    return {};
  }

  virtual MediaSourceInterface::SourceState state() const override {
    return MediaSourceInterface::SourceState::kLive;
  }

  virtual bool remote() const override { return false; }

  void RegisterAudioDataObserver(DummyAudioSourceObserver* observer) {
    observer_ = observer;
  }

  void DeRegisterAudioDataObserver() {
    observer_ = nullptr;
  }

  int bits_per_sample() const { return bits_per_sample_; }
  int sample_rate_hz() const { return sample_rate_hz_; }
  size_t number_of_channels() { return number_of_channels_; }
  size_t number_of_frames() { return number_of_frames_; }

  const char* name() const { return name_.c_str(); }

  RTCCaptureState Start();
  void Stop();
  RTCCaptureState CaptureState();
  bool IsRunning() const;

  DummyAudioSourceObserver* GetObserver() { return observer_; }

  void AddSink(AudioTrackSinkInterface* sink) override;
  void RemoveSink(AudioTrackSinkInterface* sink) override;

 protected:
  void OnData(const void* audio_data,
              int bits_per_sample,
              int sample_rate,
              size_t number_of_channels,
              size_t number_of_frames);

 private:
  void OnFrameReady(const RTCAudioDataPtr& audio_data);
  void FillBuffer();
  void CaptureFrame();

 private:
  std::unique_ptr<TaskQueueBase, TaskQueueDeleter> task_queue_;
  rtc::Thread* signaling_thread_ = nullptr;
  std::string name_;
  uint32_t sample_index_ = 0;
  int bits_per_sample_ = 16;
  int sample_rate_hz_ = 44100;
  size_t number_of_channels_ = 2;
  size_t number_of_frames_ = 441;
  RTCCaptureState capture_state_ = RTCCaptureState::CS_STOPPED;
  uint32_t capture_delay_ = 1000;  // 1s
  std::list<AudioTrackSinkInterface*> sinks_;
  std::list<RTCAudioDataPtr> datas_;
  DummyAudioSourceObserver* observer_ = nullptr;
}; // end class DummyAudioCapturer

}  // namespace internal
}  // namespace webrtc

#endif // INTERNAL_DUMMY_AUDIO_CAPTURER_H_
