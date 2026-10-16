#ifndef LIB_WEBRTC_VIDEO_SOURCE_IMPL_HXX
#define LIB_WEBRTC_VIDEO_SOURCE_IMPL_HXX

#include <memory>

#include "api/media_stream_interface.h"
#include "media/base/video_broadcaster.h"
#include "media/base/video_source_base.h"
#include "rtc_peerconnection_factory_impl.h"
#include "rtc_video_frame.h"
#include "rtc_video_source.h"
#include "rtc_video_track.h"
#include "src/internal/video_capturer.h"

namespace libwebrtc {

class RTCVideoSourceImpl : public RTCVideoSource {
 public:
  // For sources that wrap a VideoTrackSourceInterface only (e.g. desktop
  // sources). OnCapturedFrame is a no-op for instances created via this
  // constructor since there is no internal capturer to push into.
  RTCVideoSourceImpl(
      webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> video_source_track);

  // For sources backed by a libwebrtc internal::VideoCapturer (the regular
  // CapturerTrackSource path). OnCapturedFrame forwards user-supplied frames
  // through the capturer's broadcast pipeline.
  RTCVideoSourceImpl(
      webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> video_source_track,
      std::shared_ptr<webrtc::internal::VideoCapturer> capturer);

  virtual ~RTCVideoSourceImpl();

  virtual webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface>
  rtc_source_track() {
    return rtc_source_track_;
  }

  void OnCapturedFrame(scoped_refptr<RTCVideoFrame> frame) override;

  SourceType GetSourceType() const override {
    return type_;
  }

  void AddProcessor(RTCVideoProcessor* processor) override;

  void RemoveProcessor(RTCVideoProcessor* processor) override;

 private:
  webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> rtc_source_track_;
  // Receives frames from OnCapturedFrame. Null for desktop sources.
  std::shared_ptr<webrtc::internal::VideoCapturer> capturer_;
  // Runs the processors. Null for desktop sources.
  webrtc::internal::VideoCapturer* processing_capturer_ = nullptr;
  SourceType type_ = SourceType::kPlatformDevice;
};
}  // namespace libwebrtc

#endif  // LIB_WEBRTC_VIDEO_SOURCE_IMPL_HXX
