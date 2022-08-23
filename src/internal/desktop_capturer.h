#ifndef LIB_WEBRTC_DESKTOP_CAPTURER_IMPL_HXX
#define LIB_WEBRTC_DESKTOP_CAPTURER_IMPL_HXX

#include "modules/desktop_capture/desktop_capture_options.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "modules/desktop_capture/desktop_and_cursor_composer.h"
#ifdef WEBRTC_WIN
#include "modules/desktop_capture/win/window_capture_utils.h"
#endif
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"

#include "api/scoped_refptr.h"
#include "api/video/i420_buffer.h"
#include "api/video/video_frame.h"
#include "api/video/video_source_interface.h"
#include "media/base/video_adapter.h"
#include "media/base/video_broadcaster.h"
#include "pc/video_track_source.h"
#include "rtc_base/thread.h"
#include "rtc_desktop_device.h"
#include "src/internal/vcm_capturer.h"
#include "src/internal/video_capturer.h"
#include "third_party/libyuv/include/libyuv.h"

#include "src/rtc_desktop_capturer_impl.h"
#include "rtc_types.h"
#include "include/base/refcount.h"

namespace libwebrtc {

class ScreenCapturerTrackSource : public webrtc::VideoTrackSource {
 public:
   static rtc::scoped_refptr<ScreenCapturerTrackSource> Create(scoped_refptr<RTCDesktopCapturer> capturer) {
    if (capturer) {
        return rtc::make_ref_counted<ScreenCapturerTrackSource>(capturer);
      }
    return nullptr;
  }

 public:
  explicit ScreenCapturerTrackSource(scoped_refptr<RTCDesktopCapturer> capturer)
      : VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {}
  virtual ~ScreenCapturerTrackSource() { 
      capturer_->Stop();
  }
 private:
  rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
    return static_cast<RTCDesktopCapturerImpl*>(capturer_.get());
  }

  scoped_refptr<RTCDesktopCapturer> capturer_;
};

}  // namespace libwebrtc
#endif