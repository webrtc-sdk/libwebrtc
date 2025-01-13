#ifndef LIB_WEBRTC_RTC_VIDEO_RENDERER_HXX
#define LIB_WEBRTC_RTC_VIDEO_RENDERER_HXX

#include "rtc_types.h"

namespace libwebrtc {

template <typename VideoFrameT>
class RTCVideoRenderer : public RefCountInterface {
 public:
  typedef fixed_size_function<void(const VideoFrameT& frame)> OnFrameCallbackSafe;

 public:
  virtual ~RTCVideoRenderer() {}

  virtual void OnFrame(VideoFrameT frame) = 0;

  virtual void RegisterFrameCallback(void* user_data /* rtcObjectHandle */, void* callback /* rtcVideoRendererFrameDelegate */) = 0;
  virtual void RegisterFrameCallback(OnFrameCallbackSafe callback) = 0;
  virtual void UnRegisterFrameCallback() = 0;

 public:
  LIB_WEBRTC_API static scoped_refptr<RTCVideoRenderer<VideoFrameT>> Create();
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_VIDEO_RENDERER_HXX
