#include "rtc_video_renderer_impl.h"

#include "interop_api.h"
#include "rtc_base/logging.h"

namespace libwebrtc {

RTCVideoRendererImpl::RTCVideoRendererImpl()
    : user_data_(nullptr)
    , callback_(nullptr)
    , callback_safe_(nullptr)
{
    RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor " << (void*)this;
}

RTCVideoRendererImpl::~RTCVideoRendererImpl()
{
    user_data_ = nullptr;
    callback_ = nullptr;
    callback_safe_ = nullptr;
    RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}

void RTCVideoRendererImpl::OnFrame(scoped_refptr<RTCVideoFrame> frame)
{
    if (callback_) {
        void* pFrame = static_cast<void*>(frame.release());
        callback_(user_data_, pFrame);
    }
    else if (callback_safe_) {
        callback_safe_(frame);
    }
}

void RTCVideoRendererImpl::RegisterFrameCallback(void* user_data /* rtcObjectHandle */, void* callback /* rtcVideoRendererFrameDelegate */)
{
    RTC_LOG(LS_INFO) << __FUNCTION__ << ": RegisterFrameCallback " << callback;
    user_data_ = user_data;
    callback_ = reinterpret_cast<rtcVideoRendererFrameDelegate>(callback);
    callback_safe_ = nullptr;
}

void RTCVideoRendererImpl::RegisterFrameCallback(OnFrameCallbackSafe callback)
{
    RTC_LOG(LS_INFO) << __FUNCTION__ << ": RegisterFrameCallback (Safe)";
    user_data_ = nullptr;
    callback_ = nullptr;
    callback_safe_ = callback;
}

void RTCVideoRendererImpl::UnRegisterFrameCallback()
{
    RTC_LOG(LS_INFO) << __FUNCTION__;
    user_data_ = nullptr;
    callback_ = nullptr;
    callback_safe_ = nullptr;
}

template <>
scoped_refptr<RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>> RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>::Create()
{
    scoped_refptr<RTCVideoRendererImpl> video_renderer =
      scoped_refptr<RTCVideoRendererImpl>(
          new RefCountedObject<RTCVideoRendererImpl>());

    return video_renderer;
}

} // namespace libwebrtc
