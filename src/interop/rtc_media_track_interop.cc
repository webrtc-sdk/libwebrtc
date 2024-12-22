#include "rtc_media_track.h"
#include "interop_api.h"

using namespace libwebrtc;

rtcTrackState LIB_WEBRTC_CALL
RTCMediaTrack_GetState(
    rtcMediaTrackHandle mediaTrack
) noexcept
{
    CHECK_POINTER_EX(mediaTrack, rtcTrackState::kUnknown);
    scoped_refptr<RTCMediaTrack> pMediaTrack = static_cast<RTCMediaTrack*>(mediaTrack);
    return static_cast<rtcTrackState>(pMediaTrack->state());
}

rtcResultU4 LIB_WEBRTC_CALL
RTCMediaTrack_GetKind(
    rtcMediaTrackHandle mediaTrack,
    char* pOutKind,
    int cchOutKind
) noexcept
{
    CHECK_NATIVE_HANDLE(mediaTrack);
    if (cchOutKind < 1) {
        return rtcResultU4::kBufferTooSmall;
    }
    ZERO_MEMORY(pOutKind, cchOutKind);
    cchOutKind--;

    rtcResultU4 result = rtcResultU4::kSuccess;
    scoped_refptr<RTCMediaTrack> pMediaTrack = static_cast<RTCMediaTrack*>(mediaTrack);
    string kind = pMediaTrack->kind();
    size_t dstSize = static_cast<size_t>(cchOutKind);
    size_t srcSize = kind.size();
    if (srcSize > dstSize) {
        result = rtcResultU4::kBufferTooSmall;
    }

    size_t len = std::min(dstSize, srcSize);
    if (len > 0) {
        strncpy(pOutKind, kind.c_string(), len);
        pOutKind[len] = '\0';
    }

    return result;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCMediaTrack_GetId(
    rtcMediaTrackHandle mediaTrack,
    char* pOutId,
    int cchOutId
) noexcept
{
    CHECK_NATIVE_HANDLE(mediaTrack);
    if (cchOutId < 1) {
        return rtcResultU4::kBufferTooSmall;
    }
    ZERO_MEMORY(pOutId, cchOutId);
    cchOutId--;

    rtcResultU4 result = rtcResultU4::kSuccess;
    scoped_refptr<RTCMediaTrack> pMediaTrack = static_cast<RTCMediaTrack*>(mediaTrack);
    string id = pMediaTrack->id();
    size_t dstSize = static_cast<size_t>(cchOutId);
    size_t srcSize = id.size();
    if (srcSize > dstSize) {
        result = rtcResultU4::kBufferTooSmall;
    }

    size_t len = std::min(dstSize, srcSize);
    if (len > 0) {
        strncpy(pOutId, id.c_string(), len);
    }

    return result;
}

rtcBool32 LIB_WEBRTC_CALL
RTCMediaTrack_GetEnabled(
    rtcMediaTrackHandle mediaTrack
) noexcept
{
    CHECK_POINTER_EX(mediaTrack, rtcBool32::kFalse);
    scoped_refptr<RTCMediaTrack> pMediaTrack = static_cast<RTCMediaTrack*>(mediaTrack);
    return pMediaTrack->enabled()
        ? rtcBool32::kTrue
        : rtcBool32::kFalse;
}

rtcBool32 LIB_WEBRTC_CALL
RTCMediaTrack_SetEnabled(
    rtcMediaTrackHandle mediaTrack,
    rtcBool32 enabled
) noexcept
{
    CHECK_POINTER_EX(mediaTrack, rtcBool32::kFalse);
    scoped_refptr<RTCMediaTrack> pMediaTrack = static_cast<RTCMediaTrack*>(mediaTrack);
    return pMediaTrack->set_enabled(enabled != rtcBool32::kFalse)
        ? rtcBool32::kTrue
        : rtcBool32::kFalse;
}
