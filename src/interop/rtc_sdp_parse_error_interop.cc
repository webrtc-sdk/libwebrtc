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
    CHECK_NATIVE_HANDLE(sdpParseError);
    CHECK_POINTER_EX(value, rtcResultU4::kInvalidParameter);
    if (sz_value < 1) {
        return rtcResultU4::kBufferTooSmall;
    }
    ZERO_MEMORY(value, sz_value);

    scoped_refptr<RTCSdpParseError> pSdpParseError = static_cast<RTCSdpParseError*>(sdpParseError);
    string strValue = pSdpParseError->line();
    size_t len = strValue.copy_to(value, static_cast<size_t>(sz_value));
    return strValue.size() > len
        ? rtcResultU4::kBufferTooSmall
        : rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCSdpParseError_SetLine(
    rtcSdpParseErrorHandle sdpParseError,
    const char* value
) noexcept
{
    CHECK_NATIVE_HANDLE(sdpParseError);

    scoped_refptr<RTCSdpParseError> pSdpParseError = static_cast<RTCSdpParseError*>(sdpParseError);
    pSdpParseError->set_line(string(value));
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCSdpParseError_GetDescription(
    rtcSdpParseErrorHandle sdpParseError,
    char* value,
    int sz_value
) noexcept
{
    CHECK_NATIVE_HANDLE(sdpParseError);
    CHECK_POINTER_EX(value, rtcResultU4::kInvalidParameter);
    if (sz_value < 1) {
        return rtcResultU4::kBufferTooSmall;
    }
    ZERO_MEMORY(value, sz_value);

    scoped_refptr<RTCSdpParseError> pSdpParseError = static_cast<RTCSdpParseError*>(sdpParseError);
    string strValue = pSdpParseError->description();
    size_t len = strValue.copy_to(value, static_cast<size_t>(sz_value));
    return strValue.size() > len
        ? rtcResultU4::kBufferTooSmall
        : rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCSdpParseError_SetDescription(
    rtcSdpParseErrorHandle sdpParseError,
    const char* value
) noexcept
{
    CHECK_NATIVE_HANDLE(sdpParseError);

    scoped_refptr<RTCSdpParseError> pSdpParseError = static_cast<RTCSdpParseError*>(sdpParseError);
    pSdpParseError->set_description(string(value));
    return rtcResultU4::kSuccess;
}
