#include "rtc_desktop_device_impl.h"
#include "rtc_video_device_impl.h"

#include "rtc_desktop_capturer.h"

namespace libwebrtc {

DesktopDeviceImpl::DesktopDeviceImpl() {}

webrtc::DesktopCaptureOptions DesktopDeviceImpl::CreateOptions() {
  webrtc::DesktopCaptureOptions options =
      webrtc::DesktopCaptureOptions::CreateDefault();
  options.set_allow_directx_capturer(false);
  return options;
}

scoped_refptr<RTCVideoCapturer> DesktopDeviceImpl::CreateScreenCapturer() {
  webrtc::DesktopCaptureOptions options;
  RTCDesktopCapturer* desktopCapturer = new RTCDesktopCapturer(
      webrtc::DesktopCapturer::CreateScreenCapturer(options));
  scoped_refptr<RTCVideoCapturerImpl> video_capturer =
      scoped_refptr<RTCVideoCapturerImpl>(
          new RefCountedObject<RTCVideoCapturerImpl>(
              std::unique_ptr<RTCDesktopCapturer>(desktopCapturer)));
  return video_capturer;
}

scoped_refptr<RTCVideoCapturer> DesktopDeviceImpl::CreateWindowCapturer() {
  webrtc::DesktopCaptureOptions options;
  RTCDesktopCapturer* desktopCapturer = new RTCDesktopCapturer(
      webrtc::DesktopCapturer::CreateWindowCapturer(options));
  scoped_refptr<RTCVideoCapturerImpl> video_capturer =
      scoped_refptr<RTCVideoCapturerImpl>(
          new RefCountedObject<RTCVideoCapturerImpl>(
              std::unique_ptr<RTCDesktopCapturer>(desktopCapturer)));
  return video_capturer;
}
}  // namespace libwebrtc
