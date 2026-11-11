#include "rtc_media_stream.h"
#include "interop_api.h"

using namespace libwebrtc;

rtcBool32 LIB_WEBRTC_CALL
RTCMediaStream_AddAudioTrack(
    rtcMediaStreamHandle mediaStream,
    rtcAudioTrackHandle audioTrack
) noexcept
{
    CHECK_POINTER_EX(mediaStream, rtcBool32::kFalse);
    CHECK_POINTER_EX(audioTrack, rtcBool32::kFalse);

    scoped_refptr<RTCMediaStream> pMediaStream = static_cast<RTCMediaStream*>(mediaStream);
    scoped_refptr<RTCAudioTrack> pAudioTrack = static_cast<RTCAudioTrack*>(audioTrack);
    return pMediaStream->AddTrack(pAudioTrack)
        ? rtcBool32::kTrue
        : rtcBool32::kFalse;
}

rtcBool32 LIB_WEBRTC_CALL
RTCMediaStream_AddVideoTrack(
    rtcMediaStreamHandle mediaStream,
    rtcVideoTrackHandle videoTrack
) noexcept
{
    CHECK_POINTER_EX(mediaStream, rtcBool32::kFalse);
    CHECK_POINTER_EX(videoTrack, rtcBool32::kFalse);

    scoped_refptr<RTCMediaStream> pMediaStream = static_cast<RTCMediaStream*>(mediaStream);
    scoped_refptr<RTCVideoTrack> pVideoTrack = static_cast<RTCVideoTrack*>(videoTrack);
    return pMediaStream->AddTrack(pVideoTrack)
        ? rtcBool32::kTrue
        : rtcBool32::kFalse;
}

rtcBool32 LIB_WEBRTC_CALL
RTCMediaStream_RemoveAudioTrack(
    rtcMediaStreamHandle mediaStream,
    rtcAudioTrackHandle audioTrack
) noexcept
{
    CHECK_POINTER_EX(mediaStream, rtcBool32::kFalse);
    CHECK_POINTER_EX(audioTrack, rtcBool32::kFalse);

    scoped_refptr<RTCMediaStream> pMediaStream = static_cast<RTCMediaStream*>(mediaStream);
    scoped_refptr<RTCAudioTrack> pAudioTrack = static_cast<RTCAudioTrack*>(audioTrack);
    return pMediaStream->RemoveTrack(pAudioTrack)
        ? rtcBool32::kTrue
        : rtcBool32::kFalse;
}

rtcBool32 LIB_WEBRTC_CALL
RTCMediaStream_RemoveVideoTrack(
    rtcMediaStreamHandle mediaStream,
    rtcVideoTrackHandle videoTrack
) noexcept
{
    CHECK_POINTER_EX(mediaStream, rtcBool32::kFalse);
    CHECK_POINTER_EX(videoTrack, rtcBool32::kFalse);

    scoped_refptr<RTCMediaStream> pMediaStream = static_cast<RTCMediaStream*>(mediaStream);
    scoped_refptr<RTCVideoTrack> pVideoTrack = static_cast<RTCVideoTrack*>(videoTrack);
    return pMediaStream->RemoveTrack(pVideoTrack)
        ? rtcBool32::kTrue
        : rtcBool32::kFalse;
}

int LIB_WEBRTC_CALL
RTCMediaStream_NumberOfAudioTracks(
    rtcMediaStreamHandle mediaStream
) noexcept
{
    CHECK_POINTER_EX(mediaStream, 0);
    scoped_refptr<RTCMediaStream> pMediaStream = static_cast<RTCMediaStream*>(mediaStream);
    vector<scoped_refptr<RTCAudioTrack>> audio_tracks = pMediaStream->audio_tracks();
    return static_cast<int>(audio_tracks.size());
}

int LIB_WEBRTC_CALL
RTCMediaStream_NumberOfVideoTracks(
    rtcMediaStreamHandle mediaStream
) noexcept
{
    CHECK_POINTER_EX(mediaStream, 0);
    scoped_refptr<RTCMediaStream> pMediaStream = static_cast<RTCMediaStream*>(mediaStream);
    vector<scoped_refptr<RTCVideoTrack>> video_tracks = pMediaStream->video_tracks();
    return static_cast<int>(video_tracks.size());
}

rtcResultU4 LIB_WEBRTC_CALL
RTCMediaStream_GetAudioTrack (
    rtcMediaStreamHandle mediaStream,
    int index,
    rtcAudioTrackHandle* pOutRetVal
) noexcept
{
    CHECK_NATIVE_HANDLE(mediaStream);
    CHECK_OUT_POINTER(pOutRetVal);
    if (index < 0) {
        return rtcResultU4::kOutOfRange;
    }

    scoped_refptr<RTCMediaStream> pMediaStream = static_cast<RTCMediaStream*>(mediaStream);
    vector<scoped_refptr<RTCAudioTrack>> audio_tracks = pMediaStream->audio_tracks();
    size_t ndx = static_cast<size_t>(index);
    if (ndx >= audio_tracks.size()) {
        return rtcResultU4::kOutOfRange;
    }
    scoped_refptr<RTCAudioTrack> audio_track = audio_tracks[index];
    *pOutRetVal = static_cast<rtcAudioTrackHandle>(audio_track.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCMediaStream_GetVideoTrack (
    rtcMediaStreamHandle mediaStream,
    int index,
    rtcVideoTrackHandle* pOutRetVal
) noexcept
{
    CHECK_NATIVE_HANDLE(mediaStream);
    CHECK_OUT_POINTER(pOutRetVal);
    if (index < 0) {
        return rtcResultU4::kOutOfRange;
    }

    scoped_refptr<RTCMediaStream> pMediaStream = static_cast<RTCMediaStream*>(mediaStream);
    vector<scoped_refptr<RTCVideoTrack>> video_tracks = pMediaStream->video_tracks();
    size_t ndx = static_cast<size_t>(index);
    if (ndx >= video_tracks.size()) {
        return rtcResultU4::kOutOfRange;
    }
    scoped_refptr<RTCVideoTrack> video_track = video_tracks[index];
    *pOutRetVal = static_cast<rtcVideoTrackHandle>(video_track.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCMediaStream_FindAudioTrack (
    rtcMediaStreamHandle mediaStream,
    const char* track_id,
    rtcAudioTrackHandle* pOutRetVal
) noexcept
{
    CHECK_NATIVE_HANDLE(mediaStream);
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_POINTER_EX(track_id, rtcResultU4::kInvalidParameter);

    scoped_refptr<RTCMediaStream> pMediaStream = static_cast<RTCMediaStream*>(mediaStream);
    scoped_refptr<RTCAudioTrack> audio_track = pMediaStream->FindAudioTrack(string(track_id));
    *pOutRetVal = static_cast<rtcAudioTrackHandle>(audio_track.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCMediaStream_FindVideoTrack (
    rtcMediaStreamHandle mediaStream,
    const char* track_id,
    rtcVideoTrackHandle* pOutRetVal
) noexcept
{
    CHECK_NATIVE_HANDLE(mediaStream);
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_POINTER_EX(track_id, rtcResultU4::kInvalidParameter);

    scoped_refptr<RTCMediaStream> pMediaStream = static_cast<RTCMediaStream*>(mediaStream);
    scoped_refptr<RTCVideoTrack> video_track = pMediaStream->FindVideoTrack(string(track_id));
    *pOutRetVal = static_cast<rtcVideoTrackHandle>(video_track.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCMediaStream_GetLabel (
    rtcMediaStreamHandle mediaStream,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(mediaStream, value, sz_value, RTCMediaStream, label);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCMediaStream_GetId (
    rtcMediaStreamHandle mediaStream,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(mediaStream, value, sz_value, RTCMediaStream, id);
}

int LIB_WEBRTC_CALL
RTCMediaStreamList_GetCount (
    rtcMediaStreamListHandle handle
) noexcept
{
    DECLARE_LIST_GET_COUNT(handle, RTCMediaStreamList);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCMediaStreamList_GetItem (
    rtcMediaStreamListHandle mediaStreamList,
    int index,
    rtcMediaStreamHandle* pOutRetVal
) noexcept
{
    DECLARE_LIST_GET_ITEM(mediaStreamList, index, pOutRetVal, rtcMediaStreamHandle, RTCMediaStreamList, RTCMediaStream);
}
