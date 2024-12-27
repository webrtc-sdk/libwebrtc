#include "rtc_rtp_capabilities.h"
#include "rtc_rtp_capabilities_lists.h"
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
    CHECK_NATIVE_HANDLE(handle);
    ZERO_MEMORY(value, sz_value);
    CHECK_POINTER(value);
    if (sz_value < 1) {
        return rtcResultU4::kBufferTooSmall;
    }

    scoped_refptr<RTCRtpCodecCapability> pCodec = static_cast<RTCRtpCodecCapability*>(handle);
    string strValue = pCodec->mime_type();
    size_t len = strValue.copy_to(value, static_cast<size_t>(sz_value));
    return strValue.size() > len
        ? rtcResultU4::kBufferTooSmall
        : rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_SetMimeType(
    rtcRtpCodecCapabilityHandle handle,
    const char* value
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpCodecCapability> pCodec = static_cast<RTCRtpCodecCapability*>(handle);
    pCodec->set_mime_type(string(value));
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_GetClockRate(
    rtcRtpCodecCapabilityHandle handle,
    int* pOutRetVal
) noexcept
{
    CHECK_POINTER(pOutRetVal);
    RESET_OUT_POINTER_EX(pOutRetVal, 0);
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpCodecCapability> pCodec = static_cast<RTCRtpCodecCapability*>(handle);
    *pOutRetVal = pCodec->clock_rate();
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_SetClockRate(
    rtcRtpCodecCapabilityHandle handle,
    int value
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpCodecCapability> pCodec = static_cast<RTCRtpCodecCapability*>(handle);
    pCodec->set_clock_rate(value);
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_GetChannels(
    rtcRtpCodecCapabilityHandle handle,
    int* pOutRetVal
) noexcept
{
    CHECK_POINTER(pOutRetVal);
    RESET_OUT_POINTER_EX(pOutRetVal, 0);
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpCodecCapability> pCodec = static_cast<RTCRtpCodecCapability*>(handle);
    *pOutRetVal = pCodec->channels();
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_SetChannels(
    rtcRtpCodecCapabilityHandle handle,
    int value
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpCodecCapability> pCodec = static_cast<RTCRtpCodecCapability*>(handle);
    pCodec->set_channels(value);
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_GetSdpFmtpLine(
    rtcRtpCodecCapabilityHandle handle,
    char* value,
    int sz_value
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);
    ZERO_MEMORY(value, sz_value);
    CHECK_POINTER(value);
    if (sz_value < 1) {
        return rtcResultU4::kBufferTooSmall;
    }

    scoped_refptr<RTCRtpCodecCapability> pCodec = static_cast<RTCRtpCodecCapability*>(handle);
    string strValue = pCodec->sdp_fmtp_line();
    size_t len = strValue.copy_to(value, static_cast<size_t>(sz_value));
    return strValue.size() > len
        ? rtcResultU4::kBufferTooSmall
        : rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_SetSdpFmtpLine(
    rtcRtpCodecCapabilityHandle handle,
    const char* value
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpCodecCapability> pCodec = static_cast<RTCRtpCodecCapability*>(handle);
    pCodec->set_sdp_fmtp_line(string(value));
    return rtcResultU4::kSuccess;
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
    CHECK_POINTER_EX(handle, 0);
    scoped_refptr<RTCRtpCodecCapabilityList> pList = static_cast<RTCRtpCodecCapabilityList*>(handle);
    return static_cast<int>(pList->count());
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapabilityList_GetItem (
    rtcRtpCodecCapabilityListHandle handle,
    int index,
    rtcRtpCodecCapabilityHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_NATIVE_HANDLE(handle);
    
    scoped_refptr<RTCRtpCodecCapabilityList> pList = static_cast<RTCRtpCodecCapabilityList*>(handle);
    if (index < 0 || static_cast<size_t>(index) >= pList->count()) {
        return rtcResultU4::kOutOfRange;
    }
    scoped_refptr<RTCRtpCodecCapability> pItem = pList->item(static_cast<size_t>(index));
    *pOutRetVal = static_cast<rtcRtpCodecCapabilityHandle>(pItem.release());
    return rtcResultU4::kSuccess;
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
    CHECK_NATIVE_HANDLE(handle);
    ZERO_MEMORY(value, sz_value);
    CHECK_POINTER(value);
    if (sz_value < 1) {
        return rtcResultU4::kBufferTooSmall;
    }

    scoped_refptr<RTCRtpHeaderExtensionCapability> pHeaderExtension = static_cast<RTCRtpHeaderExtensionCapability*>(handle);
    string strValue = pHeaderExtension->uri();
    size_t len = strValue.copy_to(value, static_cast<size_t>(sz_value));
    return strValue.size() > len
        ? rtcResultU4::kBufferTooSmall
        : rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_SetUri(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    const char* value
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpHeaderExtensionCapability> pHeaderExtension = static_cast<RTCRtpHeaderExtensionCapability*>(handle);
    pHeaderExtension->set_uri(string(value));
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_GetPreferredId(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    int* pOutRetVal
) noexcept
{
    CHECK_POINTER(pOutRetVal);
    RESET_OUT_POINTER_EX(pOutRetVal, 0);
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpHeaderExtensionCapability> pHeaderExtension = static_cast<RTCRtpHeaderExtensionCapability*>(handle);
    *pOutRetVal = pHeaderExtension->preferred_id();
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_SetPreferredId(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    int value
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpHeaderExtensionCapability> pHeaderExtension = static_cast<RTCRtpHeaderExtensionCapability*>(handle);
    pHeaderExtension->set_preferred_id(value);
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_GetPreferredEncrypt(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    rtcBool32* pOutRetVal
) noexcept
{
    CHECK_POINTER(pOutRetVal);
    RESET_OUT_POINTER_EX(pOutRetVal, rtcBool32::kFalse);
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpHeaderExtensionCapability> pHeaderExtension = static_cast<RTCRtpHeaderExtensionCapability*>(handle);
    *pOutRetVal = pHeaderExtension->preferred_encrypt() ? rtcBool32::kTrue : rtcBool32::kFalse;
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_SetPreferredEncrypt(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    rtcBool32 value
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpHeaderExtensionCapability> pHeaderExtension = static_cast<RTCRtpHeaderExtensionCapability*>(handle);
    pHeaderExtension->set_preferred_encrypt(value != rtcBool32::kFalse);
    return rtcResultU4::kSuccess;
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
    CHECK_POINTER_EX(handle, 0);
    scoped_refptr<RTCRtpHeaderExtensionCapabilityList> pList =
        static_cast<RTCRtpHeaderExtensionCapabilityList*>(handle);
    return static_cast<int>(pList->count());
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapabilityList_GetItem (
    rtcRtpHeaderExtensionCapabilityListHandle handle,
    int index,
    rtcRtpHeaderExtensionCapabilityHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpHeaderExtensionCapabilityList> pList =
        static_cast<RTCRtpHeaderExtensionCapabilityList*>(handle);
    if (index < 0 || static_cast<size_t>(index) >= pList->count()) {
        return rtcResultU4::kOutOfRange;
    }
    scoped_refptr<RTCRtpHeaderExtensionCapability> pItem = pList->item(static_cast<size_t>(index));
    *pOutRetVal = static_cast<rtcRtpHeaderExtensionCapabilityHandle>(pItem.release());
    return rtcResultU4::kSuccess;
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
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpCapabilities> pRtpCapabilities = static_cast<RTCRtpCapabilities*>(handle);
    scoped_refptr<RTCRtpCodecCapabilityList> codecs = RTCRtpCodecCapabilityList::Create(
        pRtpCapabilities->codecs()
    );
    if (codecs == nullptr) {
        return rtcResultU4::kUnknownError;
    }
    *pOutRetVal = static_cast<rtcRtpCodecCapabilityListHandle>(codecs.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCapabilities_SetCodecs(
    rtcRtpCapabilitiesHandle handle,
    rtcRtpCodecCapabilityListHandle hCodecs
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER_EX(hCodecs, rtcResultU4::kInvalidParameter);

    scoped_refptr<RTCRtpCapabilities> pRtpCapabilities = static_cast<RTCRtpCapabilities*>(handle);
    scoped_refptr<RTCRtpCodecCapabilityList> codecs = static_cast<RTCRtpCodecCapabilityList*>(hCodecs);
    pRtpCapabilities->set_codecs(codecs->to_vector());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCapabilities_GetHeaderExtensions(
    rtcRtpCapabilitiesHandle handle,
    rtcRtpHeaderExtensionCapabilityListHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpCapabilities> pRtpCapabilities = static_cast<RTCRtpCapabilities*>(handle);
    scoped_refptr<RTCRtpHeaderExtensionCapabilityList> header_extensions = RTCRtpHeaderExtensionCapabilityList::Create(
        pRtpCapabilities->header_extensions()
    );
    if (header_extensions == nullptr) {
        return rtcResultU4::kUnknownError;
    }
    *pOutRetVal = static_cast<rtcRtpHeaderExtensionCapabilityListHandle>(header_extensions.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCapabilities_SetHeaderExtensions(
    rtcRtpCapabilitiesHandle handle,
    rtcRtpHeaderExtensionCapabilityListHandle hHeaderExtensions
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER_EX(hHeaderExtensions, rtcResultU4::kInvalidParameter);

    scoped_refptr<RTCRtpCapabilities> pRtpCapabilities = static_cast<RTCRtpCapabilities*>(handle);
    scoped_refptr<RTCRtpHeaderExtensionCapabilityList> header_extensions = 
        static_cast<RTCRtpHeaderExtensionCapabilityList*>(hHeaderExtensions);
    pRtpCapabilities->set_header_extensions(header_extensions->to_vector());
    return rtcResultU4::kSuccess;
}
