#include "src/internal/dummy_audio_capturer.h"

#include "rtc_base/logging.h"
#include "rtc_base/time_utils.h"

namespace webrtc {
namespace internal {

DummyAudioCapturer::DummyAudioCapturer(
  TaskQueueFactory* task_queue_factory,
  rtc::Thread* signaling_thread,
  int bits_per_sample,
  int sample_rate_hz,
  size_t number_of_channels,
  const std::string name
) : task_queue_(task_queue_factory->CreateTaskQueue("DummyAudioCapturer", TaskQueueFactory::Priority::HIGH))
  , signaling_thread_(signaling_thread)
  , name_(name)
  , bits_per_sample_(bits_per_sample)
  , sample_rate_hz_(sample_rate_hz)
  , number_of_channels_(number_of_channels)
  , number_of_frames_(sample_rate_hz / 100) // for 10 msec chunk
  , sinks_()
{
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor " << (void*)this;
}

DummyAudioCapturer::~DummyAudioCapturer()
{
  Stop();
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}

RTCCaptureState DummyAudioCapturer::Start()
{
  if (capture_state_ == RTCCaptureState::CS_RUNNING) {
    return capture_state_;
  }

  sample_index_ = 0;
  capture_delay_ = number_of_frames_ * 1000 / sample_rate_hz_;
  capture_state_ = RTCCaptureState::CS_RUNNING;
  task_queue_->PostTask([this] { CaptureFrame(); });
  if (observer_) {
    if (!signaling_thread_->IsCurrent()) {
      signaling_thread_->BlockingCall([&, this]() { 
        if (observer_) { observer_->OnStart(); }
      });
    }
    else {
      observer_->OnStart();
    }
  }
  return capture_state_;
}

void DummyAudioCapturer::Stop()
{
  if (!IsRunning()) {
    return;
  }

  if (observer_) {
    if (!signaling_thread_->IsCurrent()) {
      signaling_thread_->BlockingCall([&, this]() { 
        if (observer_) { observer_->OnStop(); }
      });
    } else {
      observer_->OnStop();
    }
  }
  capture_state_ = RTCCaptureState::CS_STOPPED;
}

RTCCaptureState DummyAudioCapturer::CaptureState() {
  return capture_state_;
}

bool DummyAudioCapturer::IsRunning() const {
  return capture_state_ == RTCCaptureState::CS_RUNNING;
}

void DummyAudioCapturer::AddSink(AudioTrackSinkInterface* sink) {
  if (sinks_.end() == std::find(sinks_.begin(), sinks_.end(), sink)) {
    sinks_.push_back(sink);
  }
}

void DummyAudioCapturer::RemoveSink(AudioTrackSinkInterface* sink) {
  sinks_.erase(
      std::remove_if(
          sinks_.begin(), sinks_.end(),
          [sink](AudioTrackSinkInterface* sink_) {
            return sink_ == sink;
          }),
      sinks_.end());
}

void DummyAudioCapturer::OnData(const void* audio_data,
            int bits_per_sample,
            int sample_rate,
            size_t number_of_channels,
            size_t number_of_frames)
{
  for (auto* sink_ : sinks_) {
    sink_->OnData(
      audio_data,
      bits_per_sample,
      sample_rate,
      number_of_channels,
      number_of_frames
    );
  }
}

void DummyAudioCapturer::OnFrameReady(const RTCAudioDataPtr& audio_data) {
  OnData(
    static_cast<const void*>(audio_data->data()),
    static_cast<int>(audio_data->bits_per_sample()),
    audio_data->sample_rate_hz(),
    static_cast<size_t>(audio_data->num_channels()),
    static_cast<size_t>(audio_data->samples_per_channel())
  );
}

void DummyAudioCapturer::FillBuffer()
{
  RTCAudioDataPtr audio_data = RTCAudioData::Create
  (
    sample_index_++,
    nullptr,
    static_cast<uint32_t>(bits_per_sample_),
    sample_rate_hz_,
    static_cast<uint32_t>(number_of_channels_)
  );

  datas_.push_back(audio_data);
  while (datas_.size() > 15) {
    datas_.pop_front();
  }

#if defined(DEBUG) || defined(_DEBUG)
  audio_data->Clear(libwebrtc::RTCAudioDataToneFrequency::kTone1kHz); // fill 1Khz tone signal
#endif // DEBUG
  
  if (observer_ != nullptr) {
    observer_->OnFillBuffer(audio_data);
  }
  OnFrameReady(audio_data);
}

void DummyAudioCapturer::CaptureFrame() {
  if (capture_state_ != RTCCaptureState::CS_RUNNING) {
    return;
  }
  int64_t timeFirst = rtc::TimeMillis();
  FillBuffer();
  int64_t timeLast = rtc::TimeMillis();
  int64_t waitTime = capture_delay_ - (timeLast - timeFirst);
  if (waitTime < 1) { waitTime = 1; }
  task_queue_->PostDelayedHighPrecisionTask([this]() {
    CaptureFrame();
  }, webrtc::TimeDelta::Millis(waitTime));
}

}  // namespace internal
}  // namespace webrtc
