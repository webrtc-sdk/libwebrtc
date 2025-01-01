#include "rtc_rtp_sender.h"
#include "interop_api.h"
#include "rtc_media_track.h"
#include "rtc_dtmf_sender.h"
#include "rtc_dtls_transport.h"

#include <sstream>

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
    sz_value--;

    scoped_refptr<RTCRtpSender> p = static_cast<RTCRtpSender*>(handle);
    std::vector<string> streamIds = p->stream_ids().std_vector();
    std::string strValue;
    for (auto stream_id : streamIds) {
        strValue += stream_id.std_string() + "\n";
    }
    size_t len = std::min(strValue.size(), static_cast<size_t>(sz_value));
    if (strValue.size() > 0 && len > 0) {
        strncpy(value, strValue.c_str(), len);
        value[len] = '\0';
    }
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

    std::string input(value ? value : '\0');
    std::vector<string> streamIds;
    std::stringstream ss(input);
    std::string line;

    while (std::getline(ss, line)) {
        streamIds.push_back(line);
    }

    scoped_refptr<RTCRtpSender> p = static_cast<RTCRtpSender*>(handle);
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
