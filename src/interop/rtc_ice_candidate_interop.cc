#include "rtc_ice_candidate.h"
#include "rtc_sdp_parse_error.h"
#include "interop_api.h"

using namespace libwebrtc;

rtcResultU4 LIB_WEBRTC_CALL
RTCIceCandidate_Create(
    const char* sdp,
    const char* sdp_mid,
    int sdp_mline_index,
    rtcSdpParseErrorHandle* pOutSdpParseError,
    rtcIceCandidateHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);
    RESET_OUT_POINTER(pOutSdpParseError);
    
    scoped_refptr<RTCSdpParseError> pSdpParseError = RTCSdpParseError::Create();
    scoped_refptr<RTCIceCandidate> pIceCandidate = RTCIceCandidate::Create(
        string(sdp),
        string(sdp_mid),
        sdp_mline_index,
        pSdpParseError.get()
    );

    if (pOutSdpParseError && (pIceCandidate == nullptr)) {
        *pOutSdpParseError = static_cast<rtcSdpParseErrorHandle>(pSdpParseError.release());
    }
    *pOutRetVal = static_cast<rtcIceCandidateHandle>(pIceCandidate.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCIceCandidate_GetCandidate(
    rtcIceCandidateHandle iceCandidate,
    char* value,
    int sz_value
) noexcept
{
    CHECK_NATIVE_HANDLE(iceCandidate);
    ZERO_MEMORY(value, sz_value);
    if (sz_value < 1) {
        return rtcResultU4::kBufferTooSmall;
    }

    scoped_refptr<RTCIceCandidate> pIceCandidate = static_cast<RTCIceCandidate*>(iceCandidate);
    string strValue = pIceCandidate->candidate();
    size_t len = strValue.copy_to(value, static_cast<size_t>(sz_value));
    return strValue.size() > len
        ? rtcResultU4::kBufferTooSmall
        : rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCIceCandidate_GetSdpMid(
    rtcIceCandidateHandle iceCandidate,
    char* value,
    int sz_value
) noexcept
{
    CHECK_NATIVE_HANDLE(iceCandidate);
    ZERO_MEMORY(value, sz_value);
    if (sz_value < 1) {
        return rtcResultU4::kBufferTooSmall;
    }

    scoped_refptr<RTCIceCandidate> pIceCandidate = static_cast<RTCIceCandidate*>(iceCandidate);
    string strValue = pIceCandidate->sdp_mid();
    size_t len = strValue.copy_to(value, static_cast<size_t>(sz_value));
    return strValue.size() > len
        ? rtcResultU4::kBufferTooSmall
        : rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCIceCandidate_GetSdpMlineIndex(
    rtcIceCandidateHandle iceCandidate,
    int* pOutRetVal
) noexcept
{
    CHECK_NATIVE_HANDLE(iceCandidate);
    CHECK_POINTER(pOutRetVal);
    *pOutRetVal = -1;

    scoped_refptr<RTCIceCandidate> pIceCandidate = static_cast<RTCIceCandidate*>(iceCandidate);
    *pOutRetVal = pIceCandidate->sdp_mline_index();
    return rtcResultU4::kSuccess;
}
