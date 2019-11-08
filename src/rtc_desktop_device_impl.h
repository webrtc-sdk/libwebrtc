
#ifndef LIB_WEBRTC_DESKTOP_DEVICE_IMPL_HXX
#define LIB_WEBRTC_DESKTOP_DEVICE_IMPL_HXX
#include "rtc_desktop_device.h" 
#include "rtc_desktop_capturer_impl.h" 


#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "modules/desktop_capture/desktop_capture_options.h"


//外部调用
	//std::unique_ptr<cricket::VideoCapturer> video_device = std::unique_ptr<cricket::VideoCapturer>(new DesktopCapturerDeviceImpl());

namespace libwebrtc
{
	class DesktopDeviceImpl :public RTCDesktopDevice {
	public:
		DesktopDeviceImpl();
		scoped_refptr<RTCDesktopCapturer> CreateScreenCapturer(const DesktopCaptureOptions& options)  override;
		scoped_refptr<RTCDesktopCapturer> CreateWindowCapturer(const DesktopCaptureOptions& options)  override;
		DesktopCaptureOptions CreateOptions() override;
	};
};

#endif