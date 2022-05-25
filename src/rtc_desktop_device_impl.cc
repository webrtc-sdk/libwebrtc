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
#ifdef _MSC_VER
  options.set_allow_directx_capturer(false);
#endif
  return options;
}

scoped_refptr<RTCDesktopCapturer> RTCDesktopDeviceImpl::CreateScreenCapturer(uint64_t screen_id) {
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
  sources = EnumerateScreens();
  return true;
}

SourceList RTCDesktopDeviceImpl::EnumerateScreens() {
  std::vector<Source> sources_;

  webrtc::DesktopCapturer::SourceList screen_list;
  webrtc::GetScreenList(&screen_list);
  for (const auto& screen : screen_list) {
    RTC_LOG(INFO) << " id:" << screen.id << " title:" << screen.title;
    sources_.push_back({static_cast<uint64_t>(screen.id), screen.title, kEntireScreen}); 
  }

  return sources_;
}

bool RTCDesktopDeviceImpl::GetWindowList(SourceList& sources) {
  sources = EnumerateWindows();
  return true;
}

SourceList RTCDesktopDeviceImpl::EnumerateWindows() {
  std::vector<Source> sources_;

  webrtc::DesktopCapturer::SourceList window_list;
  webrtc::GetWindowList(webrtc::GetWindowListFlags::kIgnoreUntitled | webrtc::GetWindowListFlags::kIgnoreUnresponsive, &window_list);
  for (const auto& window : window_list) {
    RTC_LOG(INFO) << " id:" << window.id << " title:" << window.title;
    sources_.push_back({static_cast<uint64_t>(window.id), window.title, kWindow}); 
  }

  return sources_;
}

scoped_refptr<RTCDesktopCapturer> RTCDesktopDeviceImpl::CreateWindowCapturer(uint64_t window_id) {
  webrtc::DesktopCaptureOptions options = CreateOptions();

  std::unique_ptr<webrtc::DesktopCapturer> capturer = webrtc::DesktopCapturer::CreateWindowCapturer(options);
  
  RTC_CHECK(capturer->SelectSource(window_id)) << "Failed to select windows";
  // RTC_CHECK(capturer->FocusOnSelectedSource()) << "Failed to focus on selected windows";

  webrtc::internal::DesktopCapturer* desktop_capturer = 
    new webrtc::internal::DesktopCapturer(std::move(capturer), window_id);
  
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
}  // namespace libwebrtc
