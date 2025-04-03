#include "rtc_video_source_impl.h"

#include "modules/video_capture/video_capture_factory.h"
#include "rtc_base/logging.h"
#include "rtc_video_frame_impl.h"

namespace libwebrtc {

class AdaptedVirtualVideoCapturer : public rtc::AdaptedVideoTrackSource {
 public:
  AdaptedVirtualVideoCapturer() {}
  ~AdaptedVirtualVideoCapturer() override {}

  bool is_screencast() const override { return false; }

  absl::optional<bool> needs_denoising() const override { return false; }

  SourceState state() const override { return kLive; }

  bool remote() const override { return false; }

  void OnFrameCaptured(scoped_refptr<RTCVideoFrame> frame) {
    VideoFrameBufferImpl* impl =
        static_cast<VideoFrameBufferImpl*>(frame.get());
    auto newFrame = webrtc::VideoFrame::Builder()
                        .set_video_frame_buffer(impl->buffer())
                        .set_rotation(impl->rtc_rotation())
                        .set_timestamp_us(impl->timestamp_us())
                        .build();
    OnFrame(newFrame);
  }
};

class VirtualVideoCapturerImpl : public VirtualVideoCapturer {
 public:
  VirtualVideoCapturerImpl() {
    adapted_source_ = new rtc::RefCountedObject<AdaptedVirtualVideoCapturer>();
    rtc_source_ = scoped_refptr<RTCVideoSourceImpl>(
        new RefCountedObject<RTCVideoSourceImpl>(adapted_source_));
  }
  virtual ~VirtualVideoCapturerImpl() {}

  virtual scoped_refptr<RTCVideoSource> source() override {
    return rtc_source_;
  }

  virtual void OnFrameCaptured(scoped_refptr<RTCVideoFrame> frame) override {
    adapted_source_->OnFrameCaptured(frame);
  }

 private:
  rtc::scoped_refptr<AdaptedVirtualVideoCapturer> adapted_source_;
  scoped_refptr<RTCVideoSourceImpl> rtc_source_;
};

scoped_refptr<VirtualVideoCapturer> VirtualVideoCapturer::Create() {
  return scoped_refptr<VirtualVideoCapturer>(
      new RefCountedObject<VirtualVideoCapturerImpl>());
}

RTCVideoSourceImpl::RTCVideoSourceImpl(
    rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> rtc_source_track)
    : rtc_source_track_(rtc_source_track) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor ";
}

RTCVideoSourceImpl::~RTCVideoSourceImpl() {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}

}  // namespace libwebrtc
