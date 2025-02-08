/**
 * File provided for Reference Use Only by isoft (c) 2025.
 * Copyright (c) isoft. All rights reserved.
 * 
 */

#include "rtc_dtmf_sender.h"
#include "src/rtc_dtmf_sender_impl.h"
#include "interop_api.h"

using namespace libwebrtc;

rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_RegisterObserver (
    rtcDtmfSenderHandle handle,
    rtcDtmfSenderObserverCallbacks* callbacks
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER_EX(callbacks, rtcResultU4::kInvalidParameter);
    
    // unregistered
    RTCDtmfSender_UnregisterObserver(handle);

    scoped_refptr<RTCDtmfSender> p = static_cast<RTCDtmfSender*>(handle);
    RTCDtmfSenderObserver* pObserver = static_cast<RTCDtmfSenderObserver*>
    (
        new RTCDtmfSenderObserverImpl(static_cast<void*>(callbacks))
    );
    p->RegisterObserver(pObserver);
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_UnregisterObserver (
    rtcDtmfSenderHandle handle
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCDtmfSender> p = static_cast<RTCDtmfSender*>(handle);
    RTCDtmfSenderObserverImpl* pObserverImpl = static_cast<RTCDtmfSenderObserverImpl*>(p->GetObserver());
    p->UnregisterObserver();    
    if (pObserverImpl) {
        delete pObserverImpl;
    }
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_InsertDtmf (
    rtcDtmfSenderHandle handle,
    const char* tones,
    int duration,
    int inter_tone_gap,
    const int* comma_delay /*= nullptr*/
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCDtmfSender> p = static_cast<RTCDtmfSender*>(handle);
    if (comma_delay) {
        p->InsertDtmf(string(tones), duration, inter_tone_gap, *comma_delay);
    }
    else {
        p->InsertDtmf(string(tones), duration, inter_tone_gap);
    }
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_CanInsertDtmf (
    rtcDtmfSenderHandle handle,
    rtcBool32* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcBool32, RTCDtmfSender, CanInsertDtmf);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_GetTones (
    rtcDtmfSenderHandle handle,
    char* value,
    int sz_value
) noexcept
{
    DECLARE_GET_STRING(handle, value, sz_value, RTCDtmfSender, tones);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_GetDuration (
    rtcDtmfSenderHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCDtmfSender, duration);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_GetInterToneGap (
    rtcDtmfSenderHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCDtmfSender, inter_tone_gap);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_GetCommaDelay (
    rtcDtmfSenderHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCDtmfSender, comma_delay);
}
