#include "rtc_rtp_transceiver.h"
#include "interop_api.h"

#include <sstream>

using namespace libwebrtc;

/*
 * ----------------------------------------------------------------------
 * RTCRtpTransceiverInit interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiverInit_Create (
    rtcRtpTransceiverDirection direction,
    const char* stream_ids,
    /* [optional, in] */ rtcRtpEncodingParametersListHandle encodings,
    rtcRtpTransceiverInitHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);

    vector<string> streamIds = string(stream_ids).split("\n");
    vector<scoped_refptr<RTCRtpEncodingParameters>> vecEncoding;
    scoped_refptr<RTCRtpEncodingParametersList> pList = static_cast<RTCRtpEncodingParametersList*>(encodings);
    if (pList) {
        vecEncoding = pList->to_vector();
    }
    scoped_refptr<RTCRtpTransceiverInit> p = RTCRtpTransceiverInit::Create(
        static_cast<RTCRtpTransceiverDirection>(direction),
        streamIds,
        vecEncoding
    );
    *pOutRetVal = static_cast<rtcRtpTransceiverInitHandle>(p.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiverInit_GetDirection (
    rtcRtpTransceiverInitHandle handle,
    rtcRtpTransceiverDirection* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcRtpTransceiverDirection, RTCRtpTransceiverInit, direction);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiverInit_SetDirection (
    rtcRtpTransceiverInitHandle handle,
    rtcRtpTransceiverDirection value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, RTCRtpTransceiverDirection, RTCRtpTransceiverInit, set_direction);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiverInit_GetStreamIds (
    rtcRtpTransceiverInitHandle handle,
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

    scoped_refptr<RTCRtpTransceiverInit> p = static_cast<RTCRtpTransceiverInit*>(handle);
    string strValue = string::join("\n", p->stream_ids());
    size_t len = strValue.copy_to(value, sz_value);
    return strValue.size() > len
        ? rtcResultU4::kBufferTooSmall
        : rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiverInit_SetStreamIds (
    rtcRtpTransceiverInitHandle handle,
    const char* value
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpTransceiverInit> p = static_cast<RTCRtpTransceiverInit*>(handle);
    vector<string> streamIds = string(value).split("\n");
    p->set_stream_ids(streamIds);
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiverInit_GetEncodings (
    rtcRtpTransceiverInitHandle handle,
    rtcRtpEncodingParametersListHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_LIST_HANDLE(handle, pOutRetVal, rtcRtpEncodingParametersListHandle, RTCRtpTransceiverInit, RTCRtpEncodingParametersList, send_encodings);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiverInit_SetEncodings (
    rtcRtpTransceiverInitHandle handle,
    rtcRtpEncodingParametersListHandle value
) noexcept
{
    DECLARE_SET_LIST_HANDLE(handle, value, RTCRtpTransceiverInit, RTCRtpEncodingParametersList, set_send_encodings);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtpTransceiver interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetMediaType (
    rtcRtpTransceiverHandle handle,
    rtcMediaType* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcMediaType, RTCRtpTransceiver, media_type);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetMid (
    rtcRtpTransceiverHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpTransceiver, mid);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetSender (
    rtcRtpTransceiverHandle handle,
    rtcRtpSenderHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcRtpSenderHandle, RTCRtpTransceiver, sender().release);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetReceiver (
    rtcRtpTransceiverHandle handle,
    rtcRtpReceiverHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcRtpReceiverHandle, RTCRtpTransceiver, receiver().release);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_Stopped (
    rtcRtpTransceiverHandle handle,
    rtcBool32* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcBool32, RTCRtpTransceiver, Stopped);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_Stopping (
    rtcRtpTransceiverHandle handle,
    rtcBool32* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcBool32, RTCRtpTransceiver, Stopping);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetDirection (
    rtcRtpTransceiverHandle handle,
    rtcRtpTransceiverDirection* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcRtpTransceiverDirection, RTCRtpTransceiver, direction);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_SetDirectionWithError (
    rtcRtpTransceiverHandle handle,
    rtcRtpTransceiverDirection value,
    char* retVal,
    int sz_retVal
) noexcept
{
    CHECK_POINTER(retVal);
    if (sz_retVal < 1) {
        return rtcResultU4::kBufferTooSmall;
    }
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpTransceiver> p = static_cast<RTCRtpTransceiver*>(handle);
    string strValue = p->SetDirectionWithError(static_cast<RTCRtpTransceiverDirection>(value));
    size_t len = strValue.copy_to(retVal, sz_retVal);
    return strValue.size() > len
        ? rtcResultU4::kBufferTooSmall
        : rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetCurrentDirection (
    rtcRtpTransceiverHandle handle,
    rtcRtpTransceiverDirection* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcRtpTransceiverDirection, RTCRtpTransceiver, current_direction);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetFiredDirection (
    rtcRtpTransceiverHandle handle,
    rtcRtpTransceiverDirection* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcRtpTransceiverDirection, RTCRtpTransceiver, fired_direction);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_StopStandard (
    rtcRtpTransceiverHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpTransceiver, StopStandard);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_SetCodecPreferences (
    rtcRtpTransceiverHandle handle,
    rtcRtpCodecCapabilityListHandle value
) noexcept
{
    DECLARE_SET_LIST_HANDLE(handle, value, RTCRtpTransceiver, RTCRtpCodecCapabilityList, SetCodecPreferences);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetTransceiverId (
    rtcRtpTransceiverHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpTransceiver, transceiver_id);
}
