#include "rtc_video_frame.h"
#include "interop_api.h"

using namespace libwebrtc;

/*
 * ---------------------------------------------------------------------- 
 * RTCVideoFrame interop methods
 * ---------------------------------------------------------------------- 
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoFrame_Create0(
    int width, int height,
    rtcVideoFrameHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);

    if (width < 2 
        || (width % 4) != 0
        || height < 2)
    {
        return rtcResultU4::kInvalidParameter;
    }

    scoped_refptr<RTCVideoFrame> pvf = RTCVideoFrame::Create(
        width, height
    );
    CHECK_POINTER_EX(pvf, rtcResultU4::kUnknownError);
    *pOutRetVal = static_cast<rtcVideoFrameHandle>(pvf.release());
    return rtcResultU4::kSuccess;
}

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
    
    int nCalcLen = (width * height * 3) / 2;
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
    int width,
    int height,
    const unsigned char* data_y,
    const unsigned char* data_u,
    const unsigned char* data_v,
    int stride_y,
    int stride_u,
    int stride_v,
    rtcVideoFrameHandle* pOutRetVal
) noexcept
{
    if (width < 2
        || (width % 4) != 0
        || height < 2
        || data_y == nullptr
        || data_u == nullptr
        || data_v == nullptr
        || stride_y < 1
        || stride_y < 1
        || stride_v < 1)
    {
        return rtcResultU4::kInvalidParameter;
    }

    scoped_refptr<RTCVideoFrame> pvf = RTCVideoFrame::Create(
        width, height,
        data_y, stride_y,
        data_u, stride_u,
        data_v, stride_v
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
RTCVideoFrame_GetWidth(
    rtcVideoFrameHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCVideoFrame, width);
}

rtcResultU4 LIB_WEBRTC_CALL 
RTCVideoFrame_GetHeight(
    rtcVideoFrameHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCVideoFrame, height);
}

rtcResultU4 LIB_WEBRTC_CALL 
RTCVideoFrame_GetSize(
    rtcVideoFrameHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCVideoFrame, size);
}

rtcResultU4 LIB_WEBRTC_CALL 
RTCVideoFrame_GetDataY(
    rtcVideoFrameHandle handle,
    const unsigned char** pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCVideoFrame> p = static_cast<RTCVideoFrame*>(handle);
    *pOutRetVal = static_cast<const unsigned char*>(p->DataY());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL 
RTCVideoFrame_GetDataU(
    rtcVideoFrameHandle handle,
    const unsigned char** pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCVideoFrame> p = static_cast<RTCVideoFrame*>(handle);
    *pOutRetVal = static_cast<const unsigned char*>(p->DataU());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL 
RTCVideoFrame_GetDataV(
    rtcVideoFrameHandle handle,
    const unsigned char** pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCVideoFrame> p = static_cast<RTCVideoFrame*>(handle);
    *pOutRetVal = static_cast<const unsigned char*>(p->DataV());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL 
RTCVideoFrame_GetStrideY(
    rtcVideoFrameHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCVideoFrame, StrideY);
}

rtcResultU4 LIB_WEBRTC_CALL 
RTCVideoFrame_GetStrideU(
    rtcVideoFrameHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCVideoFrame, StrideU);
}

rtcResultU4 LIB_WEBRTC_CALL 
RTCVideoFrame_GetStrideV(
    rtcVideoFrameHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCVideoFrame, StrideV);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoFrame_GetRotation(
    rtcVideoFrameHandle videoFrame,
    rtcVideoRotation* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(videoFrame, pOutRetVal, rtcVideoRotation, RTCVideoFrame, rotation);
}

rtcTimestamp LIB_WEBRTC_CALL
RTCVideoFrame_GetTimestampInMicroseconds(
    rtcVideoFrameHandle videoFrame
) noexcept
{
    CHECK_POINTER_EX(videoFrame, static_cast<rtcTimestamp>(0LL));

    scoped_refptr<RTCVideoFrame> pvf = static_cast<RTCVideoFrame*>(videoFrame);
    rtcTimestamp retVal = static_cast<rtcTimestamp>(pvf->timestamp_us());
    return retVal;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoFrame_SetTimestampInMicroseconds(
    rtcVideoFrameHandle videoFrame,
    rtcTimestamp timestampInMicroseconds
) noexcept
{
    DECLARE_SET_VALUE(videoFrame, timestampInMicroseconds, int64_t, RTCVideoFrame, set_timestamp_us);
}

int LIB_WEBRTC_CALL
RTCVideoFrame_ConvertToARGB(
    rtcVideoFrameHandle videoFrame,
    rtcVideoFrameARGB* dest
) noexcept
{
    CHECK_POINTER_EX(videoFrame, 0);
    CHECK_POINTER_EX(dest, 0);
    if (dest->data == nullptr
        || dest->width < 2
        || dest->height < 2
        || dest->stride < 12)
    {
        return 0;
    }

    scoped_refptr<RTCVideoFrame> pvf = static_cast<RTCVideoFrame*>(videoFrame);
    return pvf->ConvertToARGB(
        reinterpret_cast<rtcVideoFrameARGB*>(dest)
    );
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoFrame_ScaleFrom(
    rtcVideoFrameHandle dest, 
    rtcVideoFrameHandle source,
    int* pOutRetVal
) noexcept
{
    CHECK_NATIVE_HANDLE(dest);
    CHECK_NATIVE_HANDLE(source);

    scoped_refptr<RTCVideoFrame> pDst = static_cast<RTCVideoFrame*>(dest);
    scoped_refptr<RTCVideoFrame> pSrc = static_cast<RTCVideoFrame*>(source);
    int buffer_size = pDst->ScaleFrom(pSrc);
    if (pOutRetVal) {
        *pOutRetVal = buffer_size;
    }
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoFrame_ScaleFromARGB(
    rtcVideoFrameHandle dest, 
    rtcVideoFrameARGB* source,
    int* pOutRetVal
) noexcept
{
    CHECK_NATIVE_HANDLE(dest);
    CHECK_POINTER_EX(source, rtcResultU4::kInvalidParameter);
    if (source->data == nullptr ||
        source->width < 16 || 
        source->height < 16 ||
        source->stride < 64 ||
        (source->width * 4) > source->stride)
    {
        return rtcResultU4::kInvalidParameter;
    }

    switch (source->type)
    {
    case rtcVideoFrameTypeARGB::kARGB:
    case rtcVideoFrameTypeARGB::kBGRA:
    case rtcVideoFrameTypeARGB::kABGR:
    case rtcVideoFrameTypeARGB::kRGBA:
        break;
    default:
        return rtcResultU4::kInvalidParameter;
    }
    
    scoped_refptr<RTCVideoFrame> pDst = static_cast<RTCVideoFrame*>(dest);
    int buffer_size = pDst->ScaleFrom(reinterpret_cast<RTCVideoFrameARGB*>(source));
    if (pOutRetVal) {
        *pOutRetVal = buffer_size;
    }
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoFrame_ScaleFromYUV(
    rtcVideoFrameHandle dest, 
    rtcVideoFrameYUV* source,
    int* pOutRetVal
) noexcept
{
    CHECK_NATIVE_HANDLE(dest);
    CHECK_POINTER_EX(source, rtcResultU4::kInvalidParameter);
    if (source->width < 16 || 
        source->height < 16 ||
        source->dataY == nullptr ||
        source->dataU == nullptr ||
        source->dataV == nullptr ||
        source->strideY < 16 ||
        source->strideU < 8 ||
        source->strideV < 8)
    {
        return rtcResultU4::kInvalidParameter;
    }

    switch (source->type)
    {
    case rtcVideoFrameTypeYUV::kI420:
    case rtcVideoFrameTypeYUV::kYUY2:
    case rtcVideoFrameTypeYUV::kNV12:
        break;
    default:
        return rtcResultU4::kInvalidParameter;
    }
    
    scoped_refptr<RTCVideoFrame> pDst = static_cast<RTCVideoFrame*>(dest);
    int buffer_size = pDst->ScaleFrom(reinterpret_cast<RTCVideoFrameYUV*>(source));
    if (pOutRetVal) {
        *pOutRetVal = buffer_size;
    }
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoFrame_Clear(
    rtcVideoFrameHandle handle, 
    rtcVideoFrameClearType clearType,
    int* pRetVal
) noexcept
{
    if (pRetVal) {
        *pRetVal = 0;
    }
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCVideoFrame> p = static_cast<RTCVideoFrame*>(handle);
    int result = p->Clear(static_cast<RTCVideoFrameClearType>(clearType));
    if (pRetVal) {
        *pRetVal = result;
    }
    return rtcResultU4::kSuccess;
}
