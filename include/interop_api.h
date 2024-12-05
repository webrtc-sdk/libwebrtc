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

#define CHECK_POINTER_EX(p, r)      if ((p) == nullptr) { return (r); }
#define CHECK_POINTER(p)            CHECK_POINTER_EX((p), rtcResultU4::kInvalidPointer)
#define CHECK_OUT_POINTER(p)        if ((p) != nullptr) { *(p) = nullptr; } else { return rtcResultU4::kInvalidPointer; }
#define CHECK_NATIVE_HANDLE(h)      CHECK_POINTER_EX((h), rtcResultU4::kInvalidNativeHandle)

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
#endif

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
 * Add ref a pointer.
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RefCountedObject_AddRef(rtcRefCountedObjectHandle handle) noexcept;

/**
 * Release a pointer.
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RefCountedObject_Release(rtcRefCountedObjectHandle handle) noexcept;

/**
 * Initializes the RTCPeerConnectionFactor object.
 * If the LibWebRTC_Initialize method has been called;
 * do not use this method.
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_Initialize(
    rtcPeerConnectionFactoryHandle factory
) noexcept;

/**
 * Terminates the RTCPeerConnectionFactor object.
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_Terminate(
    rtcPeerConnectionFactoryHandle factory
) noexcept;

/**
 * Creates a new instance of the RTCPeerConnection object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_Create(
    rtcPeerConnectionFactoryHandle factory,
    const rtcPeerConnectionConfiguration* configuration,
    rtcMediaConstraintsHandle constraints,
    rtcPeerConnectionHandle* pRetVal) noexcept;

/**
 * Removes the RTCPeerConnection object from the factory.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_Delete(
    rtcPeerConnectionFactoryHandle factory,
    rtcPeerConnectionHandle handle
) noexcept;

/**
 * Provides RTCAudioDevice object for list audio devices.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetAudioDevice(
    rtcPeerConnectionFactoryHandle factory,
    rtcAudioDeviceHandle* pRetVal
) noexcept;

/**
 * Provides RTCVideoDevice object for list video devices.
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
 * Creates a new instance of the RTCAudioSource object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateAudioSource(
    rtcPeerConnectionFactoryHandle factory,
    const char* audio_source_label,
    rtcAudioSourceHandle* pRetVal
) noexcept;

/**
 * Creates a new instance of the RTCVideoSource object.
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
 * Creates a new instance of the RTCVideoSource object.
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
 * Creates a new instance of the RTCAudioTrack object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateAudioTrack(
    rtcPeerConnectionFactoryHandle factory,
    rtcAudioSourceHandle source,
    const char* track_id,
    rtcAudioTrackHandle* pRetVal
) noexcept;

/**
 * Creates a new instance of the RTCVideoTrack object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateVideoTrack(
    rtcPeerConnectionFactoryHandle factory,
    rtcVideoSourceHandle source,
    const char* track_id,
    rtcVideoTrackHandle* pRetVal
) noexcept;

/**
 * Creates a new instance of the RTCMediaStream object.
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

} // extern "C"

#endif  // LIB_WEBRTC_INTROP_API_HXX