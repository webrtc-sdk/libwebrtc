#include "rtc_video_source_impl.h"

#include "api/video/video_frame.h"
#include "modules/video_capture/video_capture_factory.h"
#include "rtc_base/logging.h"
#include "rtc_base/time_utils.h"
#include "rtc_video_frame_impl.h"

namespace libwebrtc {

RTCVideoSourceImpl::RTCVideoSourceImpl(
    webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> rtc_source_track)
    : rtc_source_track_(rtc_source_track) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor ";
}

RTCVideoSourceImpl::RTCVideoSourceImpl(
    webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> rtc_source_track,
    std::shared_ptr<webrtc::internal::VideoCapturer> capturer)
    : rtc_source_track_(rtc_source_track), capturer_(std::move(capturer)) {
  type_ = SourceType::kCustom;
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor (with capturer)";
}

RTCVideoSourceImpl::~RTCVideoSourceImpl() {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}

void RTCVideoSourceImpl::OnCapturedFrame(scoped_refptr<RTCVideoFrame> frame) {
  if (!frame || !capturer_) {
    return;
  }

  // The public RTCVideoFrame is always backed by VideoFrameBufferImpl, so we
  // can recover the native buffer/rotation/timestamp without copying.
  VideoFrameBufferImpl* impl = static_cast<VideoFrameBufferImpl*>(frame.get());

  int64_t timestamp_us = impl->timestamp_us();
  if (timestamp_us == 0) {
    timestamp_us = webrtc::TimeMicros();
  }

  webrtc::VideoFrame video_frame = webrtc::VideoFrame::Builder()
                                       .set_video_frame_buffer(impl->buffer())
                                       .set_rotation(impl->rtc_rotation())
                                       .set_timestamp_us(timestamp_us)
                                       .build();
  capturer_->OnFrame(video_frame);
}

}  // namespace libwebrtc
