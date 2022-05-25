#include "desktop_capturer.h"

namespace webrtc {
namespace internal {

    enum { kCaptureDelay = 33, kCaptureMessageId = 1000 };

DesktopCapturer::DesktopCapturer(
    std::unique_ptr<webrtc::DesktopCapturer> desktopcapturer)
    : capturer_(std::move(desktopcapturer)) {
}

DesktopCapturer::DesktopCapturer(std::unique_ptr<webrtc::DesktopCapturer> desktopcapturer, int window_id) 
    : capturer_(std::move(desktopcapturer)) {
      windows_id_ = window_id;
}

DesktopCapturer ::~DesktopCapturer() {
  RTC_LOG(INFO) << __FUNCTION__ << ": dtor ";
  Stop();
}

CaptureState DesktopCapturer::Start(const cricket::VideoFormat& capture_format) {
  if (!capture_thread_) {
    capture_thread_ = rtc::Thread::Create();  
  }
  capture_thread_->SetName("capture_thread", nullptr);
  RTC_CHECK(capture_thread_->Start()) << "Failed to start capture thread";

  capture_thread_->PostTask(RTC_FROM_HERE,
      [this] { 
        capture_state_ = CS_RUNNING;
        capturer_->Start(this);
        CaptureFrame();
      });

  return CS_RUNNING;
}

void DesktopCapturer::Stop() {
  capture_state_ = CS_STOPPED;
  if (capture_thread_) {
    capture_thread_->Stop();
  }
}

bool DesktopCapturer::IsRunning() {
  return capture_state_ == CS_RUNNING;
}

int filterException(int code, PEXCEPTION_POINTERS ex) {
  return EXCEPTION_EXECUTE_HANDLER;
}

void DesktopCapturer::OnCaptureResult(
    webrtc::DesktopCapturer::Result result,
    std::unique_ptr<webrtc::DesktopFrame> frame) {

  if (result != webrtc::DesktopCapturer::Result::SUCCESS) {
    return;
  }

  int width = frame->size().width();
  int height = frame->size().height();

  webrtc::DesktopRect rect_ = webrtc::DesktopRect::MakeWH(width, height);

  if (windows_id_ > 0) {
    webrtc::GetWindowRect(reinterpret_cast<HWND>(windows_id_), &rect_);
    // RTC_LOG(INFO) << "GetWindowRect(): " << rect_.size().width() << " " <<  rect_.size().height();
  }

  __try {
    i420_buffer_ = webrtc::I420Buffer::Create(width, height);

    libyuv::ConvertToI420(frame->data(), 0, i420_buffer_->MutableDataY(),
                        i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
                        i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
                        i420_buffer_->StrideV(), 0, 0, rect_.width(), rect_.height(), 
                        width, height, libyuv::kRotate0, libyuv::FOURCC_ARGB);
  
    OnFrame(webrtc::VideoFrame(i420_buffer_, 0, 0, webrtc::kVideoRotation_0));
  } 
  __except (filterException(GetExceptionCode(), GetExceptionInformation())) { }
}

void DesktopCapturer::OnMessage(rtc::Message* msg) {
  if (msg->message_id == kCaptureMessageId) {
    CaptureFrame();
  }
}

void DesktopCapturer::CaptureFrame() {
  if (capture_state_ == CS_RUNNING) {
    capturer_->CaptureFrame();
    rtc::Thread::Current()->PostDelayed(RTC_FROM_HERE, kCaptureDelay, this, kCaptureMessageId);
  }
}

}  // namespace internal
}  // namespace webrtc
