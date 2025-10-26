#include "src/rtc_dummy_video_capturer_impl.h"
#include "rtc_video_frame.h"
#include "third_party/libyuv/include/libyuv.h"

#include "interop_api.h"

namespace libwebrtc {

/**
 * DummyVideoCapturerObserverImpl
 */

DummyVideoCapturerObserverImpl::DummyVideoCapturerObserverImpl(
    void* callbacks /* rtcDummyVideoCapturerObserverCallbacks* */)
    : callbacks_(nullptr) {
  if (callbacks) {
    size_t nSize = sizeof(rtcDummyVideoCapturerObserverCallbacks);
    callbacks_ = malloc(nSize);
    memcpy(callbacks_, (const void*)callbacks, nSize);
  }
}

DummyVideoCapturerObserverImpl::~DummyVideoCapturerObserverImpl() {
  if (callbacks_) {
    free(callbacks_);
  }
  callbacks_ = nullptr;
}

void DummyVideoCapturerObserverImpl::OnStart() {
  if (callbacks_) {
    rtcDummyVideoCapturerObserverCallbacks* pCallbacks =
        reinterpret_cast<rtcDummyVideoCapturerObserverCallbacks*>(callbacks_);
    pCallbacks->Started(pCallbacks->UserData);
  }
}

void DummyVideoCapturerObserverImpl::OnPaused() {
  if (callbacks_) {
    rtcDummyVideoCapturerObserverCallbacks* pCallbacks =
        reinterpret_cast<rtcDummyVideoCapturerObserverCallbacks*>(callbacks_);
    pCallbacks->Paused(pCallbacks->UserData);
  }
}

void DummyVideoCapturerObserverImpl::OnStop() {
  if (callbacks_) {
    rtcDummyVideoCapturerObserverCallbacks* pCallbacks =
        reinterpret_cast<rtcDummyVideoCapturerObserverCallbacks*>(callbacks_);
    pCallbacks->Stopped(pCallbacks->UserData);
  }
}

void DummyVideoCapturerObserverImpl::OnError() {
  if (callbacks_) {
    rtcDummyVideoCapturerObserverCallbacks* pCallbacks =
        reinterpret_cast<rtcDummyVideoCapturerObserverCallbacks*>(callbacks_);
    pCallbacks->Failed(pCallbacks->UserData);
  }
}

void DummyVideoCapturerObserverImpl::OnFillBuffer(
    scoped_refptr<RTCVideoFrame> frame) {
  if (callbacks_) {
    rtcDummyVideoCapturerObserverCallbacks* pCallbacks =
        reinterpret_cast<rtcDummyVideoCapturerObserverCallbacks*>(callbacks_);
    rtcVideoFrameHandle hFrame =
        static_cast<rtcVideoFrameHandle>(frame.release());
    pCallbacks->FillBuffer(pCallbacks->UserData, hFrame);
  }
}

/**
 * class RTCDummyVideoCapturerImpl
 */

RTCDummyVideoCapturerImpl::RTCDummyVideoCapturerImpl(
    rtc::Thread* signaling_thread, uint32_t fps, uint32_t width,
    uint32_t height)
    : thread_(rtc::Thread::Create()),
      signaling_thread_(signaling_thread),
      fps_(fps),
      width_(width),
      height_(height),
      frame_buffer_(RTCVideoFrame::Create(width, height))
{
  thread_->Start();
}

RTCDummyVideoCapturerImpl::~RTCDummyVideoCapturerImpl() {
  observer_ = nullptr;
  thread_->Stop();
}

RTCCaptureState RTCDummyVideoCapturerImpl::Start() {
  return Start(fps_, width_, height_);
}

RTCCaptureState RTCDummyVideoCapturerImpl::Start(uint32_t fps) {
  return Start(fps, width_, height_);
}

RTCCaptureState RTCDummyVideoCapturerImpl::Start(uint32_t fps, uint32_t width,
                                                 uint32_t height) {
  if (capture_state_ == RTCCaptureState::CS_RUNNING) {
    return capture_state_;
  }

  if (fps == 0 || width == 0 || height == 0) {
    capture_state_ = RTCCaptureState::CS_FAILED;
  }

  if (fps >= 60) {
    capture_delay_ = uint32_t(1000.0 / 60.0);
  }
  else {
    capture_delay_ = uint32_t(1000.0 / fps);
  }

  capture_state_ = RTCCaptureState::CS_RUNNING;
  fps_ = fps;
  width_ = width;
  height_ = height;
  thread_->PostTask([this] { CaptureFrame(); });
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

void RTCDummyVideoCapturerImpl::Stop() {
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

bool RTCDummyVideoCapturerImpl::IsRunning() const {
  return capture_state_ == RTCCaptureState::CS_RUNNING;
}

void RTCDummyVideoCapturerImpl::OnFrameReady(const scoped_refptr<RTCVideoFrame>& frame) {
  int width = frame->width();
  int height = frame->height();

  width = width_ > 0 ? width_ : width;
  height = height_ > 0 ? height_ : height;
  if (!i420_buffer_ || !i420_buffer_.get() ||
    i420_buffer_->width() * i420_buffer_->height() != width * height) {
    i420_buffer_ = webrtc::I420Buffer::Create(width, height);
  }

  libyuv::I420Copy(
    frame->DataY(),
    frame->StrideY(),
    frame->DataU(),
    frame->StrideU(),
    frame->DataV(),
    frame->StrideV(),
    i420_buffer_->MutableDataY(),
    i420_buffer_->StrideY(),
    i420_buffer_->MutableDataU(),
    i420_buffer_->StrideU(),
    i420_buffer_->MutableDataV(),
    i420_buffer_->StrideV(),
    width,
    height
  );
  
  webrtc::VideoRotation rotation = static_cast<webrtc::VideoRotation>(frame->rotation());
  OnFrame(webrtc::VideoFrame(i420_buffer_, 0, rtc::TimeMillis(), rotation));
}

void RTCDummyVideoCapturerImpl::FillBuffer() {
  //scoped_refptr<RTCVideoFrame> frame = RTCVideoFrame::Create(width_, height_);
  if (observer_ != nullptr) {
    observer_->OnFillBuffer(frame_buffer_);
  }
  OnFrameReady(frame_buffer_);
}

void RTCDummyVideoCapturerImpl::CaptureFrame() {
  RTC_DCHECK_RUN_ON(thread_.get());
  if (capture_state_ == RTCCaptureState::CS_RUNNING) {
    FillBuffer();
    thread_->PostDelayedHighPrecisionTask(
        [this]() { CaptureFrame(); },
        webrtc::TimeDelta::Millis(capture_delay_));
  }
}

}  // namespace libwebrtc
