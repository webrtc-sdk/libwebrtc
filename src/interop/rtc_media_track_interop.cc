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
    ZERO_MEMORY(pOutKind, cchOutKind);
    CHECK_POINTER(pOutKind);
    if (cchOutKind < 1) {
        return rtcResultU4::kBufferTooSmall;
    }

    scoped_refptr<RTCMediaTrack> pMediaTrack = static_cast<RTCMediaTrack*>(mediaTrack);
    string strValue = pMediaTrack->kind();
    size_t len = strValue.copy_to(pOutKind, static_cast<size_t>(cchOutKind));
    return strValue.size() > len
        ? rtcResultU4::kBufferTooSmall
        : rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCMediaTrack_GetId(
    rtcMediaTrackHandle mediaTrack,
    char* pOutId,
    int cchOutId
) noexcept
{
    CHECK_NATIVE_HANDLE(mediaTrack);
    ZERO_MEMORY(pOutId, cchOutId);
    CHECK_POINTER(pOutId);
    if (cchOutId < 1) {
        return rtcResultU4::kBufferTooSmall;
    }

    scoped_refptr<RTCMediaTrack> pMediaTrack = static_cast<RTCMediaTrack*>(mediaTrack);
    string strValue = pMediaTrack->id();
    size_t len = strValue.copy_to(pOutId, static_cast<size_t>(cchOutId));
    return strValue.size() > len
        ? rtcResultU4::kBufferTooSmall
        : rtcResultU4::kSuccess;
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
