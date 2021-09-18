#ifndef LIB_WEBRTC_DESKTOP_DEVICE_IMPL_HXX
#define LIB_WEBRTC_DESKTOP_DEVICE_IMPL_HXX

#include "modules/desktop_capture/desktop_capture_options.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "rtc_desktop_capturer.h"
#include "rtc_desktop_device.h"

// std::unique_ptr<cricket::VideoCapturer> video_device =
// std::unique_ptr<cricket::VideoCapturer>(new DesktopCapturerDeviceImpl());
namespace libwebrtc {
class DesktopDeviceImpl : public RTCDesktopDevice {
 public:
  DesktopDeviceImpl();

  scoped_refptr<RTCVideoCapturer> CreateScreenCapturer() override;

  scoped_refptr<RTCVideoCapturer> CreateWindowCapturer() override;

  webrtc::DesktopCaptureOptions CreateOptions();
};

}  // namespace libwebrtc

#endif