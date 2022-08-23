#ifndef LIB_WEBRTC_DESKTOP_DEVICE_IMPL_HXX
#define LIB_WEBRTC_DESKTOP_DEVICE_IMPL_HXX

#include "modules/desktop_capture/desktop_capture_options.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_and_cursor_composer.h"
#include "modules/desktop_capture/desktop_frame.h"
#ifdef WEBRTC_WIN
#include "modules/desktop_capture/win/window_capture_utils.h"
#endif
#include "rtc_desktop_device.h"
#include "rtc_base/thread.h"

#include "rtc_desktop_media_list_impl.h"

namespace libwebrtc {

class RTCDesktopDeviceImpl : public RTCDesktopDevice {
 public:
  RTCDesktopDeviceImpl(rtc::Thread* signaling_thread);
  ~RTCDesktopDeviceImpl();

  scoped_refptr<RTCDesktopCapturer> CreateDesktopCapturer(scoped_refptr<MediaSource> source) override;

  scoped_refptr<RTCDesktopMediaList> GetDesktopMediaList(DesktopType type) override;

private:
  rtc::Thread* signaling_thread_ = nullptr;
  std::map<DesktopType, scoped_refptr<RTCDesktopMediaListImpl>> desktop_media_lists_;
};

}  // namespace libwebrtc

#endif