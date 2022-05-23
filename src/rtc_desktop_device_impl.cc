#include "rtc_desktop_device_impl.h"
#include "rtc_video_device_impl.h"
#include "rtc_base/thread.h"

namespace libwebrtc {

RTCDesktopCapturerImpl::RTCDesktopCapturerImpl(std::unique_ptr<webrtc::internal::DesktopCapturer> video_capturer)
      : video_capturer_(std::move(video_capturer)) {}

RTCDesktopDeviceImpl::RTCDesktopDeviceImpl(rtc::Thread* signaling_thread) 
    : signaling_thread_(signaling_thread)
     {}

RTCDesktopDeviceImpl::~RTCDesktopDeviceImpl() {
  RTC_LOG(INFO) << __FUNCTION__ << ": dtor ";
}

webrtc::DesktopCaptureOptions RTCDesktopDeviceImpl::CreateOptions() {
  webrtc::DesktopCaptureOptions options = webrtc::DesktopCaptureOptions::CreateDefault();
  // options.set_allow_directx_capturer(true);
  // options.set_allow_use_magnification_api(true);
#ifdef _MSC_VER
  options.set_allow_directx_capturer(false);
#endif
  return options;
}

scoped_refptr<RTCDesktopCapturer> RTCDesktopDeviceImpl::CreateScreenCapturer() {
  webrtc::DesktopCaptureOptions options = CreateOptions();

  webrtc::internal::DesktopCapturer* desktop_capturer = new webrtc::internal::DesktopCapturer(
      std::unique_ptr<webrtc::DesktopCapturer>(new webrtc::DesktopAndCursorComposer(
			  webrtc::DesktopCapturer::CreateScreenCapturer(options), options))
  );

  desktop_capturer->Start(cricket::VideoFormat());
  
  return signaling_thread_->Invoke<scoped_refptr<RTCDesktopCapturerImpl>>(
      RTC_FROM_HERE,[desktop_capturer] {
        return scoped_refptr<RTCDesktopCapturerImpl>(
          new RefCountedObject<RTCDesktopCapturerImpl>(
            absl::WrapUnique(desktop_capturer)
            // std::unique_ptr<webrtc::internal::DesktopCapturer>(desktop_capturer)
            ));
      });
}

bool RTCDesktopDeviceImpl::GetScreenList(SourceList& sources) {
  std::vector<Source> sources_;

  webrtc::DesktopCapturer::SourceList screens;
  webrtc::GetScreenList(&screens);

  for (const auto& screen : screens) {
    RTC_LOG(INFO) << " id:" << screen.id << " title:" << screen.title;
    sources_.push_back({"", screen.title, kEntireScreen}); // TODO: screen.id to portable::string
  }

  sources = sources_;
  return true;
}

bool RTCDesktopDeviceImpl::GetWindowList(SourceList& sources) {
  std::vector<Source> sources_;

  webrtc::DesktopCapturer::SourceList window_list;
  webrtc::GetWindowList(webrtc::GetWindowListFlags::kNone, &window_list);
  for (const auto& window : window_list) {
    RTC_LOG(INFO) << " id:" << window.id << " title:" << window.title;
    sources_.push_back({"", window.title, kWindow}); // TODO: window.id to portable::string
  }

  sources = sources_;
  return true;
}

scoped_refptr<RTCVideoCapturer> RTCDesktopDeviceImpl::CreateWindowCapturer() {
  webrtc::DesktopCaptureOptions options;
  webrtc::internal::DesktopCapturer* desktopCapturer = new webrtc::internal::DesktopCapturer(
      webrtc::DesktopCapturer::CreateWindowCapturer(options));
  scoped_refptr<RTCVideoCapturerImpl> video_capturer =
      scoped_refptr<RTCVideoCapturerImpl>(
          new RefCountedObject<RTCVideoCapturerImpl>(
              std::unique_ptr<webrtc::internal::DesktopCapturer>(desktopCapturer)));
  return video_capturer;
}
}  // namespace libwebrtc
