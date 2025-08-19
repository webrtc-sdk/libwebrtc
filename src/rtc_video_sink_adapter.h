#ifndef LIB_WEBRTC_VIDEO_SINK_ADPTER_HXX
#define LIB_WEBRTC_VIDEO_SINK_ADPTER_HXX

#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include "rtc_base/synchronization/mutex.h"
#include "rtc_peerconnection.h"
#include "rtc_video_frame.h"

namespace libwebrtc {

class VideoSinkAdapter : public webrtc::VideoSinkInterface<webrtc::VideoFrame>,
                         public RefCountInterface {
 public:
  VideoSinkAdapter(webrtc::scoped_refptr<webrtc::VideoTrackInterface> track);
  ~VideoSinkAdapter() override;

  virtual void AddRenderer(
      RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>* renderer);

  virtual void RemoveRenderer(
      RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>* renderer);

  virtual void AddRenderer(
      webrtc::VideoSinkInterface<webrtc::VideoFrame>* renderer);

  virtual void RemoveRenderer(
      webrtc::VideoSinkInterface<webrtc::VideoFrame>* renderer);

 protected:
  // VideoSinkInterface implementation
  void OnFrame(const webrtc::VideoFrame& frame) override;
  webrtc::scoped_refptr<webrtc::VideoTrackInterface> rtc_track_;
  std::unique_ptr<webrtc::Mutex> crt_sec_;
  std::vector<RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>*> renderers_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_VIDEO_SINK_ADPTER_HXX
