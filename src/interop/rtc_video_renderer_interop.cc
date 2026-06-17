#include "rtc_video_renderer.h"
#include "rtc_video_frame.h"
#include "interop_api.h"

using namespace libwebrtc;

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoRenderer_Create(
    rtcVideoRendererHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);

    scoped_refptr<RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>> p =
        RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>::Create();
    if (p == nullptr) {
        return rtcResultU4::kUnknownError;
    }
    *pOutRetVal = static_cast<rtcVideoRendererHandle>(p.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoRenderer_RegisterFrameCallback(
    rtcVideoRendererHandle videoRenderer,
    rtcObjectHandle userData,
    rtcVideoRendererFrameDelegate callback
) noexcept
{
    CHECK_NATIVE_HANDLE(videoRenderer);

    scoped_refptr<RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>> p =
        static_cast<RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>*>(videoRenderer);
    p->RegisterFrameCallback(static_cast<void*>(userData), reinterpret_cast<void*>(callback));
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoRenderer_UnRegisterFrameCallback(
    rtcVideoRendererHandle videoRenderer
) noexcept
{
    CHECK_NATIVE_HANDLE(videoRenderer);

    scoped_refptr<RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>> p =
        static_cast<RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>*>(videoRenderer);
    p->UnRegisterFrameCallback();
    return rtcResultU4::kSuccess;
}

#if defined(DEBUG) || defined(_DEBUG)
rtcResultU4 LIB_WEBRTC_CALL
RTCVideoRenderer_FireOnFrame(
    rtcVideoRendererHandle videoRenderer,
    rtcVideoFrameHandle videoFrame
) noexcept
{
    CHECK_NATIVE_HANDLE(videoRenderer);
    CHECK_NATIVE_HANDLE(videoFrame);

    scoped_refptr<RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>> vr =
        static_cast<RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>*>(videoRenderer);
    scoped_refptr<RTCVideoFrame> vf = static_cast<RTCVideoFrame*>(videoFrame);
    vr->OnFrame(vf);
    return rtcResultU4::kSuccess;
}
#endif // DEBUG or _DEBUG
