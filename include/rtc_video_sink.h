#ifndef FLUTTER_WEBRTC_VIDEO_SINK_INTERFACE_HXX
#define FLUTTER_WEBRTC_VIDEO_SINK_INTERFACE_HXX

#include "rtc_types.h"
#include "rtc_video_frame.h"
#include "rtc_video_renderer.h"

namespace libwebrtc {

class RTCVideoSink : public RefCountInterface {
 public:
  ~RTCVideoSink() {}

  LIB_WEBRTC_API static scoped_refptr<RTCVideoSink> Create();

  virtual const char* id() = 0;

  virtual void AddRenderer(RTCVideoRenderer<RTCVideoFrame>* renderer) = 0;

  virtual void RemoveRenderer(RTCVideoRenderer<RTCVideoFrame>* renderer) = 0;
};

};  // namespace libwebrtc

#endif  // FLUTTER_WEBRTC_VIDEO_SINK_INTERFACE_HXX
