#ifndef LIB_WEBRTC_DESKTOP_DEVICE_IMPL_HXX
#define LIB_WEBRTC_DESKTOP_DEVICE_IMPL_HXX

#include "modules/desktop_capture/desktop_capture_options.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_and_cursor_composer.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "modules/desktop_capture/win/screen_capture_utils.h"
#include "modules/desktop_capture/win/window_capture_utils.h"
#include "internal/desktop_capturer.h"
#include "rtc_desktop_device.h"

// std::unique_ptr<cricket::VideoCapturer> video_device =
// std::unique_ptr<cricket::VideoCapturer>(new DesktopCapturerDeviceImpl());
namespace libwebrtc {

class RTCDesktopCapturerImpl : public RTCDesktopCapturer {
 public:
  RTCDesktopCapturerImpl(std::unique_ptr<webrtc::internal::DesktopCapturer> video_capturer);
      
  std::unique_ptr<webrtc::internal::DesktopCapturer> video_capturer() {
      return std::unique_ptr<webrtc::internal::DesktopCapturer>(std::move(video_capturer_));
  }

 private:
  std::unique_ptr<webrtc::internal::DesktopCapturer> video_capturer_;
};

class RTCDesktopDeviceImpl : public RTCDesktopDevice {
 public:
  RTCDesktopDeviceImpl(rtc::Thread* signaling_thread);
  ~RTCDesktopDeviceImpl();

  virtual scoped_refptr<RTCDesktopCapturer> CreateScreenCapturer(uint64_t screen_id) override;

  virtual scoped_refptr<RTCDesktopCapturer> CreateWindowCapturer(uint64_t window_id) override;

  webrtc::DesktopCaptureOptions CreateOptions();

  virtual bool GetScreenList(SourceList& sources) override;

  virtual bool GetWindowList(SourceList& sources) override;

  virtual SourceList EnumerateWindows() override;

  virtual SourceList EnumerateScreens() override;

private:
  std::unique_ptr<rtc::Thread> _capture_thread;
  rtc::Thread* signaling_thread_ = nullptr;
};

}  // namespace libwebrtc

#endif