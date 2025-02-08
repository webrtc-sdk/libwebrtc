/**
 * File provided for Reference Use Only by isoft (c) 2025.
 * Copyright (c) isoft. All rights reserved.
 * 
 */

#include "rtc_data_channel.h"
#include "interop_api.h"
#include "src/rtc_data_channel_impl.h"

using namespace libwebrtc;

rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_Send(
    rtcDataChannelHandle dataChannel,
    const unsigned char* data,
    int data_size,
    rtcBool32 binary /*= rtcBool32::kFalse*/
) noexcept
{
    CHECK_NATIVE_HANDLE(dataChannel);

    scoped_refptr<RTCDataChannel> pDataChannel = static_cast<RTCDataChannel*>(dataChannel);
    pDataChannel->Send(
        static_cast<const uint8_t*>(data),
        data_size,
        binary != rtcBool32::kFalse
    );
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_Close(
    rtcDataChannelHandle dataChannel
) noexcept
{
    CHECK_NATIVE_HANDLE(dataChannel);

    scoped_refptr<RTCDataChannelImpl> pDataChannel = static_cast<RTCDataChannelImpl*>(dataChannel);
    RTCDataChannelObserverImpl* pObserverImpl = static_cast<RTCDataChannelObserverImpl*>(pDataChannel->GetObserver());
    pDataChannel->Close();
    if (pObserverImpl) {
        delete pObserverImpl;
    }
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_RegisterObserver(
    rtcDataChannelHandle dataChannel,
    rtcDataChannelObserverCallbacks* callbacks
) noexcept
{
    CHECK_NATIVE_HANDLE(dataChannel);
    CHECK_POINTER_EX(callbacks, rtcResultU4::kInvalidParameter);
    
    // unregistered
    RTCDataChannel_UnregisterObserver(dataChannel);

    RTCDataChannelObserver* pObserver = static_cast<RTCDataChannelObserver*>(new RTCDataChannelObserverImpl(callbacks));
    scoped_refptr<RTCDataChannel> pDataChannel = static_cast<RTCDataChannel*>(dataChannel);
    pDataChannel->RegisterObserver(pObserver);
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_UnregisterObserver(
    rtcDataChannelHandle dataChannel
) noexcept
{
    CHECK_NATIVE_HANDLE(dataChannel);

    scoped_refptr<RTCDataChannelImpl> pDataChannel = static_cast<RTCDataChannelImpl*>(dataChannel);
    RTCDataChannelObserverImpl* pObserverImpl = static_cast<RTCDataChannelObserverImpl*>(pDataChannel->GetObserver());
    pDataChannel->UnregisterObserver();    
    if (pObserverImpl) {
        delete pObserverImpl;
    }
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_GetLabel(
    rtcDataChannelHandle dataChannel,
    char* label,
    int label_size
) noexcept
{
    DECLARE_GET_STRING(dataChannel, label, label_size, RTCDataChannel, label);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_GetId(
    rtcDataChannelHandle dataChannel,
    int* id
) noexcept
{
    DECLARE_GET_VALUE(dataChannel, id, int, RTCDataChannel, id);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_GetState(
    rtcDataChannelHandle dataChannel,
    rtcDataChannelState* state
) noexcept
{
    DECLARE_GET_VALUE(dataChannel, state, rtcDataChannelState, RTCDataChannel, state);
}