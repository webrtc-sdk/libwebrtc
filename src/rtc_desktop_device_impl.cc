#include "rtc_desktop_device_impl.h"
#include "rtc_video_device_impl.h"

#include "rtc_desktop_capturer.h"

namespace libwebrtc {

DesktopDeviceImpl::DesktopDeviceImpl(rtc::Thread* worker_thread)
    : worker_thread_(worker_thread) {}

webrtc::DesktopCaptureOptions DesktopDeviceImpl::CreateOptions() {
  webrtc::DesktopCaptureOptions options =
      webrtc::DesktopCaptureOptions::CreateDefault();
#ifdef _MSC_VER
  options.set_allow_directx_capturer(false);
#endif
  return options;
}

scoped_refptr<RTCVideoCapturer> DesktopDeviceImpl::CreateScreenCapturer() {
  webrtc::DesktopCaptureOptions options = CreateOptions();
  RTCDesktopCapturer* desktopCapturer = new RTCDesktopCapturer(
      worker_thread_,
      webrtc::DesktopCapturer::CreateScreenCapturer(options));
  RefCountedObject<RTCVideoCapturerImpl>* vci =
      new RefCountedObject<RTCVideoCapturerImpl>(
          std::unique_ptr<RTCDesktopCapturer>(desktopCapturer));
  scoped_refptr<RTCVideoCapturerImpl> video_capturer = scoped_refptr<RTCVideoCapturerImpl>(vci);
  cricket::VideoFormat capture_format;
  desktopCapturer->Start(capture_format);
  return video_capturer;
}

scoped_refptr<RTCVideoCapturer> DesktopDeviceImpl::CreateWindowCapturer() {
  webrtc::DesktopCaptureOptions options;
  RTCDesktopCapturer* desktopCapturer = new RTCDesktopCapturer(
      worker_thread_,
      webrtc::DesktopCapturer::CreateWindowCapturer(options));
  scoped_refptr<RTCVideoCapturerImpl> video_capturer =
      scoped_refptr<RTCVideoCapturerImpl>(
          new RefCountedObject<RTCVideoCapturerImpl>(
              std::unique_ptr<RTCDesktopCapturer>(desktopCapturer)));
  return video_capturer;
}
}  // namespace libwebrtc
