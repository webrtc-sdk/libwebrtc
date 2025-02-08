/**
 * File provided for Reference Use Only by isoft (c) 2025.
 * Copyright (c) isoft. All rights reserved.
 * 
 */

#include "rtc_rtp_receiver.h"
#include "interop_api.h"
#include "rtc_media_stream.h"
#include "rtc_dtls_transport.h"
#include "src/rtc_rtp_receiver_impl.h"

using namespace libwebrtc;

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_GetTrack (
    rtcRtpReceiverHandle handle,
    rtcMediaTrackHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcMediaTrackHandle, RTCRtpReceiver, track().release);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_GetDtlsTransport (
    rtcRtpReceiverHandle handle,
    rtcDtlsTransportHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcDtlsTransportHandle, RTCRtpReceiver, dtls_transport().release);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_GetStreamIds (
    rtcRtpReceiverHandle handle,
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

    scoped_refptr<RTCRtpReceiver> p = static_cast<RTCRtpReceiver*>(handle);
    string strValue = string::join("\n", p->stream_ids());
    size_t len = strValue.copy_to(value, sz_value);
    return strValue.size() > len
        ? rtcResultU4::kBufferTooSmall
        : rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_GetStreams (
    rtcRtpReceiverHandle handle,
    rtcMediaStreamListHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_LIST_HANDLE(handle, pOutRetVal, rtcMediaStreamListHandle, RTCRtpReceiver, RTCMediaStreamList, streams);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_GetMediaType (
    rtcRtpReceiverHandle handle,
    rtcMediaType* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcMediaType, RTCRtpReceiver, media_type);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_GetId (
    rtcRtpReceiverHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCRtpReceiver, id);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_GetParameters (
    rtcRtpReceiverHandle handle,
    rtcRtpParametersHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcRtpParametersHandle, RTCRtpReceiver, parameters().release);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_SetParameters (
    rtcRtpReceiverHandle handle,
    rtcRtpParametersHandle value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, RTCRtpParameters*, RTCRtpReceiver, set_parameters);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_RegisterObserver (
    rtcRtpReceiverHandle handle,
    rtcRtpReceiverObserverCallbacks* callbacks
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER_EX(callbacks, rtcResultU4::kInvalidParameter);
    
    // unregistered
    RTCRtpReceiver_UnregisterObserver(handle);

    scoped_refptr<RTCRtpReceiver> p = static_cast<RTCRtpReceiver*>(handle);
    RTCRtpReceiverObserver* pObserver = static_cast<RTCRtpReceiverObserver*>
    (
        new RTCRtpReceiverObserverImpl(static_cast<void*>(callbacks))
    );
    p->SetObserver(pObserver);
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_UnregisterObserver(
    rtcRtpReceiverHandle handle
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCRtpReceiver> p = static_cast<RTCRtpReceiver*>(handle);
    RTCRtpReceiverObserverImpl* pObserverImpl = static_cast<RTCRtpReceiverObserverImpl*>(p->GetObserver());
    p->SetObserver(nullptr);    
    if (pObserverImpl) {
        delete pObserverImpl;
    }
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_SetJitterBufferMinimumDelay (
    rtcRtpReceiverHandle handle,
    double value
) noexcept
{
    DECLARE_SET_VALUE(handle, value, double, RTCRtpReceiver, SetJitterBufferMinimumDelay);
}

int LIB_WEBRTC_CALL
RTCRtpReceiverList_GetCount (
    rtcRtpReceiverListHandle handle
) noexcept
{
    DECLARE_LIST_GET_COUNT(handle, RTCRtpReceiverList);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiverList_GetItem (
    rtcRtpReceiverListHandle handle,
    int index,
    rtcRtpReceiverHandle* pOutRetVal
) noexcept
{
    DECLARE_LIST_GET_ITEM(handle, index, pOutRetVal, rtcRtpReceiverHandle, RTCRtpReceiverList, RTCRtpReceiver);
}
