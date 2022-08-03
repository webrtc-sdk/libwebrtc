#include "rtc_video_track_impl.h"
#include "rtc_media_stream_impl.h"

#include "rtc_base/logging.h"

namespace libwebrtc {

VideoTrackImpl::VideoTrackImpl(
    rtc::scoped_refptr<webrtc::VideoTrackInterface> rtc_track)
    : rtc_track_(rtc_track),
      video_sink_(new RefCountedObject<VideoSinkAdapter>(rtc_track)) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor ";
  id_ = rtc_track_->id();
  kind_ = rtc_track_->kind();
}

VideoTrackImpl::~VideoTrackImpl() {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}

void VideoTrackImpl::AddRenderer(
    RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>* renderer) {
  return video_sink_->AddRenderer(renderer);
}

void VideoTrackImpl::RemoveRenderer(
    RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>* renderer) {
  return video_sink_->RemoveRenderer(renderer);
}

}  // namespace libwebrtc
