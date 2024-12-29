#include "rtc_sdp_parse_error.h"
#include "src/rtc_sdp_parse_error_impl.h"
#include "interop_api.h"

using namespace libwebrtc;

rtcResultU4 LIB_WEBRTC_CALL
RTCSdpParseError_Create(
    rtcSdpParseErrorHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);

    scoped_refptr<RTCSdpParseError> pSdpParseError = RTCSdpParseError::Create();
    *pOutRetVal = static_cast<rtcSdpParseErrorHandle>(pSdpParseError.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCSdpParseError_GetLine(
    rtcSdpParseErrorHandle sdpParseError,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(sdpParseError, value, sz_value, RTCSdpParseError, line);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCSdpParseError_SetLine(
    rtcSdpParseErrorHandle sdpParseError,
    const char* value
) noexcept
{
    DECLARE_SET_VALUE(sdpParseError, value, string, RTCSdpParseError, set_line);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCSdpParseError_GetDescription(
    rtcSdpParseErrorHandle sdpParseError,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(sdpParseError, value, sz_value, RTCSdpParseError, description);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCSdpParseError_SetDescription(
    rtcSdpParseErrorHandle sdpParseError,
    const char* value
) noexcept
{
    DECLARE_SET_VALUE(sdpParseError, value, string, RTCSdpParseError, set_description);
}
