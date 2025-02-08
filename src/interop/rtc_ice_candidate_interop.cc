/**
 * File provided for Reference Use Only by isoft (c) 2025.
 * Copyright (c) isoft. All rights reserved.
 * 
 */

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
    DECLARE_GET_STRING(iceCandidate, value, sz_value, RTCIceCandidate, candidate);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCIceCandidate_GetSdpMid(
    rtcIceCandidateHandle iceCandidate,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(iceCandidate, value, sz_value, RTCIceCandidate, sdp_mid);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCIceCandidate_GetSdpMlineIndex(
    rtcIceCandidateHandle iceCandidate,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(iceCandidate, pOutRetVal, int, RTCIceCandidate, sdp_mline_index);
}
