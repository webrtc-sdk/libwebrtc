#include "rtc_desktop_capturer.h"

namespace libwebrtc {

    enum { kCaptureDelay = 33 };

RTCDesktopCapturer::RTCDesktopCapturer(
    std::unique_ptr<webrtc::DesktopCapturer> desktopcapturer)
    : capturer(std::move(desktopcapturer)) {
}

RTCDesktopCapturer ::~RTCDesktopCapturer() {}

CaptureState RTCDesktopCapturer::Start(
    const cricket::VideoFormat& capture_format) {

  capture_state_ = CS_RUNNING;
  capturer->Start(this);
  CaptureFrame();
  return CS_RUNNING;
}

void RTCDesktopCapturer::Stop() {
  capture_state_ = CS_STOPPED;
}

bool RTCDesktopCapturer::IsRunning() {
  return capture_state_ == CS_RUNNING;
}

bool RTCDesktopCapturer::GetPreferredFourccs(std::vector<uint32_t>* fourccs) {
  fourccs->push_back(cricket::FOURCC_I420);
  fourccs->push_back(cricket::FOURCC_MJPG);
  return true;
}

void RTCDesktopCapturer::OnCaptureResult(
    webrtc::DesktopCapturer::Result result,
    std::unique_ptr<webrtc::DesktopFrame> frame) {
  if (result != webrtc::DesktopCapturer::Result::SUCCESS) {
    return;
  }

  int width = frame->size().width();
  int height = frame->size().height();

  if (!i420_buffer_ || !i420_buffer_.get() ||
      i420_buffer_->width() * i420_buffer_->height() < width * height) {
    i420_buffer_ = webrtc::I420Buffer::Create(width, height);
  }
  libyuv::ConvertToI420(frame->data(), 0, i420_buffer_->MutableDataY(),
                        i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
                        i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
                        i420_buffer_->StrideV(), 0, 0, width, height, width,
                        height, libyuv::kRotate0, libyuv::FOURCC_ARGB);

  OnFrame(webrtc::VideoFrame(i420_buffer_, 0, 0, webrtc::kVideoRotation_0));
}

void RTCDesktopCapturer::OnMessage(rtc::Message* msg) {
  if (msg->message_id == 0) {
    CaptureFrame();
  }
}

void RTCDesktopCapturer::CaptureFrame() {
  capturer->CaptureFrame();
  rtc::Thread::Current()->PostDelayed(RTC_FROM_HERE, kCaptureDelay, this, 0);
}
}  // namespace libwebrtc
