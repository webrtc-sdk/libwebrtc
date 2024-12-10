#ifndef LIB_WEBRTC_INTROP_API_HXX
#define LIB_WEBRTC_INTROP_API_HXX

#ifdef LIB_WEBRTC_API_EXPORTS
#define LIB_WEBRTC_CALL __stdcall
#elif defined(LIB_WEBRTC_API_DLL)
#define LIB_WEBRTC_CALL __stdcall
#else
#define LIB_WEBRTC_CALL
#endif

#include "rtc_types.h"

#define CHECK_POINTER_EX(p, r)          if ((p) == nullptr) { return (r); }
#define CHECK_POINTER(p)                CHECK_POINTER_EX(p, rtcResultU4::kInvalidPointer)
#define CHECK_OUT_POINTER(p)            if ((p) != nullptr) { *(p) = nullptr; } else { return rtcResultU4::kInvalidPointer; }
#define CHECK_NATIVE_HANDLE(h)          CHECK_POINTER_EX(h, rtcResultU4::kInvalidNativeHandle)
#define ZERO_MEMORY(p,sz)               if ((sz) > 0) { memset((void*)(p), 0, (size_t)(sz)); }

extern "C" {

/// Types used from libwebrtc.
using IceTransportsType = libwebrtc::IceTransportsType;
using BundlePolicy = libwebrtc::BundlePolicy;
using RtcpMuxPolicy = libwebrtc::RtcpMuxPolicy;
using CandidateNetworkPolicy = libwebrtc::CandidateNetworkPolicy;
using TcpCandidatePolicy = libwebrtc::TcpCandidatePolicy;
using MediaSecurityType = libwebrtc::MediaSecurityType;
using SdpSemantics = libwebrtc::SdpSemantics;
using RTCMediaType = libwebrtc::RTCMediaType;

/// 32-bit boolean for interop API.
enum class rtcBool32 : int { kTrue = -1, kFalse = 0 };

/// 32-bit result enumerator
enum class rtcResultU4 : unsigned int {
    /// The operation was successful.
    kSuccess = 0,

    //
    // Generic errors
    //

    /// Unknown internal error.
    /// This is generally the fallback value when no other error code applies.
    kUnknownError = 0x80000000,

    /// A parameter passed to the API function was invalid.
    kInvalidParameter = 0x80000001,

    /// The operation cannot be performed in the current state.
    kInvalidOperation = 0x80000002,

    /// A call was made to an API function on the wrong thread.
    /// This is generally related to platforms with thread affinity like UWP.
    kWrongThread = 0x80000003,

    /// An object was not found.
    kNotFound = 0x80000004,

    /// An interop handle referencing a native object instance is invalid,
    /// although the API function was expecting a valid object.
    kInvalidNativeHandle = 0x80000005,

    /// The API object is not initialized, and cannot as a result perform the
    /// given operation.
    kNotInitialized = 0x80000006,

    /// The current operation is not supported by the implementation.
    kUnsupported = 0x80000007,

    /// An argument was passed to the API function with a value out of the
    /// expected range.
    kOutOfRange = 0x80000008,

    /// The buffer provided by the caller was too small for the operation to
    /// complete successfully.
    kBufferTooSmall = 0x80000009,

    //
    // Peer connection (0x1xx)
    //

    /// The peer connection is closed, but the current operation requires an open
    /// peer connection.
    kPeerConnectionClosed = 0x80000101,

    //
    // Data (0x3xx)
    //

    /// The SCTP handshake for data channels encryption was not performed, because
    /// the connection was established before any data channel was added to it.
    /// Due to limitations in the implementation, without SCTP handshake data
    /// channels cannot be used, and therefor applications expecting to use data
    /// channels must open at least a single channel before establishing a peer
    /// connection (calling |CreateOffer()|).
    kSctpNotNegotiated = 0x80000301,

    /// The specified data channel ID is invalid.
    kInvalidDataChannelId = 0x80000302,

    //
    // Media (0x4xx)
    //

    /// Some audio-only function was called on a video-only object or vice-versa.
    /// For example, trying to get the local audio track of a video transceiver.
    kInvalidMediaKind = 0x80000401,

    /// The internal audio resampler used in the audio track read buffer doesn't
    /// support the specified input/output frequency ratio. Use a different output
    /// frequency for the current audio source to solve the issue.
    kAudioResamplingNotSupported = 0x80000402,

    /// Error rtcResultU4 for a null or invalid pointer.
    kInvalidPointer = 0x80004003,
}; // end enum class rtcResultU4

struct rtcIceServer {
    const char* uri = nullptr;
    const char* username = nullptr;
    const char* password = nullptr;
};

struct rtcPeerConnectionConfiguration {
    rtcIceServer ice_servers[libwebrtc::kMaxIceServerSize];
    IceTransportsType type = IceTransportsType::kAll;
    BundlePolicy bundle_policy = BundlePolicy::kBundlePolicyBalanced;
    RtcpMuxPolicy rtcp_mux_policy = RtcpMuxPolicy::kRtcpMuxPolicyRequire;
    CandidateNetworkPolicy candidate_network_policy =
        CandidateNetworkPolicy::kCandidateNetworkPolicyAll;
    TcpCandidatePolicy tcp_candidate_policy =
        TcpCandidatePolicy::kTcpCandidatePolicyEnabled;

    int ice_candidate_pool_size = 0;

    MediaSecurityType srtp_type = MediaSecurityType::kDTLS_SRTP;
    SdpSemantics sdp_semantics = SdpSemantics::kUnifiedPlan;
    rtcBool32 offer_to_receive_audio = rtcBool32::kTrue;
    rtcBool32 offer_to_receive_video = rtcBool32::kTrue;

    rtcBool32 disable_ipv6 = rtcBool32::kFalse;
    rtcBool32 disable_ipv6_on_wifi = rtcBool32::kFalse;
    int max_ipv6_networks = 5;
    rtcBool32 disable_link_local_networks = rtcBool32::kFalse;
    int screencast_min_bitrate = -1;

    // private
    rtcBool32 use_rtp_mux = rtcBool32::kTrue;
    uint32_t local_audio_bandwidth = 128;
    uint32_t local_video_bandwidth = 512;
};

/// Opaque handle to a native interop object.
using rtcObjectHandle = void*;

/// Opaque handle to a native reference-counted interop object.
using rtcRefCountedObjectHandle = rtcObjectHandle;

/// Opaque handle to a native RTCPeerConnectionFactory interop object.
using rtcPeerConnectionFactoryHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCPeerConnection interop object.
using rtcPeerConnectionHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCAudioDevice interop object.
using rtcAudioDeviceHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCVideoDevice interop object.
using rtcVideoDeviceHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCAudioSource interop object.
using rtcAudioSourceHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCVideoSource interop object.
using rtcVideoSourceHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCVideoCapturer interop object.
using rtcVideoCapturerHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCMediaConstraints interop object.
using rtcMediaConstraintsHandle = rtcRefCountedObjectHandle;

#ifdef RTC_DESKTOP_DEVICE
/// Opaque handle to a native RTCDesktopDevice interop object.
using rtcDesktopDeviceHandle = rtcRefCountedObjectHandle;
/// Opaque handle to a native RTCDesktopCapturer interop object.
using rtcDesktopCapturerHandle = rtcRefCountedObjectHandle;
#endif // RTC_DESKTOP_DEVICE

/// Opaque handle to a native RTCMediaStream interop object.
using rtcMediaStreamHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpCapabilities interop object.
using rtcRtpCapabilitiesHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCMediaTrack interop object.
using rtcMediaTrackHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCAudioTrack interop object.
using rtcAudioTrackHandle = rtcMediaTrackHandle;

/// Opaque handle to a native RTCVideoTrack interop object.
using rtcVideoTrackHandle = rtcMediaTrackHandle;


/*
 * ---------------------------------------------------------------------- 
 * LibWebRTC interop methods
 * ---------------------------------------------------------------------- 
 */

/**
 * @brief Initializes the WebRTC PeerConnectionFactory and threads.
 *
 * Initializes the WebRTC PeerConnectionFactory and threads. This method is
 * thread-safe and can be called from any thread. It initializes SSL and
 * creates three threads: worker_thread, signaling_thread and network_thread.
 *
 * @return kTrue if initialization is successful, kFalse otherwise.
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
LibWebRTC_Initialize() noexcept;

/**
 * @brief Creates a new WebRTC PeerConnectionFactory.
 *
 * Creates a new WebRTC PeerConnectionFactory. This method is thread-safe and
 * can be called from any thread. It creates a new instance of the
 * RTCPeerConnectionFactoryImpl class and initializes it.
 *
 * @return A scoped_refptr object that points to the newly created
 * RTCPeerConnectionFactory.
 */
LIB_WEBRTC_API rtcPeerConnectionFactoryHandle LIB_WEBRTC_CALL
LibWebRTC_CreateRTCPeerConnectionFactory() noexcept;

/**
 * @brief Terminates the WebRTC PeerConnectionFactory and threads.
 *
 * Terminates the WebRTC PeerConnectionFactory and threads. This method is
 * thread-safe and can be called from any thread. It cleans up SSL and stops
 * and destroys the three threads: worker_thread, signaling_thread and
 * network_thread.
 *
 */
LIB_WEBRTC_API void LIB_WEBRTC_CALL
LibWebRTC_Terminate() noexcept;

/**
 * @brief Returns the error message text for the specified 'code' value.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
LibWebRTC_GetErrorMessage(
    rtcResultU4 code,
    char* pBuffer,
    unsigned int cchBuffer
) noexcept;


/*
 * ---------------------------------------------------------------------- 
 * RefCountedObject interop methods
 * ---------------------------------------------------------------------- 
 */

/**
 * @brief Add ref a pointer.
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RefCountedObject_AddRef(rtcRefCountedObjectHandle handle) noexcept;

/**
 * @brief Release a pointer.
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RefCountedObject_Release(rtcRefCountedObjectHandle handle) noexcept;


/*
 * ---------------------------------------------------------------------- 
 * MediaConstraints interop methods
 * ---------------------------------------------------------------------- 
 */

/**
 * @brief Creates a new instance of the RTCMediaConstraints object.
 */
LIB_WEBRTC_API rtcMediaConstraintsHandle LIB_WEBRTC_CALL
MediaConstraints_Create() noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
MediaConstraints_AddMandatoryConstraint(
    rtcMediaConstraintsHandle handle,
    const char* key,
    const char* value) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
MediaConstraints_AddOptionalConstraint(
    rtcMediaConstraintsHandle handle,
    const char* key,
    const char* value) noexcept;


/*
 * ---------------------------------------------------------------------- 
 * RTCPeerConnectionFactory interop methods
 * ---------------------------------------------------------------------- 
 */

/**
 * @brief Creates a new instance of the RTCPeerConnectionFactor object.
 */
LIB_WEBRTC_API rtcPeerConnectionFactoryHandle LIB_WEBRTC_CALL
RTCPeerConnectionFactory_Create() noexcept;

/**
 * @brief Initializes the RTCPeerConnectionFactor object.
 * If the LibWebRTC_Initialize method has been called;
 * do not use this method.
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_Initialize(
    rtcPeerConnectionFactoryHandle factory
) noexcept;

/**
 * @brief Terminates the RTCPeerConnectionFactor object.
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_Terminate(
    rtcPeerConnectionFactoryHandle factory
) noexcept;

/**
 * @brief Creates a new instance of the RTCPeerConnection object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreatePeerConnection(
    rtcPeerConnectionFactoryHandle factory,
    const rtcPeerConnectionConfiguration* configuration,
    rtcMediaConstraintsHandle constraints,
    rtcPeerConnectionHandle* pRetVal) noexcept;

/**
 * @brief Removes the RTCPeerConnection object from the factory.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_DeletePeerConnection(
    rtcPeerConnectionFactoryHandle factory,
    rtcPeerConnectionHandle handle
) noexcept;

/**
 * @brief Provides RTCAudioDevice object for list audio devices.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetAudioDevice(
    rtcPeerConnectionFactoryHandle factory,
    rtcAudioDeviceHandle* pRetVal
) noexcept;

/**
 * @brief Provides RTCVideoDevice object for list video devices.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetVideoDevice(
    rtcPeerConnectionFactoryHandle factory,
    rtcVideoDeviceHandle* pRetVal
) noexcept;

#ifdef RTC_DESKTOP_DEVICE
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetDesktopDevice(
    rtcPeerConnectionFactoryHandle factory,
    rtcDesktopDeviceHandle* pRetVal
) noexcept;
#endif // RTC_DESKTOP_DEVICE

/**
 * @brief Creates a new instance of the RTCAudioSource object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateAudioSource(
    rtcPeerConnectionFactoryHandle factory,
    const char* audio_source_label,
    rtcAudioSourceHandle* pRetVal
) noexcept;

/**
 * @brief Creates a new instance of the RTCVideoSource object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateVideoSource(
    rtcPeerConnectionFactoryHandle factory,
    rtcVideoCapturerHandle capturer,
    const char* video_source_label,
    rtcMediaConstraintsHandle constraints,
    rtcVideoSourceHandle* pRetVal
) noexcept;

#ifdef RTC_DESKTOP_DEVICE
/**
 * @brief Creates a new instance of the RTCVideoSource object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateDesktopSource(
    rtcPeerConnectionFactoryHandle factory,
    rtcDesktopCapturerHandle capturer,
    const char* video_source_label,
    rtcMediaConstraintsHandle constraints,
    rtcVideoSourceHandle* pRetVal
) noexcept;
#endif // RTC_DESKTOP_DEVICE

/**
 * @brief Creates a new instance of the RTCAudioTrack object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateAudioTrack(
    rtcPeerConnectionFactoryHandle factory,
    rtcAudioSourceHandle source,
    const char* track_id,
    rtcAudioTrackHandle* pRetVal
) noexcept;

/**
 * @brief Creates a new instance of the RTCVideoTrack object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateVideoTrack(
    rtcPeerConnectionFactoryHandle factory,
    rtcVideoSourceHandle source,
    const char* track_id,
    rtcVideoTrackHandle* pRetVal
) noexcept;

/**
 * @brief Creates a new instance of the RTCMediaStream object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateStream(
    rtcPeerConnectionFactoryHandle factory,
    const char* stream_id,
    rtcMediaStreamHandle* pRetVal
) noexcept;

/**
 * Returns an RTCRtpCapabilities object containing the
 * sender's capabilities.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetRtpSenderCapabilities(
    rtcPeerConnectionFactoryHandle factory,
    RTCMediaType media_type,
    rtcRtpCapabilitiesHandle* pRetVal
) noexcept;

/**
 * Returns an RTCRtpCapabilities object containing the
 * receiver's capabilities.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetRtpReceiverCapabilities(
    rtcPeerConnectionFactoryHandle factory,
    RTCMediaType media_type,
    rtcRtpCapabilitiesHandle* pRetVal
) noexcept;

/*
 * ---------------------------------------------------------------------- 
 * RTCAudioDevice interop methods
 * ---------------------------------------------------------------------- 
 */

/**
 * Audio device change callback delegate
 */
using rtcRTCAudioDeviceChangeCallback = void(LIB_WEBRTC_CALL*)();

/**
 * Returns the number of playout devices available.
 *
 * @param audiDevice - Audio device handle
 * @return int - The number of playout devices available.
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCAudioDevice_PlayoutDevices(
    rtcAudioDeviceHandle audiDevice
) noexcept;

/**
 * Returns the number of recording devices available.
 *
 * @param audiDevice - Audio device handle
 * @return int - The number of recording devices available.
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCAudioDevice_RecordingDevices(
    rtcAudioDeviceHandle audiDevice
) noexcept;

/**
 * Retrieves the name and GUID of the specified playout device.
 *
 * @param audiDevice - Audio device handle
 * @param index - The index of the device.
 * @param pOutName - The device name.
 * @param cchOutName - The size of the name.
 * @param pOutGuid - The device GUID.
 * @param cchOutGuid - The size of the guid.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_PlayoutDeviceName(
    rtcAudioDeviceHandle audiDevice,
    int index,
    char* pOutName,
    int cchOutName,
    char* pOutGuid,
    int cchOutGuid
) noexcept;

/**
 * Retrieves the name and GUID of the specified recording device.
 *
 * @param audiDevice - Audio device handle
 * @param index - The index of the device.
 * @param pOutName - The device name.
 * @param cchOutName - The size of the name.
 * @param pOutGuid - The device GUID.
 * @param cchOutGuid - The size of the guid.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_RecordingDeviceName(
    rtcAudioDeviceHandle audiDevice,
    int index,
    char* pOutName,
    int cchOutName,
    char* pOutGuid,
    int cchOutGuid
) noexcept;

/**
 * Sets the playout device to use.
 *
 * @param audiDevice - Audio device handle
 * @param index - The index of the device.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_SetPlayoutDevice(
    rtcAudioDeviceHandle audiDevice,
    int index
) noexcept;

/**
 * Sets the recording device to use.
 *
 * @param audiDevice - Audio device handle
 * @param index - The index of the device.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_SetRecordingDevice(
    rtcAudioDeviceHandle audiDevice,
    int index
) noexcept;

/**
 * The callback was invoked when the audio device changed.
 * 
 * @param audiDevice - Audio device handle
 * @param deviceChangeCallback - Callback delegate
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_RegisterDeviceChangeCallback(
    rtcAudioDeviceHandle audiDevice,
    rtcRTCAudioDeviceChangeCallback deviceChangeCallback
) noexcept;

/**
 * Sets the microphone volume level.
 * 
 * @param audiDevice - Audio device handle
 * @param volume - Volume level
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_SetMicrophoneVolume(
    rtcAudioDeviceHandle audiDevice,
    unsigned int volume
) noexcept;

/**
 * Gets the microphone volume level.
 * 
 * @param audiDevice - Audio device handle
 * @param volume - Volume level
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_GetMicrophoneVolume(
    rtcAudioDeviceHandle audiDevice,
    unsigned int* volume
) noexcept;

/**
 * Sets the speaker volume level.
 * 
 * @param audiDevice - Audio device handle
 * @param volume - Volume level
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_SetSpeakerVolume(
    rtcAudioDeviceHandle audiDevice,
    unsigned int volume
) noexcept;

/**
 * Gets the speaker volume level.
 * 
 * @param audiDevice - Audio device handle
 * @param volume - Volume level
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_GetSpeakerVolume(
    rtcAudioDeviceHandle audiDevice,
    unsigned int* volume
) noexcept;

/*
 * ---------------------------------------------------------------------- 
 * RTCVideoDevice interop methods
 * ---------------------------------------------------------------------- 
 */

/**
 * Returns the number of video devices available.
 *
 * @param videoDevice - Video device handle
 * @return int - The number of video devices available.
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCVideoDevice_NumberOfDevices(
    rtcVideoDeviceHandle videoDevice
) noexcept;

/**
 * Returns information about video device with the specified index.
 * 
 * @param videoDevice - Video device handle
 * @param index - The index of the device.
 * @param pOutDeviceNameUTF8 - Device name
 * @param cchOutDeviceNameUTF8 - The size of the name.
 * @param pOutDeviceUniqueIdUTF8 - Device id
 * @param cchOutDeviceUniqueIdUTF8 - The size of the id.
 * @param pOutProductUniqueIdUTF8 - Device product id
 * @param cchOutProductUniqueIdUTF8 - The size of the product id.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCVideoDevice_GetDeviceName(
    rtcVideoDeviceHandle videoDevice,
    int index,
    char* pOutDeviceNameUTF8,
    int cchOutDeviceNameUTF8,
    char* pOutDeviceUniqueIdUTF8,
    int cchOutDeviceUniqueIdUTF8,
    char* pOutProductUniqueIdUTF8 = 0,
    int cchOutProductUniqueIdUTF8 = 0
) noexcept;

/**
 * Creates a new instance of the RTCVideoCapturer object.
 * 
 * @param videoDevice - Video device handle
 * @param name - Video capturer name (for logs)
 * @param index - The index of the video device.
 * @param width - Video frame width
 * @param height - Video frame height
 * @param target_fps - Target frame per second
 * @param pOutRetVal - Handle for the RTCVideoCapturer object to be created.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCVideoDevice_CreateVideoCapturer(
    rtcVideoDeviceHandle videoDevice,
    const char* name,
    int index,
    int width,
    int height,
    int target_fps,
    rtcVideoCapturerHandle* pOutRetVal
) noexcept;

/*
 * ---------------------------------------------------------------------- 
 * RTCVideoCapturer interop methods
 * ---------------------------------------------------------------------- 
 */

/**
 * Starts video capturing.
 * 
 * @param videoCapturer - Video capturer handle
 * @return rtcBool32 - rtcBool32::kTrue if successful, otherwise rtcBool32::kFalse
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
RTCVideoCapturer_StartCapture(
    rtcVideoCapturerHandle videoCapturer
) noexcept;

/**
 * Returns whether the capture state has been initialized.
 * 
 * @param videoCapturer - Video capturer handle
 * @return rtcBool32 - rtcBool32::kTrue if started, otherwise rtcBool32::kFalse
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
RTCVideoCapturer_CaptureStarted(
    rtcVideoCapturerHandle videoCapturer
) noexcept;

/**
 * Stops video capture.
 * 
 * @param videoCapturer - Video capturer handle
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCVideoCapturer_StopCapture(
    rtcVideoCapturerHandle videoCapturer
) noexcept;

} // extern "C"

#endif  // LIB_WEBRTC_INTROP_API_HXX