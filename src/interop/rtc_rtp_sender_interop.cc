#include "rtc_rtp_sender.h"
#include "interop_api.h"
#include "rtc_media_track.h"
#include "rtc_dtmf_sender.h"
#include "rtc_dtls_transport.h"

using namespace libwebrtc;

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetTrack (
    rtcRtpSenderHandle handle,
    rtcMediaTrackHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcMediaTrackHandle, RTCRtpSender, track().release);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_SetTrack (
    rtcRtpSenderHandle handle,
    rtcMediaTrackHandle value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, RTCMediaTrack*, RTCRtpSender, set_track);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetDtlsTransport (
    rtcRtpSenderHandle handle,
    rtcDtlsTransportHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcDtlsTransportHandle, RTCRtpSender, dtls_transport().release);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetSSRC (
    rtcRtpSenderHandle handle,
    unsigned int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, unsigned int, RTCRtpSender, ssrc);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetMediaType (
    rtcRtpSenderHandle handle,
    rtcMediaType* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcMediaType, RTCRtpSender, media_type);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetId (
    rtcRtpSenderHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpSender, id);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetStreamIds (
    rtcRtpSenderHandle handle,
    char* value,
    int sz_value
) noexcept
{
    CHECK_POINTER(value);
    if (sz_value < 1) {
        return rtcResultU4::kBufferTooSmall;
    }
    ZERO_MEMORY(value, sz_value);
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpSender> p = static_cast<RTCRtpSender*>(handle);
    string strValue = string::join("\n", p->stream_ids());
    size_t len = strValue.copy_to(value, sz_value);
    return strValue.size() > len
        ? rtcResultU4::kBufferTooSmall
        : rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_SetStreamIds (
    rtcRtpSenderHandle handle,
    const char* value
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpSender> p = static_cast<RTCRtpSender*>(handle);
    vector<string> streamIds = string(value).split("\n");
    p->set_stream_ids(streamIds);
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetInitEncodings (
    rtcRtpSenderHandle handle,
    rtcRtpEncodingParametersListHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_LIST_HANDLE(handle, pOutRetVal, rtcRtpEncodingParametersListHandle, RTCRtpSender, RTCRtpEncodingParametersList, init_send_encodings);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetParameters (
    rtcRtpSenderHandle handle,
    rtcRtpParametersHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcRtpParametersHandle, RTCRtpSender, parameters().release);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_SetParameters (
    rtcRtpSenderHandle handle,
    rtcRtpParametersHandle value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, RTCRtpParameters*, RTCRtpSender, set_parameters);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetDtmfSender (
    rtcRtpSenderHandle handle,
    rtcDtmfSenderHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcDtmfSenderHandle, RTCRtpSender, dtmf_sender().release);
}
