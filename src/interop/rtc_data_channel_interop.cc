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
    CHECK_NATIVE_HANDLE(dataChannel);
    CHECK_POINTER(label);
    if (label_size < 1) {
        return rtcResultU4::kInvalidParameter;
    }
    ZERO_MEMORY(label, label_size);

    rtcResultU4 result = rtcResultU4::kSuccess;
    scoped_refptr<RTCDataChannelImpl> pDataChannel = static_cast<RTCDataChannelImpl*>(dataChannel);
    string sLabel = pDataChannel->label();
    if (sLabel.size() > 0) {
        size_t dstSize = static_cast<size_t>(label_size);
        size_t len = std::min(dstSize, sLabel.size());
        if (sLabel.size() > len) {
            result = rtcResultU4::kBufferTooSmall;
        }
        strncpy(label, sLabel.c_string(), len);
    }

    return result;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_GetId(
    rtcDataChannelHandle dataChannel,
    int* id
) noexcept
{
    CHECK_NATIVE_HANDLE(dataChannel);
    CHECK_POINTER(id);
    RESET_OUT_POINTER_EX(id, 0);

    scoped_refptr<RTCDataChannelImpl> pDataChannel = static_cast<RTCDataChannelImpl*>(dataChannel);
    *id = pDataChannel->id();
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_GetState(
    rtcDataChannelHandle dataChannel,
    rtcDataChannelState* state
) noexcept
{
    CHECK_NATIVE_HANDLE(dataChannel);
    CHECK_POINTER(state);
    RESET_OUT_POINTER_EX(state, static_cast<rtcDataChannelState>(-1));

    scoped_refptr<RTCDataChannelImpl> pDataChannel = static_cast<RTCDataChannelImpl*>(dataChannel);
    *state = static_cast<rtcDataChannelState>(pDataChannel->state());
    return rtcResultU4::kSuccess;
}