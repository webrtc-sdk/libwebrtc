/**
 * File provided for Reference Use Only by isoft (c) 2025.
 * Copyright (c) isoft. All rights reserved.
 * 
 */

#include "interop_api.h"
#include "rtc_peerconnection_factory.h"
#include "libwebrtc.h"

using namespace libwebrtc;

rtcBool32 LIB_WEBRTC_CALL LibWebRTC_Initialize() noexcept
{
    return LibWebRTC::Initialize()
        ? rtcBool32::kTrue
        : rtcBool32::kFalse;
}

rtcPeerConnectionFactoryHandle LIB_WEBRTC_CALL LibWebRTC_CreateRTCPeerConnectionFactory(
    rtcBool32 use_dummy_audio /*= rtcBool32::kFalse*/
) noexcept
{
    scoped_refptr<RTCPeerConnectionFactory> p = 
        LibWebRTC::CreateRTCPeerConnectionFactory(use_dummy_audio != rtcBool32::kFalse);
    return static_cast<rtcPeerConnectionFactoryHandle>(p.release());
}

void LIB_WEBRTC_CALL LibWebRTC_Terminate() noexcept
{
    LibWebRTC::Terminate();
}

rtcResultU4 LIB_WEBRTC_CALL
LibWebRTC_GetErrorMessage(
    rtcResultU4 code,
    char* pBuffer,
    unsigned int sizeOfBuffer
) noexcept
{
    CHECK_POINTER(pBuffer);
    if (sizeOfBuffer == 0) {
        return rtcResultU4::kBufferTooSmall;
    }
    ZERO_MEMORY(pBuffer, sizeOfBuffer);

    char szTemp[256] = {0};
    const char* message = "";
    switch (code)
    {
        case rtcResultU4::kSuccess:
            message = "The operation was successful.";
            break;
        case rtcResultU4::kUnknownError:
            message = "Unknown internal error.";
            break;
        case rtcResultU4::kInvalidParameter:
            message = "A parameter passed to the API function was invalid.";
            break;
        case rtcResultU4::kInvalidOperation:
            message = "The operation cannot be performed in the current state.";
            break;
        case rtcResultU4::kWrongThread:
            message = "A call was made to an API function on the wrong thread.";
            break;
        case rtcResultU4::kNotFound:
            message = "An object was not found.";
            break;
        case rtcResultU4::kInvalidNativeHandle:
            message = "An interop handle referencing a native object instance is invalid, although the API function was expecting a valid object.";
            break;
        case rtcResultU4::kNotInitialized:
            message = "The API object is not initialized, and cannot as a result perform the given operation.";
            break;
        case rtcResultU4::kUnsupported:
            message = "The current operation is not supported by the implementation.";
            break;
        case rtcResultU4::kOutOfRange:
            message = "An argument was passed to the API function with a value out of the expected range.";
            break;
        case rtcResultU4::kBufferTooSmall:
            message = "The buffer provided by the caller was too small for the operation to complete successfully.";
            break;
        case rtcResultU4::kPeerConnectionClosed:
            message = "The peer connection is closed, but the current operation requires an open peer connection.";
            break;
        case rtcResultU4::kSctpNotNegotiated:
            message = "The SCTP handshake for data channels encryption was not performed.";
            break;
        case rtcResultU4::kInvalidDataChannelId:
            message = "The specified data channel ID is invalid.";
            break;
        case rtcResultU4::kInvalidMediaKind:
            message = "Some audio-only function was called on a video-only object or vice-versa.";
            break;
        case rtcResultU4::kAudioResamplingNotSupported:
            message= "The internal audio resampler used in the audio track read buffer doesn't support the specified input/output frequency ratio. Use a different output frequency for the current audio source to solve the issue.";
            break;
        case rtcResultU4::kDummyAudioSourceNotSupported:
            message = "Dummy audio source does not support. To resolve the issue, use the PeerConnectionFactory initialize method with 'use_dummy_audio'.";
            break;
        case rtcResultU4::kInvalidPointer:
            message = "A null or invalid pointer.";
            break;
        default:
            snprintf(szTemp, sizeof(szTemp), "Undefined error: 0x%X", static_cast<unsigned int>(code));
            message = szTemp;
            break;
    }

    string strMessage(message);
    size_t len = strMessage.copy_to(pBuffer, static_cast<size_t>(sizeOfBuffer));

    return strMessage.size() > len
        ? rtcResultU4::kBufferTooSmall
        : rtcResultU4::kSuccess;
}
