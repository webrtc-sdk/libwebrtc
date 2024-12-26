#include "rtc_media_stream_list.h"
#include "interop_api.h"

using namespace libwebrtc;

int LIB_WEBRTC_CALL
RTCMediaStreamList_GetCount (
    rtcMediaStreamListHandle mediaStreamList
) noexcept
{
    CHECK_POINTER_EX(mediaStreamList, 0);

    scoped_refptr<RTCMediaStreamList> pMediaStreamList = static_cast<RTCMediaStreamList*>(mediaStreamList);
    return static_cast<int>(pMediaStreamList->count());
}

rtcResultU4 LIB_WEBRTC_CALL
RTCMediaStreamList_GetItem (
    rtcMediaStreamListHandle mediaStreamList,
    int index,
    rtcMediaStreamHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);
    RESET_OUT_POINTER(pOutRetVal);
    CHECK_NATIVE_HANDLE(mediaStreamList);
    if (index < 0) {
        return rtcResultU4::kOutOfRange;
    }

    scoped_refptr<RTCMediaStreamList> pMediaStreamList = static_cast<RTCMediaStreamList*>(mediaStreamList);
    scoped_refptr<RTCMediaStream> pMediaStream = pMediaStreamList->item(static_cast<size_t>(index));
    if (pMediaStream == nullptr) {
        return rtcResultU4::kUnknownError;
    }
    *pOutRetVal = static_cast<rtcMediaStreamHandle>(pMediaStream.release());
    return rtcResultU4::kSuccess;
}
