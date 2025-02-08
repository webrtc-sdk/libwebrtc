/**
 * File provided for Reference Use Only by isoft (c) 2025.
 * Copyright (c) isoft. All rights reserved.
 * 
 */

#include "rtc_rtp_parameters.h"
#include "interop_api.h"

using namespace libwebrtc;

/*
 * ----------------------------------------------------------------------
 * RTCRtcpFeedback interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpFeedback_GetType(
    rtcRtcpFeedbackHandle handle,
    rtcRtcpFeedbackType* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcRtcpFeedbackType, RTCRtcpFeedback, type);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpFeedback_SetType(
    rtcRtcpFeedbackHandle handle,
    rtcRtcpFeedbackType value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, RTCRtcpFeedbackType, RTCRtcpFeedback, set_type);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpFeedback_GetMessageType(
    rtcRtcpFeedbackHandle handle,
    rtcRtcpFeedbackMessageType* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcRtcpFeedbackMessageType, RTCRtcpFeedback, message_type);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpFeedback_SetMessageType(
    rtcRtcpFeedbackHandle handle,
    rtcRtcpFeedbackMessageType value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, RTCRtcpFeedbackMessageType, RTCRtcpFeedback, set_message_type);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtcpFeedbackList interop methods
 * ----------------------------------------------------------------------
 */

int LIB_WEBRTC_CALL
RTCRtcpFeedbackList_GetCount (
    rtcRtcpFeedbackListHandle handle
) noexcept
{
    DECLARE_LIST_GET_COUNT(handle, RTCRtcpFeedbackList);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpFeedbackList_GetItem (
    rtcRtcpFeedbackListHandle handle,
    int index,
    rtcRtcpFeedbackHandle* pOutRetVal
) noexcept
{
    DECLARE_LIST_GET_ITEM(handle, index, pOutRetVal, rtcRtcpFeedbackHandle, RTCRtcpFeedbackList, RTCRtcpFeedback);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtpExtension interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtension_ToString (
    rtcRtpExtensionHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpExtension, ToString);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtension_GetUri (
    rtcRtpExtensionHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpExtension, uri);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtension_SetUri (
    rtcRtpExtensionHandle handle,
    const char* value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, string, RTCRtpExtension, set_uri);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtension_GetId (
    rtcRtpExtensionHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCRtpExtension, id);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtension_SetId (
    rtcRtpExtensionHandle handle,
    int value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, int, RTCRtpExtension, set_id);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtension_GetEncrypt (
    rtcRtpExtensionHandle handle,
    rtcBool32* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcBool32, RTCRtpExtension, encrypt);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtension_SetEncrypt (
    rtcRtpExtensionHandle handle,
    rtcBool32 value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, bool, RTCRtpExtension, set_encrypt);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtpExtensionList interop methods
 * ----------------------------------------------------------------------
 */

int LIB_WEBRTC_CALL
RTCRtpExtensionList_GetCount (
    rtcRtpExtensionListHandle handle
) noexcept
{
    DECLARE_LIST_GET_COUNT(handle, RTCRtpExtensionList);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtensionList_GetItem (
    rtcRtpExtensionListHandle handle,
    int index,
    rtcRtpExtensionHandle* pOutRetVal
) noexcept
{
    DECLARE_LIST_GET_ITEM(handle, index, pOutRetVal, rtcRtpExtensionHandle, RTCRtpExtensionList, RTCRtpExtension);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtpCodecParametersPair interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParametersPair_Create(
    const rtcStringPair* pair,
    rtcRtpCodecParametersPairHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);

    scoped_refptr<RTCRtpCodecParametersPair> p;
    if (pair) {
        p = RTCRtpCodecParametersPair::Create(
            std::pair<string, string>(string(pair->key), string(pair->value))
        );
    }
    else {
        p = RTCRtpCodecParametersPair::Create();
    }
    *pOutRetVal = static_cast<rtcRtpCodecParametersPairHandle>(p.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParametersPair_GetKey(
    rtcRtpCodecParametersPairHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpCodecParametersPair, key);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParametersPair_GetValue(
    rtcRtpCodecParametersPairHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpCodecParametersPair, value);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtpCodecParametersMap interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParametersMap_Create(
    const rtcStringPair* pairs,
    int length,
    rtcRtpCodecParametersPairHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);
    if (length > 0) {
        CHECK_POINTER_EX(pairs, rtcResultU4::kInvalidParameter);
    }

    scoped_refptr<RTCRtpCodecParametersMap> pMap;
    std::vector<scoped_refptr<RTCRtpCodecParametersPair>> items;
    if (length > 0) {
        for (int i = 0; i < length; i++) {
            items.push_back(
                RTCRtpCodecParametersPair::Create(std::pair<string, string>(
                    string(pairs[i].key),
                    string(pairs[i].value)
                ))
            );
        }
    }
    pMap = RTCRtpCodecParametersMap::Create(items);
    *pOutRetVal = static_cast<rtcRtpCodecParametersPairHandle>(pMap.release());
    return rtcResultU4::kSuccess;
}

int LIB_WEBRTC_CALL
RTCRtpCodecParametersMap_GetCount (
    rtcRtpCodecParametersMapHandle handle
) noexcept
{
    DECLARE_LIST_GET_COUNT(handle, RTCRtpCodecParametersMap);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParametersMap_GetItem (
    rtcRtpCodecParametersMapHandle handle,
    int index,
    rtcRtpCodecParametersPairHandle* pOutRetVal
) noexcept
{
    DECLARE_LIST_GET_ITEM(handle, index, pOutRetVal, rtcRtpCodecParametersPairHandle, RTCRtpCodecParametersMap, RTCRtpCodecParametersPair);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtpCodecParameters interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetMimeType (
    rtcRtpCodecParametersHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpCodecParameters, mime_type);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetName (
    rtcRtpCodecParametersHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpCodecParameters, name);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetName (
    rtcRtpCodecParametersHandle handle,
    const char* value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, string, RTCRtpCodecParameters, set_name);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetKind (
    rtcRtpCodecParametersHandle handle,
    rtcMediaType* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcMediaType, RTCRtpCodecParameters, kind);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetKind (
    rtcRtpCodecParametersHandle handle,
    rtcMediaType value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, RTCMediaType, RTCRtpCodecParameters, set_kind);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetPayloadType (
    rtcRtpCodecParametersHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCRtpCodecParameters, payload_type);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetPayloadType (
    rtcRtpCodecParametersHandle handle,
    int value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, int, RTCRtpCodecParameters, set_payload_type);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetClockRate (
    rtcRtpCodecParametersHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCRtpCodecParameters, clock_rate);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetClockRate (
    rtcRtpCodecParametersHandle handle,
    int value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, int, RTCRtpCodecParameters, set_clock_rate);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetNumChannels (
    rtcRtpCodecParametersHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCRtpCodecParameters, num_channels);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetNumChannels (
    rtcRtpCodecParametersHandle handle,
    int value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, int, RTCRtpCodecParameters, set_num_channels);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetMaxPTime (
    rtcRtpCodecParametersHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCRtpCodecParameters, max_ptime);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetMaxPTime (
    rtcRtpCodecParametersHandle handle,
    int value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, int, RTCRtpCodecParameters, set_max_ptime);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetPTime (
    rtcRtpCodecParametersHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCRtpCodecParameters, ptime);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetPTime (
    rtcRtpCodecParametersHandle handle,
    int value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, int, RTCRtpCodecParameters, set_ptime);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetRtcpFeedBacks (
    rtcRtpCodecParametersHandle handle,
    rtcRtcpFeedbackListHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_LIST_HANDLE(handle, pOutRetVal, rtcRtcpFeedbackListHandle, RTCRtpCodecParameters, RTCRtcpFeedbackList, rtcp_feedback);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetRtcpFeedBacks (
    rtcRtpCodecParametersHandle handle,
    rtcRtcpFeedbackListHandle value
) noexcept
{
    DECLARE_SET_LIST_HANDLE(handle, value, RTCRtpCodecParameters, RTCRtcpFeedbackList, set_rtcp_feedback);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetParameters (
    rtcRtpCodecParametersHandle handle,
    rtcRtpCodecParametersMapHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpCodecParameters> p = static_cast<RTCRtpCodecParameters*>(handle);
    std::vector<std::pair<string, string>> parameters = p->parameters().std_vector();
    std::vector<scoped_refptr<RTCRtpCodecParametersPair>> pParameters;
    for (auto item : parameters) {
        pParameters.push_back(RTCRtpCodecParametersPair::Create(item));
    }
    scoped_refptr<RTCRtpCodecParametersMap> pMap = RTCRtpCodecParametersMap::Create(pParameters);
    if (pMap == nullptr) {
        return rtcResultU4::kUnknownError;
    }
    *pOutRetVal = static_cast<rtcRtpCodecParametersMapHandle>(pMap.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetParameters (
    rtcRtpCodecParametersHandle handle,
    rtcRtpCodecParametersMapHandle value
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpCodecParameters> p = static_cast<RTCRtpCodecParameters*>(handle);
    vector<std::pair<string, string>> parameters;
    if (value) {
        scoped_refptr<RTCRtpCodecParametersMap> pMap = static_cast<RTCRtpCodecParametersMap*>(value);
        parameters = pMap->to_parameters();
    }
    p->set_parameters(parameters);
    return rtcResultU4::kSuccess;
}

/*
 * ----------------------------------------------------------------------
 * RTCRtpCodecParametersList interop methods
 * ----------------------------------------------------------------------
 */

int LIB_WEBRTC_CALL
RTCRtpCodecParametersList_GetCount (
    rtcRtpCodecParametersListHandle handle
) noexcept
{
    DECLARE_LIST_GET_COUNT(handle, RTCRtpCodecParametersList);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParametersList_GetItem (
    rtcRtpCodecParametersListHandle handle,
    int index,
    rtcRtpCodecParametersHandle* pOutRetVal
) noexcept
{
    DECLARE_LIST_GET_ITEM(handle, index, pOutRetVal, rtcRtpCodecParametersHandle, RTCRtpCodecParametersList, RTCRtpCodecParameters);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtcpParameters interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_GetSSRC (
    rtcRtcpParametersHandle handle,
    unsigned int* value
) noexcept
{
    DECLARE_GET_VALUE(handle, value, unsigned int, RTCRtcpParameters, ssrc);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_SetSSRC (
    rtcRtcpParametersHandle handle,
    unsigned int value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, uint32_t, RTCRtcpParameters, set_ssrc);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_GetCName (
    rtcRtcpParametersHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtcpParameters, cname);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_SetCName (
    rtcRtcpParametersHandle handle,
    const char* value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, string, RTCRtcpParameters, set_cname);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_GetReducedSize (
    rtcRtcpParametersHandle handle,
    rtcBool32* value
) noexcept
{
    DECLARE_GET_VALUE(handle, value, rtcBool32, RTCRtcpParameters, reduced_size);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_SetReducedSize (
    rtcRtcpParametersHandle handle,
    rtcBool32 value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, bool, RTCRtcpParameters, set_reduced_size);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_GetMux (
    rtcRtcpParametersHandle handle,
    rtcBool32* value
) noexcept
{
    DECLARE_GET_VALUE(handle, value, rtcBool32, RTCRtcpParameters, mux);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_SetMux (
    rtcRtcpParametersHandle handle,
    rtcBool32 value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, bool, RTCRtcpParameters, set_mux);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtcpParametersList interop methods
 * ----------------------------------------------------------------------
 */

int LIB_WEBRTC_CALL
RTCRtcpParametersList_GetCount (
    rtcRtcpParametersListHandle handle
) noexcept
{
    DECLARE_LIST_GET_COUNT(handle, RTCRtcpParametersList);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParametersList_GetItem (
    rtcRtcpParametersListHandle handle,
    int index,
    rtcRtcpParametersHandle* pOutRetVal
) noexcept
{
    DECLARE_LIST_GET_ITEM(handle, index, pOutRetVal, rtcRtcpParametersHandle, RTCRtcpParametersList, RTCRtcpParameters);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtpEncodingParameters interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_Create (
    rtcRtpEncodingParametersHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);

    scoped_refptr<RTCRtpEncodingParameters> p = RTCRtpEncodingParameters::Create();
    *pOutRetVal = static_cast<rtcRtpEncodingParametersHandle>(p.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetSSRC (
    rtcRtpEncodingParametersHandle handle,
    unsigned int* value
) noexcept
{
    DECLARE_GET_VALUE(handle, value, unsigned int, RTCRtpEncodingParameters, ssrc);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetSSRC (
    rtcRtpEncodingParametersHandle handle,
    unsigned int value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, uint32_t, RTCRtpEncodingParameters, set_ssrc);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetBitratePriority (
    rtcRtpEncodingParametersHandle handle,
    double* value
) noexcept
{
    DECLARE_GET_VALUE(handle, value, double, RTCRtpEncodingParameters, bitrate_priority);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetBitratePriority (
    rtcRtpEncodingParametersHandle handle,
    double value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, double, RTCRtpEncodingParameters, set_bitrate_priority);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetNetworkPriority (
    rtcRtpEncodingParametersHandle handle,
    rtcNetworkPriority* value
) noexcept
{
    DECLARE_GET_VALUE(handle, value, rtcNetworkPriority, RTCRtpEncodingParameters, network_priority);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetNetworkPriority (
    rtcRtpEncodingParametersHandle handle,
    rtcNetworkPriority value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, RTCPriority, RTCRtpEncodingParameters, set_network_priority);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetMaxBitrateBps (
    rtcRtpEncodingParametersHandle handle,
    int* value
) noexcept
{
    DECLARE_GET_VALUE(handle, value, int, RTCRtpEncodingParameters, max_bitrate_bps);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetMaxBitrateBps (
    rtcRtpEncodingParametersHandle handle,
    int value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, int, RTCRtpEncodingParameters, set_max_bitrate_bps);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetMinBitrateBps (
    rtcRtpEncodingParametersHandle handle,
    int* value
) noexcept
{
    DECLARE_GET_VALUE(handle, value, int, RTCRtpEncodingParameters, min_bitrate_bps);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetMinBitrateBps (
    rtcRtpEncodingParametersHandle handle,
    int value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, int, RTCRtpEncodingParameters, set_min_bitrate_bps);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetMaxFramerate (
    rtcRtpEncodingParametersHandle handle,
    double* value
) noexcept
{
    DECLARE_GET_VALUE(handle, value, double, RTCRtpEncodingParameters, max_framerate);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetMaxFramerate (
    rtcRtpEncodingParametersHandle handle,
    double value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, double, RTCRtpEncodingParameters, set_max_framerate);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetNumTemporalLayers (
    rtcRtpEncodingParametersHandle handle,
    int* value
) noexcept
{
    DECLARE_GET_VALUE(handle, value, int, RTCRtpEncodingParameters, num_temporal_layers);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetNumTemporalLayers (
    rtcRtpEncodingParametersHandle handle,
    int value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, int, RTCRtpEncodingParameters, set_num_temporal_layers);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetScaleResolutionDownBy (
    rtcRtpEncodingParametersHandle handle,
    double* value
) noexcept
{
    DECLARE_GET_VALUE(handle, value, double, RTCRtpEncodingParameters, scale_resolution_down_by);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetScaleResolutionDownBy (
    rtcRtpEncodingParametersHandle handle,
    double value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, double, RTCRtpEncodingParameters, set_scale_resolution_down_by);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetScalabilityMode (
    rtcRtpEncodingParametersHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpEncodingParameters, scalability_mode);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetScalabilityMode (
    rtcRtpEncodingParametersHandle handle,
    const char* value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, string, RTCRtpEncodingParameters, set_scalability_mode);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetActive (
    rtcRtpEncodingParametersHandle handle,
    rtcBool32* value
) noexcept
{
    DECLARE_GET_VALUE(handle, value, rtcBool32, RTCRtpEncodingParameters, active);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetActive (
    rtcRtpEncodingParametersHandle handle,
    rtcBool32 value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, bool, RTCRtpEncodingParameters, set_active);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetRid (
    rtcRtpEncodingParametersHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpEncodingParameters, rid);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetRid (
    rtcRtpEncodingParametersHandle handle,
    const char* value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, string, RTCRtpEncodingParameters, set_rid);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetAdaptivePTime(
    rtcRtpEncodingParametersHandle handle,
    rtcBool32* value
) noexcept
{
    DECLARE_GET_VALUE(handle, value, rtcBool32, RTCRtpEncodingParameters, adaptive_ptime);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetAdaptivePTime (
    rtcRtpEncodingParametersHandle handle,
    rtcBool32 value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, bool, RTCRtpEncodingParameters, set_adaptive_ptime);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtpEncodingParametersList interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParametersList_Create (
    rtcRtpEncodingParametersHandle* items,
    int count,
    rtcRtpEncodingParametersListHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_POINTER_EX(items, rtcResultU4::kInvalidParameter);
    if (count <= 0) {
        return rtcResultU4::kInvalidParameter;
    }

    std::vector<scoped_refptr<RTCRtpEncodingParameters>> vecItems;
    for (int i = 0; i < count; i++) {
        scoped_refptr<RTCRtpEncodingParameters> pItem = static_cast<RTCRtpEncodingParameters*>(items[i]);
        vecItems.push_back(pItem);
    }
    scoped_refptr<RTCRtpEncodingParametersList> p = RTCRtpEncodingParametersList::Create(vecItems);
    *pOutRetVal = static_cast<rtcRtpEncodingParametersListHandle>(p.release());
    return rtcResultU4::kSuccess;
}

int LIB_WEBRTC_CALL
RTCRtpEncodingParametersList_GetCount (
    rtcRtpEncodingParametersListHandle handle
) noexcept
{
    DECLARE_LIST_GET_COUNT(handle, RTCRtpEncodingParametersList);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParametersList_GetItem (
    rtcRtpEncodingParametersListHandle handle,
    int index,
    rtcRtpEncodingParametersHandle* pOutRetVal
) noexcept
{
    DECLARE_LIST_GET_ITEM(handle, index, pOutRetVal, rtcRtpEncodingParametersHandle, RTCRtpEncodingParametersList, RTCRtpEncodingParameters);
}

/*
 * ----------------------------------------------------------------------
 * RTCRtpParameters interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_GetTransactionId (
    rtcRtpParametersHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpParameters, transaction_id);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_SetTransactionId (
    rtcRtpParametersHandle handle,
    const char* value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, string, RTCRtpParameters, set_transaction_id);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_GetMid (
    rtcRtpParametersHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpParameters, mid);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_SetMid (
    rtcRtpParametersHandle handle,
    const char* value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, string, RTCRtpParameters, set_mid);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_GetCodecs (
    rtcRtpParametersHandle handle,
    rtcRtpCodecParametersListHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_LIST_HANDLE(handle, pOutRetVal, rtcRtpCodecParametersListHandle, RTCRtpParameters, RTCRtpCodecParametersList, codecs);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_SetCodecs (
    rtcRtpParametersHandle handle,
    rtcRtpCodecParametersListHandle value
) noexcept
{
    DECLARE_SET_LIST_HANDLE(handle, value, RTCRtpParameters, RTCRtpCodecParametersList, set_codecs);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_GetHeaderExtensions (
    rtcRtpParametersHandle handle,
    rtcRtpExtensionListHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_LIST_HANDLE(handle, pOutRetVal, rtcRtpExtensionListHandle, RTCRtpParameters, RTCRtpExtensionList, header_extensions);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_SetHeaderExtensions (
    rtcRtpParametersHandle handle,
    rtcRtpExtensionListHandle value
) noexcept
{
    DECLARE_SET_LIST_HANDLE(handle, value, RTCRtpParameters, RTCRtpExtensionList, set_header_extensions);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_GetEncodings (
    rtcRtpParametersHandle handle,
    rtcRtpEncodingParametersListHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_LIST_HANDLE(handle, pOutRetVal, rtcRtpEncodingParametersListHandle, RTCRtpParameters, RTCRtpEncodingParametersList, encodings);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_SetEncodings (
    rtcRtpParametersHandle handle,
    rtcRtpEncodingParametersListHandle value
) noexcept
{
    DECLARE_SET_LIST_HANDLE(handle, value, RTCRtpParameters, RTCRtpEncodingParametersList, set_encodings);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_GetRtcpParameters (
    rtcRtpParametersHandle handle,
    rtcRtcpParametersHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcRtcpParametersHandle, RTCRtpParameters, rtcp_parameters().release);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_SetRtcpParameters (
    rtcRtpParametersHandle handle,
    rtcRtcpParametersHandle value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, RTCRtcpParameters*, RTCRtpParameters, set_rtcp_parameters);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_GetDegradationPreference (
    rtcRtpParametersHandle handle,
    rtcDegradationPreference* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcDegradationPreference, RTCRtpParameters, GetDegradationPreference);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_SetDegradationPreference (
    rtcRtpParametersHandle handle,
    rtcDegradationPreference value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, RTCDegradationPreference, RTCRtpParameters, SetDegradationPreference);
}
