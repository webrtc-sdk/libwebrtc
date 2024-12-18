#include "rtc_video_track.h"
#include "interop_api.h"
#include "rtc_video_frame.h"
#include "rtc_video_renderer.h"

using namespace libwebrtc;

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoTrack_AddRenderer(
    rtcVideoTrackHandle videoTrack,
    rtcVideoRendererHandle videoRenderer
) noexcept
{
    CHECK_NATIVE_HANDLE(videoTrack);
    CHECK_NATIVE_HANDLE(videoRenderer);

    scoped_refptr<RTCVideoTrack> pVideoTrack = static_cast<RTCVideoTrack*>(videoTrack);
    RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>* pVideoRenderer =
        static_cast<RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>*>(videoRenderer);
    pVideoTrack->AddRenderer(pVideoRenderer);
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoTrack_RemoveRenderer(
    rtcVideoTrackHandle videoTrack,
    rtcVideoRendererHandle videoRenderer
) noexcept
{
    CHECK_NATIVE_HANDLE(videoTrack);
    CHECK_NATIVE_HANDLE(videoRenderer);

    scoped_refptr<RTCVideoTrack> pVideoTrack = static_cast<RTCVideoTrack*>(videoTrack);
    RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>* pVideoRenderer =
        static_cast<RTCVideoRenderer<scoped_refptr<RTCVideoFrame>>*>(videoRenderer);
    pVideoTrack->RemoveRenderer(pVideoRenderer);
    return rtcResultU4::kSuccess;
}
