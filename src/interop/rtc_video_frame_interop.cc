#include "rtc_video_frame.h"
#include "interop_api.h"

using namespace libwebrtc;

/*
 * ---------------------------------------------------------------------- 
 * RTCVideoFrame interop methods
 * ---------------------------------------------------------------------- 
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoFrame_Create1(
    int width, int height,
    const unsigned char* buffer, int length,
    rtcVideoFrameHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);

    if (width < 2 
        || (width % 4) != 0
        || height < 2
        || buffer == nullptr
        || length < 6)
    {
        return rtcResultU4::kInvalidParameter;
    }

    int nCalcLen = width * height + (((width >> 1) * (height >> 1)) << 1);
    if (nCalcLen != length) {
        return rtcResultU4::kOutOfRange;
    }

    scoped_refptr<RTCVideoFrame> pvf = RTCVideoFrame::Create(
        width, height,
        buffer, length
    );
    CHECK_POINTER_EX(pvf, rtcResultU4::kUnknownError);
    *pOutRetVal = static_cast<rtcVideoFrameHandle>(pvf.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoFrame_Create2(
    const rtcVideoFrameDatas* frameDatas,
    rtcVideoFrameHandle* pOutRetVal
) noexcept
{
    if (frameDatas == nullptr
        || frameDatas->width < 2
        || (frameDatas->width % 4) != 0
        || frameDatas->height < 2
        || frameDatas->data_y == nullptr
        || frameDatas->data_u == nullptr
        || frameDatas->data_v == nullptr
        || frameDatas->stride_y < 1
        || frameDatas->stride_y < 1
        || frameDatas->stride_v < 1)
    {
        return rtcResultU4::kInvalidParameter;
    }

    scoped_refptr<RTCVideoFrame> pvf = RTCVideoFrame::Create(
        frameDatas->width, frameDatas->height,
        frameDatas->data_y, frameDatas->stride_y,
        frameDatas->data_u, frameDatas->stride_u,
        frameDatas->data_v, frameDatas->stride_v
    );
    CHECK_POINTER_EX(pvf, rtcResultU4::kUnknownError);
    *pOutRetVal = static_cast<rtcVideoFrameHandle>(pvf.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoFrame_Copy(
    rtcVideoFrameHandle videoFrame,
    rtcVideoFrameHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_NATIVE_HANDLE(videoFrame);

    scoped_refptr<RTCVideoFrame> pvf = static_cast<RTCVideoFrame*>(videoFrame);
    scoped_refptr<RTCVideoFrame> pvfCopy = pvf->Copy();
    CHECK_POINTER_EX(pvfCopy, rtcResultU4::kUnknownError);
    *pOutRetVal = static_cast<rtcVideoFrameHandle>(pvfCopy.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoFrame_GetFrameDatas(
    rtcVideoFrameHandle videoFrame,
    rtcVideoFrameDatas* refFrameDatas
) noexcept
{
    CHECK_POINTER(refFrameDatas);
    ZERO_MEMORY(refFrameDatas, sizeof(rtcVideoFrameDatas));

    scoped_refptr<RTCVideoFrame> pvf = static_cast<RTCVideoFrame*>(videoFrame);
    
    refFrameDatas->width = pvf->width();
    refFrameDatas->height = pvf->height();
    refFrameDatas->data_y = static_cast<const unsigned char*>(pvf->DataY());
    refFrameDatas->data_u = static_cast<const unsigned char*>(pvf->DataU());
    refFrameDatas->data_v = static_cast<const unsigned char*>(pvf->DataV());
    refFrameDatas->stride_y = pvf->StrideY();
    refFrameDatas->stride_u = pvf->StrideU();
    refFrameDatas->stride_v = pvf->StrideV();

    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoFrame_GetRotation(
    rtcVideoFrameHandle videoFrame,
    rtcVideoRotation* pOutRetVal
) noexcept
{
    CHECK_POINTER(pOutRetVal);
    *pOutRetVal = rtcVideoRotation::kVideoRotation_0;
    CHECK_NATIVE_HANDLE(videoFrame);

    scoped_refptr<RTCVideoFrame> pvf = static_cast<RTCVideoFrame*>(videoFrame);
    *pOutRetVal = static_cast<rtcVideoRotation>(pvf->rotation());
    return rtcResultU4::kSuccess;
}
