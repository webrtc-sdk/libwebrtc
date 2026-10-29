#ifndef LIB_WEBRTC_RTC_VIDEO_PROCESSOR_HXX
#define LIB_WEBRTC_RTC_VIDEO_PROCESSOR_HXX

#include "rtc_types.h"
#include "rtc_video_frame.h"

namespace libwebrtc {

// Modifies local video frames before they reach the encoder and the local
// renderers. Attach one with RTCVideoSource::AddProcessor.
//
// Process() runs synchronously on the capture thread for every frame, after
// the source has applied its resolution and frame rate adaptation. The input
// frame is always I420. Return it unchanged to pass it through, return a new
// frame built with RTCVideoFrame::Create() to replace it, or return nullptr to
// drop it. The timestamp and rotation of the input frame are kept either way.
//
// Do not keep a reference to the input frame after Process() returns, because
// some capturers reuse its buffer for the next frame.
class RTCVideoProcessor {
 public:
  virtual scoped_refptr<RTCVideoFrame> Process(
      scoped_refptr<RTCVideoFrame> frame) = 0;

 protected:
  virtual ~RTCVideoProcessor() {}
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_VIDEO_PROCESSOR_HXX
