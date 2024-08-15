#ifndef LIB_WEBRTC_RTC_VIDEO_SOURCE_HXX
#define LIB_WEBRTC_RTC_VIDEO_SOURCE_HXX

#include "rtc_types.h"
#include "rtc_video_frame.h"

namespace libwebrtc {

class RTCVideoSource : public RefCountInterface {
 public:
  ~RTCVideoSource() {}
};

class VirtualVideoCapturer : public RefCountInterface {
 public:
  LIB_WEBRTC_API static scoped_refptr<VirtualVideoCapturer> Create();

  virtual void OnFrameCaptured(scoped_refptr<RTCVideoFrame> frame) = 0; 

  virtual scoped_refptr<RTCVideoSource> source() = 0;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_VIDEO_SOURCE_HXX
