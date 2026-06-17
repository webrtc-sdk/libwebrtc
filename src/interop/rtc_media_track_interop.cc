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
    DECLARE_GET_STRING(mediaTrack, pOutKind, cchOutKind, RTCMediaTrack, kind);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCMediaTrack_GetId(
    rtcMediaTrackHandle mediaTrack,
    char* pOutId,
    int cchOutId
) noexcept
{
    DECLARE_GET_STRING(mediaTrack, pOutId, cchOutId, RTCMediaTrack, id);
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
