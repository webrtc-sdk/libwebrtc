#ifndef LIB_WEBRTC_VIDEO_RENDERER_IMPL_HXX
#define LIB_WEBRTC_VIDEO_RENDERER_IMPL_HXX

#include "rtc_video_renderer.h"
#include "rtc_video_frame.h"

namespace libwebrtc {

class RTCVideoRendererImpl : public RTCVideoRenderer<scoped_refptr<RTCVideoFrame>> {
 public:
  typedef fixed_size_function<void(void* user_data /* rtcObjectHandle */, void* frame /* rtcVideoFrameHandle */)> OnFrameCallback;

 public:
  RTCVideoRendererImpl();
  virtual ~RTCVideoRendererImpl();

  void OnFrame(scoped_refptr<RTCVideoFrame> frame) override;

  void RegisterFrameCallback(void* user_data /* rtcObjectHandle */, void* callback /* rtcVideoRendererFrameDelegate */) override;
  void RegisterFrameCallback(OnFrameCallbackSafe callback) override;
  void UnRegisterFrameCallback() override;
 
 private:
  void* user_data_ = nullptr;
  OnFrameCallback callback_ = nullptr;
  OnFrameCallbackSafe callback_safe_ = nullptr;
}; // end class RTCVideoRendererImpl

} // end namespace libwebrtc

#endif // LIB_WEBRTC_VIDEO_RENDERER_IMPL_HXX
