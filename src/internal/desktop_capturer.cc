#include "desktop_capturer.h"

namespace webrtc {
namespace internal {

    enum { kCaptureDelay = 33, kCaptureMessageId = 1000 };

DesktopCapturer::DesktopCapturer(
    std::unique_ptr<webrtc::DesktopCapturer> desktopcapturer)
    : capturer_(std::move(desktopcapturer)) {
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

void DesktopCapturer::OnCaptureResult(
    webrtc::DesktopCapturer::Result result,
    std::unique_ptr<webrtc::DesktopFrame> frame) {

  if (result != webrtc::DesktopCapturer::Result::SUCCESS) {
    return;
  }

  int width = frame->size().width();
  int height = frame->size().height();

  if (!i420_buffer_.get() || i420_buffer_->width() * i420_buffer_->height() < width * height) {
    i420_buffer_ = webrtc::I420Buffer::Create(width, height);
  }

  libyuv::ConvertToI420(frame->data(), 0, i420_buffer_->MutableDataY(),
                        i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
                        i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
                        i420_buffer_->StrideV(), 0, 0, width, height, width,
                        height, libyuv::kRotate0, libyuv::FOURCC_ARGB);
  
  OnFrame(webrtc::VideoFrame(i420_buffer_, 0, 0, webrtc::kVideoRotation_0));
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

 bool DesktopCapturer::GetSourceList(std::vector<libwebrtc::Source>& sources) {
  RTC_LOG(INFO) << __FUNCTION__ << " before: " << sources.size();
  webrtc::DesktopCapturer::SourceList screens;
  capturer_->GetSourceList(&screens);
  RTC_LOG(INFO) << __FUNCTION__ << " " << screens.capacity();
  for (const auto& screen : screens) {
    const libwebrtc::Source source {screen.title, screen.title, libwebrtc::SourceType::kEntireScreen};
	  sources.push_back(source);
    RTC_LOG(INFO) << " id:" << screen.id << " title:" << screen.title;
	}
  RTC_LOG(INFO) << __FUNCTION__ << " after: " << sources.size();
  return true;
 }

}  // namespace internal
}  // namespace webrtc
