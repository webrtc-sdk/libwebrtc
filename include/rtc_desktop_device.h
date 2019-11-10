#ifndef LIB_WEBRTC_RTC_DESKTOP_DEVICE_HXX
#define LIB_WEBRTC_RTC_DESKTOP_DEVICE_HXX

#include "rtc_types.h"
#include "rtc_video_device.h"
#include "modules/desktop_capture/desktop_capturer.h"

namespace libwebrtc {

using namespace webrtc;

class RTCDesktopDevice : public RefCountInterface {
 public:
  virtual scoped_refptr<RTCVideoCapturer> CreateScreenCapturer(const DesktopCaptureOptions& options) = 0;
  virtual scoped_refptr<RTCVideoCapturer> CreateWindowCapturer(const DesktopCaptureOptions& options) = 0;
  virtual DesktopCaptureOptions CreateOptions() = 0;

 protected:
  virtual ~RTCDesktopDevice() {}
};

};  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_VIDEO_DEVICE_HXX
