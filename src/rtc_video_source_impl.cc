#include "rtc_video_source_impl.h"

#include "modules/video_capture/video_capture_factory.h"
#include "rtc_base/logging.h"
#include "rtc_video_frame_impl.h"

namespace libwebrtc {

RTCVideoSourceImpl::RTCVideoSourceImpl(
    webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> rtc_source_track)
    : rtc_source_track_(rtc_source_track) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor ";
}

RTCVideoSourceImpl::~RTCVideoSourceImpl() {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}

}  // namespace libwebrtc
