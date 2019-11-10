#include "rtc_desktop_device_impl.h" 
#include "rtc_video_device_impl.h"

namespace libwebrtc {

	DesktopDeviceImpl::DesktopDeviceImpl()
	{
	}

	DesktopCaptureOptions DesktopDeviceImpl::CreateOptions()
	{
		webrtc::DesktopCaptureOptions options = webrtc::DesktopCaptureOptions::CreateDefault();
		options.set_allow_directx_capturer(false);
		return options;
	} 
    /*×ÀÃæÍ¼Ïñ*/
	scoped_refptr<RTCVideoCapturer> DesktopDeviceImpl::CreateScreenCapturer(const webrtc::DesktopCaptureOptions& options)
	{
		RTCDesktopCapturer* desktopCapturer = new RTCDesktopCapturer(webrtc::DesktopCapturer::CreateScreenCapturer(options)); 
		scoped_refptr<RTCVideoCapturerImpl> video_capturer = scoped_refptr<RTCVideoCapturerImpl>(new RefCountedObject<RTCVideoCapturerImpl>(
			std::unique_ptr<cricket::VideoCapturer>(desktopCapturer)));
		return video_capturer;
	}
	/*Ó¦ÓÃÍ¼Ïñ*/
	scoped_refptr<RTCVideoCapturer> DesktopDeviceImpl::CreateWindowCapturer(const webrtc::DesktopCaptureOptions& options)
	{
		RTCDesktopCapturer* desktopCapturer = new RTCDesktopCapturer(webrtc::DesktopCapturer::CreateWindowCapturer(options)); 
		scoped_refptr<RTCVideoCapturerImpl> video_capturer = scoped_refptr<RTCVideoCapturerImpl>(new RefCountedObject<RTCVideoCapturerImpl>(
			std::unique_ptr<cricket::VideoCapturer>(desktopCapturer)));
		return video_capturer;

	}
}