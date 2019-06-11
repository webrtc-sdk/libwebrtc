#ifndef LIB_WEBRTC_VIDEO_SOURCE_IMPL_HXX
#define LIB_WEBRTC_VIDEO_SOURCE_IMPL_HXX

#include "rtc_video_frame.h"
#include "rtc_video_source.h"
#include "rtc_video_track.h"
#include "rtc_peerconnection_factory_impl.h"

#include "api/media_stream_interface.h"
#include "media/base/video_broadcaster.h"
#include "media/base/video_source_base.h"

namespace libwebrtc {

class RTCVideoSourceImpl : public RTCVideoSource {
 public:
  RTCVideoSourceImpl(
      rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> video_source_track);
  virtual ~RTCVideoSourceImpl();

  virtual rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> rtc_source_track() {
    return rtc_source_track_;
  }
 private:
  rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> rtc_source_track_;
};
}  // namespace libwebrtc

#endif //LIB_WEBRTC_VIDEO_SOURCE_IMPL_HXX
