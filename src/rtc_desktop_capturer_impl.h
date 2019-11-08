
#ifndef LIB_WEBRTC_DESKTOP_CAPTURER_IMPL_HXX
#define LIB_WEBRTC_DESKTOP_CAPTURER_IMPL_HXX
#include "rtc_base/refcount.h"

#include "rtc_desktop_device.h" 
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "modules/desktop_capture/desktop_capture_options.h"

#include "third_party/libyuv/include/libyuv.h" 
#include "api/video/i420_buffer.h"
#include "media/base/videocapturer.h"
#include "media/base/videocommon.h"
#include "api/scoped_refptr.h"
#include "rtc_base/messagehandler.h"
#include "rtc_base/thread.h"

//外部调用
	//std::unique_ptr<cricket::VideoCapturer> video_device = std::unique_ptr<cricket::VideoCapturer>(new DesktopCapturerDeviceImpl());

namespace libwebrtc
{
	class RTCDesktopCapturerImpl : public cricket::VideoCapturer,
		public rtc::MessageHandler,
		public webrtc::DesktopCapturer::Callback,
		public RTCDesktopCapturer
	{
	public:
		RTCDesktopCapturerImpl(std::unique_ptr<webrtc::DesktopCapturer> desktopcapturer);
		~RTCDesktopCapturerImpl();

		void CaptureFrame();
		virtual cricket::CaptureState Start(const cricket::VideoFormat& capture_format);
		virtual void Stop();
		virtual bool IsRunning();
		virtual bool IsScreencast() const { return true; }
		virtual void OnCaptureResult(webrtc::DesktopCapturer::Result result, std::unique_ptr<webrtc::DesktopFrame> frame);
		virtual void OnMessage(rtc::Message* msg);

	protected:
		virtual bool GetPreferredFourccs(std::vector<uint32_t>* fourccs); 
	private:
		std::unique_ptr<webrtc::DesktopCapturer> capturer;
		rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_;
	};
};
#endif