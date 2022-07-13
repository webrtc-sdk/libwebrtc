#ifndef LIB_WEBRTC_DESKTOP_CAPTURER_IMPL_HXX
#define LIB_WEBRTC_DESKTOP_CAPTURER_IMPL_HXX

#include "modules/desktop_capture/desktop_capture_options.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "modules/desktop_capture/desktop_and_cursor_composer.h"
#include "modules/desktop_capture/win/window_capture_utils.h"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"

#include "api/scoped_refptr.h"
#include "api/video/i420_buffer.h"
#include "api/video/video_frame.h"
#include "api/video/video_source_interface.h"
#include "media/base/video_adapter.h"
#include "media/base/video_broadcaster.h"
#include "pc/video_track_source.h"
#include "rtc_base/thread.h"
#include "rtc_desktop_device.h"
#include "src/internal/vcm_capturer.h"
#include "src/internal/video_capturer.h"
#include "third_party/libyuv/include/libyuv.h"

#include "rtc_types.h"

// std::unique_ptr<cricket::VideoCapturer> video_device =
// std::unique_ptr<cricket::VideoCapturer>(new DesktopCapturerDeviceImpl());

namespace webrtc {
namespace internal {

enum CaptureState { CS_RUNNING, CS_STOPPED };

class DesktopCapturer : public webrtc::internal::VideoCapturer,
                        public rtc::MessageHandler,
                        public webrtc::DesktopCapturer::Callback {
 public:
  DesktopCapturer(std::unique_ptr<webrtc::DesktopCapturer> desktopcapturer);
  DesktopCapturer(std::unique_ptr<webrtc::DesktopCapturer> desktopcapturer, int window_id);
  ~DesktopCapturer();
  void CaptureFrame();
  virtual CaptureState Start(
      const cricket::VideoFormat& capture_format);
  virtual void Stop();
  virtual bool IsRunning();
  virtual void OnCaptureResult(
      webrtc::DesktopCapturer::Result result,
      std::unique_ptr<webrtc::DesktopFrame> frame) override;
  virtual void OnMessage(rtc::Message* msg) override;
 
 private:
  std::unique_ptr<webrtc::DesktopCapturer> capturer_;
  rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_;
  CaptureState capture_state_ = CS_STOPPED;
  std::unique_ptr<rtc::Thread> capture_thread_;
  int windows_id_= -1;
};

class ScreenCapturerTrackSource : public webrtc::VideoTrackSource {
 public:
   static rtc::scoped_refptr<ScreenCapturerTrackSource> Create(std::unique_ptr<DesktopCapturer> capturer) {
    if (capturer) {
        return new rtc::RefCountedObject<ScreenCapturerTrackSource>(std::move(capturer));
      }
    return nullptr;
  }

 public:
  explicit ScreenCapturerTrackSource(std::unique_ptr<DesktopCapturer> capturer)
      : VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {}

 private:
  rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
    return capturer_.get();
  }

  std::unique_ptr<DesktopCapturer> capturer_;
};

}  // namespace internal
}  // namespace webrtc
#endif