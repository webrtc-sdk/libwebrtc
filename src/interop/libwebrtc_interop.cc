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

rtcPeerConnectionFactoryHandle LIB_WEBRTC_CALL LibWebRTC_CreateRTCPeerConnectionFactory() noexcept
{
    scoped_refptr<RTCPeerConnectionFactory> p = LibWebRTC::CreateRTCPeerConnectionFactory();
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
    unsigned int cchBuffer
) noexcept
{
    CHECK_POINTER(pBuffer);
    if (cchBuffer == 0) {
        return rtcResultU4::kBufferTooSmall;
    }
    memset(pBuffer, 0x0, cchBuffer);

    char szBuffer[256] = {0};
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
        case rtcResultU4::kInvalidPointer:
            message = "A null or invalid pointer.";
            break;
        default:
            snprintf(szBuffer, sizeof(szBuffer), "Undefined error: 0x%X", static_cast<unsigned int>(code));
            message = (const char*)szBuffer;
            break;
    }

    unsigned int cchMessage = (unsigned int)strlen(message);
    unsigned int cchLen = std::min(cchMessage, cchBuffer);
    strncpy(pBuffer, (const char*)message, (size_t)cchLen);

    return cchMessage > cchBuffer
        ? rtcResultU4::kBufferTooSmall
        : rtcResultU4::kSuccess;
}
