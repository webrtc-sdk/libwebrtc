#include "rtc_video_sink_adapter.h"
#include "rtc_video_frame_impl.h"
#include "rtc_video_track.h"

#include "rtc_base/logging.h"

namespace libwebrtc {

VideoSinkAdapter::VideoSinkAdapter(
    rtc::scoped_refptr<webrtc::VideoTrackInterface> track)
    : rtc_track_(track), crt_sec_(new webrtc::Mutex()) {
  rtc_track_->AddOrUpdateSink(this, rtc::VideoSinkWants());
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor " << (void*)this;
}

VideoSinkAdapter::~VideoSinkAdapter() {
  rtc_track_->RemoveSink(this);
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}

// VideoSinkInterface implementation
void VideoSinkAdapter::OnFrame(const webrtc::VideoFrame& video_frame) {
  scoped_refptr<VideoFrameBufferImpl> frame_buffer =
      scoped_refptr<VideoFrameBufferImpl>(
          new RefCountedObject<VideoFrameBufferImpl>(
              video_frame.video_frame_buffer()));

  frame_buffer->set_rotation(video_frame.rotation());
  frame_buffer->set_timestamp_us(video_frame.timestamp_us());

  webrtc::MutexLock cs(crt_sec_.get());
  for (auto renderer : renderers_) {
    renderer->OnFrame(frame_buffer);
  }
}

void VideoSinkAdapter::AddRenderer(
    RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>* renderer) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": AddRenderer " << (void*)renderer;
  webrtc::MutexLock cs(crt_sec_.get());
  renderers_.push_back(renderer);
}

void VideoSinkAdapter::RemoveRenderer(
    RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>* renderer) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": RemoveRenderer " << (void*)renderer;
  webrtc::MutexLock cs(crt_sec_.get());
  renderers_.erase(
      std::remove_if(
          renderers_.begin(), renderers_.end(),
          [renderer](
              const RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>* renderer_) {
            return renderer_ == renderer;
          }),
      renderers_.end());
}

void VideoSinkAdapter::AddRenderer(
    rtc::VideoSinkInterface<webrtc::VideoFrame>* renderer) {
  rtc_track_->AddOrUpdateSink(renderer, rtc::VideoSinkWants());
}
void VideoSinkAdapter::RemoveRenderer(
    rtc::VideoSinkInterface<webrtc::VideoFrame>* renderer) {
  rtc_track_->RemoveSink(renderer);
}

}  // namespace libwebrtc
