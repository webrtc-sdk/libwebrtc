#include "rtc_desktop_capturer_impl.h" 


namespace libwebrtc {

	RTCDesktopCapturerImpl::RTCDesktopCapturerImpl(std::unique_ptr<webrtc::DesktopCapturer> desktopcapturer) :capturer(std::move(desktopcapturer))
	{
		std::vector<cricket::VideoFormat> formats;
		formats.push_back(cricket::VideoFormat(800, 600, cricket::VideoFormat::FpsToInterval(30), cricket::FOURCC_I420));
		SetSupportedFormats(formats);
	}

	RTCDesktopCapturerImpl ::~RTCDesktopCapturerImpl() {}

	cricket::CaptureState RTCDesktopCapturerImpl::Start(const cricket::VideoFormat& capture_format)
	{
		cricket::VideoFormat supported;
		if (GetBestCaptureFormat(capture_format, &supported))
		{
			SetCaptureFormat(&supported);
		}

		SetCaptureState(cricket::CS_RUNNING);
		capturer->Start(this);
		CaptureFrame();
		return cricket::CS_RUNNING;
	}

	void RTCDesktopCapturerImpl::Stop() {
		SetCaptureState(cricket::CS_STOPPED);
		SetCaptureFormat(NULL);
	}

	bool RTCDesktopCapturerImpl::IsRunning() {
		return capture_state() == cricket::CS_RUNNING;
	}

	bool RTCDesktopCapturerImpl::GetPreferredFourccs(std::vector<uint32_t>* fourccs) {
		fourccs->push_back(cricket::FOURCC_I420);
		fourccs->push_back(cricket::FOURCC_MJPG);
		return true;
	}

	void RTCDesktopCapturerImpl::OnCaptureResult(webrtc::DesktopCapturer::Result result, std::unique_ptr<webrtc::DesktopFrame> frame) {
		if (result != webrtc::DesktopCapturer::Result::SUCCESS)
		{
			return;
		}

		int width = frame->size().width();
		int height = frame->size().height();

		if (!i420_buffer_ || !i420_buffer_.get() || i420_buffer_->width() * i420_buffer_->height() < width * height)
		{
			i420_buffer_ = webrtc::I420Buffer::Create(width, height);
		}
		libyuv::ConvertToI420(frame->data(), 0, i420_buffer_->MutableDataY(),
			i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
			i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
			i420_buffer_->StrideV(), 0, 0, width, height, width,
			height, libyuv::kRotate0, libyuv::FOURCC_ARGB);

		OnFrame(webrtc::VideoFrame(i420_buffer_, 0, 0, webrtc::kVideoRotation_0), width, height);
	}

	void RTCDesktopCapturerImpl::OnMessage(rtc::Message* msg) {
		if (msg->message_id == 0)
		{
			CaptureFrame();
		}
	}

	void RTCDesktopCapturerImpl::CaptureFrame() {
		capturer->CaptureFrame();
		rtc::Location loc(__FUNCTION__, __FILE__);
		rtc::Thread::Current()->PostDelayed(loc, 33, this, 0);
	}
}