#include "rtc_audio_track.h"
#include "interop_api.h"

using namespace libwebrtc;

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioTrack_SetVolume(
    rtcAudioTrackHandle audioTrack,
    double volume
) noexcept
{
    CHECK_NATIVE_HANDLE(audioTrack);

    if (volume < 0) volume = 0;
    if (volume > 10) volume = 10;
    scoped_refptr<RTCAudioTrack> pAudioTrack = static_cast<RTCAudioTrack*>(audioTrack);
    pAudioTrack->SetVolume(volume);
    return rtcResultU4::kSuccess;
}
