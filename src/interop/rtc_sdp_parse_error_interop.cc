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

    rtcResultU4 result = rtcResultU4::kSuccess;
    scoped_refptr<RTCSdpParseError> pSdpParseError = static_cast<RTCSdpParseError*>(sdpParseError);
    string sValue = pSdpParseError->line();
    if (sValue.size() > 0) {
        size_t dstLen = static_cast<size_t>(sz_value) - 1;
        size_t len = std::min(dstLen, sValue.size());
        if (sValue.size() > len) {
            result = rtcResultU4::kBufferTooSmall;
        }
        if (len > 0) {
            strncpy(value, sValue.c_string(), len);
            value[len] = '\0';
        }
    }
    return result;
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

    rtcResultU4 result = rtcResultU4::kSuccess;
    scoped_refptr<RTCSdpParseError> pSdpParseError = static_cast<RTCSdpParseError*>(sdpParseError);
    string sValue = pSdpParseError->description();
    if (sValue.size() > 0) {
        size_t dstLen = static_cast<size_t>(sz_value) - 1;
        size_t len = std::min(dstLen, sValue.size());
        if (sValue.size() > len) {
            result = rtcResultU4::kBufferTooSmall;
        }
        if (len > 0) {
            strncpy(value, sValue.c_string(), len);
            value[len] = '\0';
        }
    }
    return result;
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
