#ifndef LIB_WEBRTC_VIDEO_SINK_ADPTER_HXX
#define LIB_WEBRTC_VIDEO_SINK_ADPTER_HXX

#include "rtc_peerconnection.h"
#include "rtc_video_frame.h"

#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include "rtc_base/synchronization/mutex.h"

namespace libwebrtc {

class VideoSinkAdapter : public rtc::VideoSinkInterface<webrtc::VideoFrame>,
                         public RefCountInterface {
 public:
  VideoSinkAdapter(rtc::scoped_refptr<webrtc::VideoTrackInterface> track);
  ~VideoSinkAdapter() override;

  virtual void AddRenderer(
      RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>* renderer);

  virtual void RemoveRenderer(
      RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>* renderer);

  virtual void AddRenderer(
      rtc::VideoSinkInterface<webrtc::VideoFrame>* renderer);

  virtual void RemoveRenderer(
      rtc::VideoSinkInterface<webrtc::VideoFrame>* renderer);

 protected:
  // VideoSinkInterface implementation
  void OnFrame(const webrtc::VideoFrame& frame) override;
  rtc::scoped_refptr<webrtc::VideoTrackInterface> rtc_track_;
  std::unique_ptr<webrtc::Mutex> crt_sec_;
  std::vector<RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>*> renderers_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_VIDEO_SINK_ADPTER_HXX
