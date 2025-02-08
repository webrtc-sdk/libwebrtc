/**
 * File provided for Reference Use Only by isoft (c) 2025.
 * Copyright (c) isoft. All rights reserved.
 * 
 */

#include "rtc_rtp_capabilities.h"
#include "interop_api.h"

using namespace libwebrtc;

/*
 * ----------------------------------------------------------------------
 * RTCRtpCodecCapability interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_Create(
    rtcRtpCodecCapabilityHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);

    scoped_refptr<RTCRtpCodecCapability> pCodec = RTCRtpCodecCapability::Create();
    if (pCodec == nullptr) {
        return rtcResultU4::kUnknownError;
    }
    *pOutRetVal = static_cast<rtcRtpCodecCapabilityHandle>(pCodec.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_GetMimeType(
    rtcRtpCodecCapabilityHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpCodecCapability, mime_type);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_SetMimeType(
    rtcRtpCodecCapabilityHandle handle,
    const char* value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, string, RTCRtpCodecCapability, set_mime_type);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_GetClockRate(
    rtcRtpCodecCapabilityHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCRtpCodecCapability, clock_rate);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_SetClockRate(
    rtcRtpCodecCapabilityHandle handle,
    int value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, int, RTCRtpCodecCapability, set_clock_rate);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_GetChannels(
    rtcRtpCodecCapabilityHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCRtpCodecCapability, channels);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_SetChannels(
    rtcRtpCodecCapabilityHandle handle,
    int value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, int, RTCRtpCodecCapability, set_channels);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_GetSdpFmtpLine(
    rtcRtpCodecCapabilityHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpCodecCapability, sdp_fmtp_line);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_SetSdpFmtpLine(
    rtcRtpCodecCapabilityHandle handle,
    const char* value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, string, RTCRtpCodecCapability, set_sdp_fmtp_line);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtpCodecCapabilityList interop methods
 * ----------------------------------------------------------------------
 */

int LIB_WEBRTC_CALL
RTCRtpCodecCapabilityList_GetCount (
    rtcRtpCodecCapabilityListHandle handle
) noexcept
{
    DECLARE_LIST_GET_COUNT(handle, RTCRtpCodecCapabilityList);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapabilityList_GetItem (
    rtcRtpCodecCapabilityListHandle handle,
    int index,
    rtcRtpCodecCapabilityHandle* pOutRetVal
) noexcept
{
    DECLARE_LIST_GET_ITEM(handle, index, pOutRetVal, rtcRtpCodecCapabilityHandle, RTCRtpCodecCapabilityList, RTCRtpCodecCapability);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtpHeaderExtensionCapability interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_GetUri(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpHeaderExtensionCapability, uri);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_SetUri(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    const char* value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, string, RTCRtpHeaderExtensionCapability, set_uri);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_GetPreferredId(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCRtpHeaderExtensionCapability, preferred_id);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_SetPreferredId(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    int value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, int, RTCRtpHeaderExtensionCapability, set_preferred_id);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_GetPreferredEncrypt(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    rtcBool32* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcBool32, RTCRtpHeaderExtensionCapability, preferred_encrypt);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_SetPreferredEncrypt(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    rtcBool32 value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, bool, RTCRtpHeaderExtensionCapability, set_preferred_encrypt);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtpHeaderExtensionCapabilityList interop methods
 * ----------------------------------------------------------------------
 */

int LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapabilityList_GetCount (
    rtcRtpHeaderExtensionCapabilityListHandle handle
) noexcept
{
    DECLARE_LIST_GET_COUNT(handle, RTCRtpHeaderExtensionCapabilityList);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapabilityList_GetItem (
    rtcRtpHeaderExtensionCapabilityListHandle handle,
    int index,
    rtcRtpHeaderExtensionCapabilityHandle* pOutRetVal
) noexcept
{
    DECLARE_LIST_GET_ITEM(handle, index, pOutRetVal, rtcRtpHeaderExtensionCapabilityHandle, RTCRtpHeaderExtensionCapabilityList, RTCRtpHeaderExtensionCapability);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtpCapabilities interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCapabilities_GetCodecs(
    rtcRtpCapabilitiesHandle handle,
    rtcRtpCodecCapabilityListHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_LIST_HANDLE(handle, pOutRetVal, rtcRtpCodecCapabilityListHandle, RTCRtpCapabilities, RTCRtpCodecCapabilityList, codecs);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCapabilities_SetCodecs(
    rtcRtpCapabilitiesHandle handle,
    rtcRtpCodecCapabilityListHandle hCodecs
) noexcept
{
    DECLARE_SET_LIST_HANDLE(handle, hCodecs, RTCRtpCapabilities, RTCRtpCodecCapabilityList, set_codecs);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCapabilities_GetHeaderExtensions(
    rtcRtpCapabilitiesHandle handle,
    rtcRtpHeaderExtensionCapabilityListHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_LIST_HANDLE(handle, pOutRetVal, rtcRtpHeaderExtensionCapabilityListHandle, RTCRtpCapabilities, RTCRtpHeaderExtensionCapabilityList, header_extensions);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCapabilities_SetHeaderExtensions(
    rtcRtpCapabilitiesHandle handle,
    rtcRtpHeaderExtensionCapabilityListHandle hHeaderExtensions
) noexcept
{
    DECLARE_SET_LIST_HANDLE(handle, hHeaderExtensions, RTCRtpCapabilities, RTCRtpHeaderExtensionCapabilityList, set_header_extensions);
}
