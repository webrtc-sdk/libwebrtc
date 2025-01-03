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
#include "interop_macros.h"

extern "C" {

/// Types used from libwebrtc.
using rtcIceTransportsType = libwebrtc::IceTransportsType;
using rtcBundlePolicy = libwebrtc::BundlePolicy;
using rtcRtcpMuxPolicy = libwebrtc::RtcpMuxPolicy;
using rtcCandidateNetworkPolicy = libwebrtc::CandidateNetworkPolicy;
using rtcTcpCandidatePolicy = libwebrtc::TcpCandidatePolicy;
using rtcMediaSecurityType = libwebrtc::MediaSecurityType;
using rtcSdpSemantics = libwebrtc::SdpSemantics;
using rtcMediaType = libwebrtc::RTCMediaType;
using rtcDesktopType = libwebrtc::DesktopType;

/// 64-bit timestamp for interop API.
using rtcTimestamp = long long;

/// 32-bit boolean for interop API.
enum class rtcBool32 : int { kFalse = 0, kTrue = 1 };

enum class rtcKeyRingSize : int { kMin = 1, kDefault = 16, kMax = 256 };

enum class rtcTrackState : int { kUnknown = -1, kLive = 0, kEnded = 1 };

enum class rtcVideoFrameType : int {
  kUnknown = -1,
  kARGB = 0,
  kBGRA = 1,
  kABGR = 2,
  kRGBA = 3
};

enum class rtcVideoRotation : int {
  kVideoRotation_0 = 0,
  kVideoRotation_90 = 90,
  kVideoRotation_180 = 180,
  kVideoRotation_270 = 270
};

enum class rtcDataChannelState : int {
  kConnecting = 0,
  kOpen = 1,
  kClosing = 2,
  kClosed = 3
};

enum class rtcDataChannelProtocol : int { kSCTP = 0, kQUIC = 1 };

enum class rtcDtlsTransportState : int {
  kNew = 0,         // Has not started negotiating yet.
  kConnecting = 1,  // In the process of negotiating a secure connection.
  kConnected = 2,   // Completed negotiation and verified fingerprints.
  kClosed = 3,      // Intentionally closed.
  kFailed = 4,      // Failure due to an error or failing to verify a remote
  // fingerprint.
  kNumValues = 5
};

enum class rtcFrameCryptionAlgorithm : int { kAesGcm = 0, kAesCbc = 1 };

enum class rtcFrameCryptionState : int {
  kNew = 0,
  kOk = 1,
  kEncryptionFailed = 2,
  kDecryptionFailed = 3,
  kMissingKey = 4,
  kKeyRatcheted = 5,
  kInternalError = 6
};

enum class rtcSessionDescriptionErrorType : int {
  kPeerConnectionInitFailed = 0,
  kCreatePeerConnectionFailed = 1,
  kSDPParseFailed = 2
};

enum class rtcPeerConnectionState : int {
  kNew = 0,
  kConnecting = 1,
  kConnected = 2,
  kDisconnected = 3,
  kFailed = 4,
  kClosed = 5
};

enum class rtcSignalingState : int {
  kStable = 0,
  kHaveLocalOffer = 1,
  kHaveRemoteOffer = 2,
  kHaveLocalPrAnswer = 3,
  kHaveRemotePrAnswer = 4,
  kClosed = 5
};

enum class rtcIceGatheringState : int {
  kNew = 0,
  kGathering = 1,
  kComplete = 2
};

enum class rtcIceConnectionState : int {
  kNew = 0,
  kChecking = 1,
  kCompleted = 2,
  kConnected = 3,
  kFailed = 4,
  kDisconnected = 5,
  kClosed = 6,
  kMax = 7
};

enum class rtcStatsMemberType : int {
  kBool = 0,    // bool
  kInt32 = 1,   // int32_t
  kUint32 = 2,  // uint32_t
  kInt64 = 3,   // int64_t
  kUint64 = 4,  // uint64_t
  kDouble = 5,  // double
  kString = 6,  // std::string

  kSequenceBool = 7,     // std::vector<bool>
  kSequenceInt32 = 8,    // std::vector<int32_t>
  kSequenceUint32 = 9,   // std::vector<uint32_t>
  kSequenceInt64 = 10,   // std::vector<int64_t>
  kSequenceUint64 = 11,  // std::vector<uint64_t>
  kSequenceDouble = 12,  // std::vector<double>
  kSequenceString = 13,  // std::vector<std::string>

  kMapStringUint64 = 14,  // std::map<std::string, uint64_t>
  kMapStringDouble = 15   // std::map<std::string, double>
};

enum class rtcRtpTransceiverDirection : int {
  kSendRecv = 0,
  kSendOnly = 1,
  kRecvOnly = 2,
  kInactive = 3,
  kStopped = 4
};

enum class rtcFecMechanism : int {
  kRED = 0,
  kRED_AND_ULPFEC = 1,
  kFLEXFEC = 2
};

enum class rtcRtcpFeedbackType : int {
  kCCM = 0,
  kLNTF = 1,
  kNACK = 2,
  kREMB = 3,
  kTRANSPORT_CC = 4
};

enum class rtcRtcpFeedbackMessageType : int {
  kGENERIC_NACK = 0,
  kPLI = 1,
  kFIR = 2
};

enum class rtcDtxStatus : int { kDISABLED = 0, kENABLED = 1 };

enum class rtcDegradationPreference : int {
  kDISABLED = 0,
  kMAINTAIN_FRAMERATE = 1,
  kMAINTAIN_RESOLUTION = 2,
  kBALANCED = 3
};

enum class rtcNetworkPriority : int {
  kVeryLow = 0,
  kLow = 1,
  kMedium = 2,
  kHigh = 3
};

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
};  // end enum class rtcResultU4

struct rtcStringPair {
  const char* key = nullptr;
  const char* value = nullptr;
}; // end struct rtcStringPair

struct rtcIceServer {
  const char* uri = nullptr;
  const char* username = nullptr;
  const char* password = nullptr;
};  // end struct rtcIceServer

struct rtcPeerConnectionConfiguration {
  rtcIceServer ice_servers[libwebrtc::kMaxIceServerSize];
  rtcIceTransportsType type = rtcIceTransportsType::kAll;
  rtcBundlePolicy bundle_policy = rtcBundlePolicy::kBundlePolicyBalanced;
  rtcRtcpMuxPolicy rtcp_mux_policy = rtcRtcpMuxPolicy::kRtcpMuxPolicyRequire;
  rtcCandidateNetworkPolicy candidate_network_policy =
      rtcCandidateNetworkPolicy::kCandidateNetworkPolicyAll;
  rtcTcpCandidatePolicy tcp_candidate_policy =
      rtcTcpCandidatePolicy::kTcpCandidatePolicyEnabled;

  int ice_candidate_pool_size = 0;

  rtcMediaSecurityType srtp_type = rtcMediaSecurityType::kDTLS_SRTP;
  rtcSdpSemantics sdp_semantics = rtcSdpSemantics::kUnifiedPlan;
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
};  // end struct rtcPeerConnectionConfiguration

struct rtcVideoFrameDatas {
  // frame width in pixel
  int width = 0;
  // frame height in pixel
  int height = 0;
  // The YUV starting address of the frame buffer.
  const unsigned char* data_y = nullptr;
  const unsigned char* data_u = nullptr;
  const unsigned char* data_v = nullptr;
  // The stride of the YUV
  int stride_y = 0;
  int stride_u = 0;
  int stride_v = 0;
};  // end struct rtcVideoFrameDatas

struct rtcDataChannelInit {
  rtcBool32 ordered = rtcBool32::kTrue;
  rtcBool32 reliable = rtcBool32::kTrue;
  int maxRetransmitTime = -1;
  int maxRetransmits = -1;
  rtcDataChannelProtocol protocol =
      rtcDataChannelProtocol::kSCTP;  // kSCTP | kQUIC
  rtcBool32 negotiated = rtcBool32::kFalse;
  int id = 0;
};  // end struct rtcDataChannelInit

struct rtcKeyProviderOptions {
  rtcBool32 shared_key = rtcBool32::kFalse;
  unsigned char* ratchet_salt = nullptr;
  int ratchet_salt_len = 0;
  unsigned char* uncrypted_magic_bytes = nullptr;
  int uncrypted_magic_bytes_len = 0;
  int ratchet_window_size = 0;
  int failure_tolerance = -1;
  // The size of the key ring. between 1 and 255.
  rtcKeyRingSize key_ring_size = rtcKeyRingSize::kDefault;
  rtcBool32 discard_frame_when_cryptor_not_ready = rtcBool32::kFalse;
};  // end struct rtcKeyProviderOptions

/// Opaque handle to a native interop object.
using rtcObjectHandle = void*;

/// Opaque handle to a native reference-counted interop object.
using rtcRefCountedObjectHandle = rtcObjectHandle;

/// Opaque handle to a native RTCPeerConnectionFactory interop object.
using rtcPeerConnectionFactoryHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCStatsMemberList interop object.
using rtcStatsMemberListHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native MediaRTCStatsList interop object.
using rtcMediaRTCStatsListHandle = rtcRefCountedObjectHandle;

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

/// Opaque handle to a native RTCDesktopDevice interop object.
using rtcDesktopDeviceHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCDesktopCapturer interop object.
using rtcDesktopCapturerHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native MediaSource interop object.
using rtcDesktopMediaSourceHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCDesktopMediaList interop object.
using rtcDesktopMediaListHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCMediaStream interop object.
using rtcMediaStreamHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCMediaStreamList interop object.
using rtcMediaStreamListHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCMediaTrack interop object.
using rtcMediaTrackHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCAudioTrack interop object.
using rtcAudioTrackHandle = rtcMediaTrackHandle;

/// Opaque handle to a native RTCVideoTrack interop object.
using rtcVideoTrackHandle = rtcMediaTrackHandle;

/// Opaque handle to a native RTCVideoFrame interop object.
using rtcVideoFrameHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCVideoRenderer interop object.
using rtcVideoRendererHandle = rtcRefCountedObjectHandle;  // ???

/// Opaque handle to a native RTCDataChannel interop object.
using rtcDataChannelHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCDtlsTransportInformation interop object.
using rtcDtlsTransportInformationHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCDtlsTransport interop object.
using rtcDtlsTransportHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCDtmfSender interop object.
using rtcDtmfSenderHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native KeyProvider interop object.
using rtcKeyProviderHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCFrameCryptorObserver interop object.
using rtcFrameCryptorObserverHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCFrameCryptor interop object.
using rtcFrameCryptorHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCSdpParseError interop object.
using rtcSdpParseErrorHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCIceCandidate interop object.
using rtcIceCandidateHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native IceTransport interop object.
using rtcIceTransportHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCStatsMember interop object.
using rtcStatsMemberHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native MediaRTCStats interop object.
using rtcMediaRTCStatsHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpCodecCapability interop object.
using rtcRtpCodecCapabilityHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpHeaderExtensionCapability interop object.
using rtcRtpHeaderExtensionCapabilityHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpCodecCapabilityList interop object.
using rtcRtpCodecCapabilityListHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpHeaderExtensionListCapability interop object.
using rtcRtpHeaderExtensionCapabilityListHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpCapabilities interop object.
using rtcRtpCapabilitiesHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtcpFeedback interop object.
using rtcRtcpFeedbackHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtcpFeedbackList interop object.
using rtcRtcpFeedbackListHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpExtension interop object.
using rtcRtpExtensionHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpExtensionList interop object.
using rtcRtpExtensionListHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RtpFecParameters interop object.
using rtcRtpFecParametersHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpCodecParameters interop object.
using rtcRtpCodecParametersHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpCodecParametersPair interop object.
using rtcRtpCodecParametersPairHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpCodecParametersMap interop object.
using rtcRtpCodecParametersMapHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpCodecParametersList interop object.
using rtcRtpCodecParametersListHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtcpParameters interop object.
using rtcRtcpParametersHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtcpParametersList interop object.
using rtcRtcpParametersListHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpEncodingParameters interop object.
using rtcRtpEncodingParametersHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpEncodingParametersList interop object.
using rtcRtpEncodingParametersListHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpParameters interop object.
using rtcRtpParametersHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpReceiver interop object.
using rtcRtpReceiverHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpReceiverList interop object.
using rtcRtpReceiverListHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpSender interop object.
using rtcRtpSenderHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpSenderList interop object.
using rtcRtpSenderListHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpTransceiverInit interop object.
using rtcRtpTransceiverInitHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpTransceiver interop object.
using rtcRtpTransceiverHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCRtpTransceiverList interop object.
using rtcRtpTransceiverListHandle = rtcRefCountedObjectHandle;

/// Opaque handle to a native RTCSessionDescription interop object.
using rtcSessionDescriptionHandle = rtcRefCountedObjectHandle;

/* ---------------------------------------------------------------- */

/**
 * Audio device change callback delegate
 */
using rtcAudioDeviceChangeDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data);

/**
 * Callback delegate structure for RTCAudioDevice.
 */
struct rtcAudioDeviceCallbacks {
  rtcObjectHandle UserData{};
  rtcAudioDeviceChangeDelegate DeviceChanged{};
};

/**
 * Callback delegate for MediaListObserver.
 */
using rtcMediaListObserverDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcDesktopMediaSourceHandle source);

/**
 * Callback delegate structure for MediaListObserver.
 */
struct rtcMediaListObserverCallbacks {
  rtcObjectHandle UserData{};
  rtcMediaListObserverDelegate MediaSourceAdded{};
  rtcMediaListObserverDelegate MediaSourceRemoved{};
  rtcMediaListObserverDelegate MediaSourceNameChanged{};
  rtcMediaListObserverDelegate MediaSourceThumbnailChanged{};
};

/**
 * Callback OnFrame delegate for RTCVideoRenderer.
 */
using rtcVideoRendererFrameDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcVideoFrameHandle frame);

/**
 * Callback OnStateChange delegate for RTCDataChannelObserver.
 */
using rtcDataChannelObserverStateChangeDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcDataChannelState state);

/**
 * Callback OnMessage delegate for RTCDataChannelObserver.
 */
using rtcDataChannelObserverMessageDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, const char* buffer, int length, rtcBool32 binary);

/**
 * Callback delegate structure for RTCDataChannelObserver.
 */
struct rtcDataChannelObserverCallbacks {
  rtcObjectHandle UserData{};
  rtcDataChannelObserverStateChangeDelegate StateChanged{};
  rtcDataChannelObserverMessageDelegate MessageReceived{};
};

/**
 * RTCRtpReceiver: first packet received callback delegate
 */
using rtcRtpReceiverFirstPacketReceivedDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcMediaType media_type);

/**
 * Callback delegate structure for RTCRtpReceiverObserver.
 */
struct rtcRtpReceiverObserverCallbacks {
  rtcObjectHandle UserData{};
  rtcRtpReceiverFirstPacketReceivedDelegate FirstPacketReceived{};
};

/**
 * RTCDtmfSender: Tone changed callback delegate
 */
using rtcDtmfSenderToneChangedDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, const char* tone, const char* tone_buffer);

/**
 * Callback delegate structure for RTCDtmfSenderObserver.
 */
struct rtcDtmfSenderObserverCallbacks {
  rtcObjectHandle UserData{};
  rtcDtmfSenderToneChangedDelegate ToneChanged{};
};

/**
 * RTCDtlsTransport: State changed callback delegate
 */
using rtcDtlsTransportStateChangedDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcDtlsTransportInformationHandle info);

/**
 * RTCDtlsTransport: Error callback delegate
 */
using rtcDtlsTransportErrorDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, const int type, const char* message);

/**
 * Callback delegate structure for RTCDtlsTransportObserver.
 */
struct rtcDtlsTransportObserverCallbacks {
  rtcObjectHandle UserData{};
  rtcDtlsTransportStateChangedDelegate StateChanged{};
  rtcDtlsTransportErrorDelegate Error{};
};

/**
 * RTCPeerConnectionObserverCallbacks
 */
using rtcSignalingStateChangedDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcSignalingState state);

using rtcConnectionStateChangedDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcPeerConnectionState state);

using rtcIceGatheringStateChangedDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcIceGatheringState state);

using rtcIceConnectionStateChangedDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcIceConnectionState state);

using rtcIceCandidateReadytoSendDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcIceCandidateHandle iceCandidate);

using rtcStreamAddedRemovedDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcMediaStreamHandle mediaStream);

using rtcDataChannelAddedDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcDataChannelHandle dataChannel);

using rtcRenegotiationNeededDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data);

using rtcTransceiverAddedDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcRtpTransceiverHandle rtpTransceiver);

using rtcTrackAddedDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcMediaStreamListHandle mediaStreamList, rtcRtpReceiverHandle rtpReceiver);

using rtcTrackRemovedDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcRtpReceiverHandle rtpReceiver);

/**
 * Callback delegate structure for RTCPeerConnectionObserver.
 */
struct rtcPeerConnectionObserverCallbacks {
  rtcObjectHandle UserData{};
  rtcSignalingStateChangedDelegate SignalingStateChanged{};
  rtcConnectionStateChangedDelegate ConnectionStateChanged{};
  rtcIceGatheringStateChangedDelegate IceGatheringStateChanged{};
  rtcIceConnectionStateChangedDelegate IceConnectionStateChanged{};
  rtcIceCandidateReadytoSendDelegate IceCandidateReadytoSend{};
  rtcStreamAddedRemovedDelegate StreamAdded{};
  rtcStreamAddedRemovedDelegate StreamRemoved{};        
  rtcDataChannelAddedDelegate DataChannelAdded{};
  rtcRenegotiationNeededDelegate RenegotiationNeeded{};        
  rtcTransceiverAddedDelegate TransceiverAdded{};
  rtcTrackAddedDelegate TrackAdded{};
  rtcTrackRemovedDelegate TrackRemoved{};
}; // end struct rtcPeerConnectionObserverCallbacks

/*-----------------------------------------------------------------*/

using rtcOnStatsCollectorSuccessDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, rtcMediaRTCStatsListHandle reports);

using rtcOnFailureDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, const char* error);

using rtcOnGetSdpSuccessDelegate = void(LIB_WEBRTC_CALL*)(
    rtcObjectHandle user_data, const char* sdp, const char* type);

using rtcOnSetSdpSuccessDelegate = void(LIB_WEBRTC_CALL*)(rtcObjectHandle user_data);

/**
 * Callback delegate structure for RTCPeerConnection.
 */
struct rtcOnStatsCallbacks {
  rtcObjectHandle UserData{};
  rtcOnStatsCollectorSuccessDelegate Success{};
  rtcOnFailureDelegate Failure{};
}; // end struct rtcOnStatsCallbacks

/**
 * Callback delegate structure for RTCPeerConnection.
 */
struct rtcOnGetSdpCallbacks {
  rtcObjectHandle UserData{};
  rtcOnGetSdpSuccessDelegate Success{};
  rtcOnFailureDelegate Failure{};
}; // end struct rtcOnGetSdpCallbacks

/**
 * Callback delegate structure for RTCPeerConnection.
 */
struct rtcOnSetSdpCallbacks {
  rtcObjectHandle UserData{};
  rtcOnSetSdpSuccessDelegate Success{};
  rtcOnFailureDelegate Failure{};
}; // end struct rtcOnSetSdpCallbacks

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
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL LibWebRTC_Initialize() noexcept;

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
LIB_WEBRTC_API void LIB_WEBRTC_CALL LibWebRTC_Terminate() noexcept;

/**
 * @brief Returns the error message text for the specified 'code' value.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL LibWebRTC_GetErrorMessage(
    rtcResultU4 code, char* pBuffer, unsigned int cchBuffer) noexcept;

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
MediaConstraints_AddMandatoryConstraint(rtcMediaConstraintsHandle handle,
                                        const char* key,
                                        const char* value) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
MediaConstraints_AddOptionalConstraint(rtcMediaConstraintsHandle handle,
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
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL RTCPeerConnectionFactory_Initialize(
    rtcPeerConnectionFactoryHandle factory) noexcept;

/**
 * @brief Terminates the RTCPeerConnectionFactor object.
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL RTCPeerConnectionFactory_Terminate(
    rtcPeerConnectionFactoryHandle factory) noexcept;

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
    rtcPeerConnectionHandle handle) noexcept;

/**
 * @brief Provides RTCAudioDevice object for list audio devices.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetAudioDevice(rtcPeerConnectionFactoryHandle factory,
                                        rtcAudioDeviceHandle* pRetVal) noexcept;

/**
 * @brief Provides RTCVideoDevice object for list video devices.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetVideoDevice(rtcPeerConnectionFactoryHandle factory,
                                        rtcVideoDeviceHandle* pRetVal) noexcept;

#ifdef RTC_DESKTOP_DEVICE
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetDesktopDevice(
    rtcPeerConnectionFactoryHandle factory,
    rtcDesktopDeviceHandle* pRetVal) noexcept;
#endif  // RTC_DESKTOP_DEVICE

/**
 * @brief Creates a new instance of the RTCAudioSource object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateAudioSource(
    rtcPeerConnectionFactoryHandle factory, const char* audio_source_label,
    rtcAudioSourceHandle* pRetVal) noexcept;

/**
 * @brief Creates a new instance of the RTCVideoSource object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateVideoSource(
    rtcPeerConnectionFactoryHandle factory, rtcVideoCapturerHandle capturer,
    const char* video_source_label, rtcMediaConstraintsHandle constraints,
    rtcVideoSourceHandle* pRetVal) noexcept;

#ifdef RTC_DESKTOP_DEVICE
/**
 * @brief Creates a new instance of the RTCVideoSource object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateDesktopSource(
    rtcPeerConnectionFactoryHandle factory, rtcDesktopCapturerHandle capturer,
    const char* video_source_label, rtcMediaConstraintsHandle constraints,
    rtcVideoSourceHandle* pRetVal) noexcept;
#endif  // RTC_DESKTOP_DEVICE

/**
 * @brief Creates a new instance of the RTCAudioTrack object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateAudioTrack(
    rtcPeerConnectionFactoryHandle factory, rtcAudioSourceHandle source,
    const char* track_id, rtcAudioTrackHandle* pRetVal) noexcept;

/**
 * @brief Creates a new instance of the RTCVideoTrack object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateVideoTrack(
    rtcPeerConnectionFactoryHandle factory, rtcVideoSourceHandle source,
    const char* track_id, rtcVideoTrackHandle* pRetVal) noexcept;

/**
 * @brief Creates a new instance of the RTCMediaStream object.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateStream(rtcPeerConnectionFactoryHandle factory,
                                      const char* stream_id,
                                      rtcMediaStreamHandle* pRetVal) noexcept;

/**
 * Returns an RTCRtpCapabilities object containing the
 * sender's capabilities.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetRtpSenderCapabilities(
    rtcPeerConnectionFactoryHandle factory, rtcMediaType media_type,
    rtcRtpCapabilitiesHandle* pRetVal) noexcept;

/**
 * Returns an RTCRtpCapabilities object containing the
 * receiver's capabilities.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetRtpReceiverCapabilities(
    rtcPeerConnectionFactoryHandle factory, rtcMediaType media_type,
    rtcRtpCapabilitiesHandle* pRetVal) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCAudioDevice interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Returns the number of playout devices available.
 *
 * @param audiDevice - Audio device handle
 * @return int - The number of playout devices available.
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCAudioDevice_PlayoutDevices(rtcAudioDeviceHandle audiDevice) noexcept;

/**
 * Returns the number of recording devices available.
 *
 * @param audiDevice - Audio device handle
 * @return int - The number of recording devices available.
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCAudioDevice_RecordingDevices(rtcAudioDeviceHandle audiDevice) noexcept;

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
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCAudioDevice_PlayoutDeviceName(
    rtcAudioDeviceHandle audiDevice, int index, char* pOutName, int cchOutName,
    char* pOutGuid, int cchOutGuid) noexcept;

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
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCAudioDevice_RecordingDeviceName(
    rtcAudioDeviceHandle audiDevice, int index, char* pOutName, int cchOutName,
    char* pOutGuid, int cchOutGuid) noexcept;

/**
 * Sets the playout device to use.
 *
 * @param audiDevice - Audio device handle
 * @param index - The index of the device.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCAudioDevice_SetPlayoutDevice(
    rtcAudioDeviceHandle audiDevice, int index) noexcept;

/**
 * Sets the recording device to use.
 *
 * @param audiDevice - Audio device handle
 * @param index - The index of the device.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCAudioDevice_SetRecordingDevice(
    rtcAudioDeviceHandle audiDevice, int index) noexcept;

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
    rtcAudioDeviceCallbacks* callbacks
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_UnregisterDeviceChangeCallback(
    rtcAudioDeviceHandle audiDevice
) noexcept;

/**
 * Sets the microphone volume level.
 *
 * @param audiDevice - Audio device handle
 * @param volume - Volume level
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCAudioDevice_SetMicrophoneVolume(
    rtcAudioDeviceHandle audiDevice, unsigned int volume) noexcept;

/**
 * Gets the microphone volume level.
 *
 * @param audiDevice - Audio device handle
 * @param volume - Volume level
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCAudioDevice_GetMicrophoneVolume(
    rtcAudioDeviceHandle audiDevice, unsigned int* volume) noexcept;

/**
 * Sets the speaker volume level.
 *
 * @param audiDevice - Audio device handle
 * @param volume - Volume level
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCAudioDevice_SetSpeakerVolume(
    rtcAudioDeviceHandle audiDevice, unsigned int volume) noexcept;

/**
 * Gets the speaker volume level.
 *
 * @param audiDevice - Audio device handle
 * @param volume - Volume level
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCAudioDevice_GetSpeakerVolume(
    rtcAudioDeviceHandle audiDevice, unsigned int* volume) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCMediaTrack interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Returns the track state.
 *
 * @param mediaTrack - Media track handle
 * @return rtcTrackState - Track state enum
 */
LIB_WEBRTC_API rtcTrackState LIB_WEBRTC_CALL
RTCMediaTrack_GetState(rtcMediaTrackHandle mediaTrack) noexcept;

/**
 * Returns the track kind. (video, audio, vs.)
 *
 * @param mediaTrack - Media track handle
 * @param pOutKind - Media track kind
 * @param cchOutKind - The size of the kind
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCMediaTrack_GetKind(
    rtcMediaTrackHandle mediaTrack, char* pOutKind, int cchOutKind) noexcept;

/**
 * Returns the track id.
 *
 * @param mediaTrack - Media track handle
 * @param pOutId - Media track id
 * @param cchOutId - The size of the id
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCMediaTrack_GetId(
    rtcMediaTrackHandle mediaTrack, char* pOutId, int cchOutId) noexcept;

/**
 * Returns the enable/disable status of the track.
 *
 * @param mediaTrack - Media track handle
 * @return rtcBool32 - kTrue if enabled, otherwise disabled
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
RTCMediaTrack_GetEnabled(rtcMediaTrackHandle mediaTrack) noexcept;

/**
 * Sets the enable/disable of the track.
 *
 * @param mediaTrack - Media track handle
 * @param enabled - Media track enable/disable value
 * @return rtcBool32 - kTrue if the property has changed, otherwise kFalse
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL RTCMediaTrack_SetEnabled(
    rtcMediaTrackHandle mediaTrack, rtcBool32 enabled) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCAudioTrack interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Sets the volume of the audio track.
 *
 * @param audioTrack - Audio track handle
 * @param volume - volume in [0-10]
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCAudioTrack_SetVolume(rtcAudioTrackHandle audioTrack, double volume) noexcept;

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
RTCVideoDevice_NumberOfDevices(rtcVideoDeviceHandle videoDevice) noexcept;

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
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCVideoDevice_GetDeviceName(
    rtcVideoDeviceHandle videoDevice, int index, char* pOutDeviceNameUTF8,
    int cchOutDeviceNameUTF8, char* pOutDeviceUniqueIdUTF8,
    int cchOutDeviceUniqueIdUTF8, char* pOutProductUniqueIdUTF8 = 0,
    int cchOutProductUniqueIdUTF8 = 0) noexcept;

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
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCVideoDevice_CreateVideoCapturer(
    rtcVideoDeviceHandle videoDevice, const char* name, int index, int width,
    int height, int target_fps, rtcVideoCapturerHandle* pOutRetVal) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCVideoCapturer interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Starts video capturing.
 *
 * @param videoCapturer - Video capturer handle
 * @return rtcBool32 - rtcBool32::kTrue if successful, otherwise
 * rtcBool32::kFalse
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
RTCVideoCapturer_StartCapture(rtcVideoCapturerHandle videoCapturer) noexcept;

/**
 * Returns whether the capture state has been initialized.
 *
 * @param videoCapturer - Video capturer handle
 * @return rtcBool32 - rtcBool32::kTrue if started, otherwise rtcBool32::kFalse
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
RTCVideoCapturer_CaptureStarted(rtcVideoCapturerHandle videoCapturer) noexcept;

/**
 * Stops video capture.
 *
 * @param videoCapturer - Video capturer handle
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCVideoCapturer_StopCapture(rtcVideoCapturerHandle videoCapturer) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCVideoFrame interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Creates a new instance of an empty video frame. (i420)
 *
 * @param width - Frame width
 * @param height - Frame height
 * @param pOutRetVal - Handle for the created video frame class.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCVideoFrame_Create0(
    int width, int height, rtcVideoFrameHandle* pOutRetVal) noexcept;

/**
 * Creates a new instance of the video frame
 * from the specified source buffer. (i420)
 *
 * @param width - Frame width
 * @param height - Frame height
 * @param buffer - Frame buffer
 * @param length - The size of the frame buffer
 * @param pOutRetVal - Handle for the created video frame class.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCVideoFrame_Create1(int width, int height, const unsigned char* buffer,
                      int length, rtcVideoFrameHandle* pOutRetVal) noexcept;

/**
 * Creates a new instance of the video frame
 * from the specified frame datas. (i420)
 *
 * @param frameDatas - Frame datas
 * @param pOutRetVal - Handle for the created video frame handle.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCVideoFrame_Create2(const rtcVideoFrameDatas* frameDatas,
                      rtcVideoFrameHandle* pOutRetVal) noexcept;

/**
 * Creates a copy of the video frame.
 *
 * @param videoFrame - Source video frame handle
 * @param pOutRetVal - Handle for the created video frame handle.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCVideoFrame_Copy(
    rtcVideoFrameHandle videoFrame, rtcVideoFrameHandle* pOutRetVal) noexcept;

/**
 * Returns the datas of the video frame.
 *
 * @param videoFrame - Source video frame handle
 * @param refFrameDatas - The frame datas.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCVideoFrame_GetFrameDatas(
    rtcVideoFrameHandle videoFrame, rtcVideoFrameDatas* refFrameDatas) noexcept;

/**
 * Returns the rotation of the video frame. (See: rtcVideoRotation)
 *
 * @param videoFrame - Source video frame handle
 * @param pRotation - The frame rotation value.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCVideoFrame_GetRotation(
    rtcVideoFrameHandle videoFrame, rtcVideoRotation* pOutRetVal) noexcept;

/**
 * Returns a timestamp in microseconds.
 *
 * @param videoFrame - Source video frame handle
 * @return rtcTimestamp - Timestamp in microseconds.
 */
LIB_WEBRTC_API rtcTimestamp LIB_WEBRTC_CALL
RTCVideoFrame_GetTimestampInMicroseconds(
    rtcVideoFrameHandle videoFrame) noexcept;

/**
 * Sets the timestamp in microseconds.
 *
 * @param videoFrame - Source video frame handle
 * @param timestampInMicroseconds - Timestamp in microseconds.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCVideoFrame_SetTimestampInMicroseconds(
    rtcVideoFrameHandle videoFrame,
    rtcTimestamp timestampInMicroseconds) noexcept;

/**
 * Converts the video frame to RGB colorspace.
 *
 * @param videoFrame - Source video frame handle
 * @param type - Type of destination video frame buffer.
 * @param dst_argb - Destination video frame buffer.
 * @param dst_stride_argb - Stride of destination video frame buffer.
 * @param dest_width - Width of destination video frame buffer.
 * @param dest_height - Height of destination video frame buffer.
 * @return int - Size of destination
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL RTCVideoFrame_ConvertToARGB(
    rtcVideoFrameHandle videoFrame, rtcVideoFrameType type,
    unsigned char* dst_argb, int dst_stride_argb, int dest_width,
    int dest_height) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCVideoRenderer interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Creates a new instance of the video renderer.
 *
 * @param pOutRetVal - Video renderer handle.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCVideoRenderer_Create(rtcVideoRendererHandle* pOutRetVal) noexcept;

/**
 * Registers the callback method for the video renderer.
 *
 * @param videoRenderer - Video renderer handle.
 * @param userData - User data handle
 * @param callback - Callback method for OnFrame
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCVideoRenderer_RegisterFrameCallback(
    rtcVideoRendererHandle videoRenderer, rtcObjectHandle userData,
    rtcVideoRendererFrameDelegate callback) noexcept;

/**
 * UnRegisters the callback method for the video renderer.
 *
 * @param videoRenderer - Video renderer handle.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCVideoRenderer_UnRegisterFrameCallback(
    rtcVideoRendererHandle videoRenderer) noexcept;

#if defined(DEBUG) || defined(_DEBUG)
/**
 * It is used for testing purposes to trigger
 * the OnRender callback method.
 *
 * @param videoRenderer - Video renderer handle.
 * @param videoFrame - Video frame handle.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCVideoRenderer_FireOnFrame(rtcVideoRendererHandle videoRenderer,
                             rtcVideoFrameHandle videoFrame) noexcept;
#endif  // DEBUG or _DEBUG

/*
 * ----------------------------------------------------------------------
 * RTCVideoTrack interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Adds a video renderer to the video track for frame callback operations.
 *
 * @param videoTrack - Video track handle.
 * @param videoRenderer - Video renderer handle.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCVideoTrack_AddRenderer(rtcVideoTrackHandle videoTrack,
                          rtcVideoRendererHandle videoRenderer) noexcept;

/**
 * Removes the video renderer that is already attached to
 * the video track for frame callback operations.
 *
 * @param videoTrack - Video track handle.
 * @param videoRenderer - Video renderer handle.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCVideoTrack_RemoveRenderer(rtcVideoTrackHandle videoTrack,
                             rtcVideoRendererHandle videoRenderer) noexcept;

#ifdef RTC_DESKTOP_DEVICE

/*
 * ----------------------------------------------------------------------
 * RTCDesktopMediaList interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Registers callback delegates for MediaListObserve.
 *
 * @param hMediaList - Desktop media list handle
 * @param rtcMediaListObserverCallbacks - Callback delegate structure for
 * MediaListObserve.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopMediaList_RegisterMediaListObserver(
    rtcDesktopMediaListHandle hMediaList,
    rtcMediaListObserverCallbacks* callbacks) noexcept;

/**
 * Unregisters callback delegates for MediaListObserve.
 *
 * @param hMediaList - Desktop media list handle
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopMediaList_DeRegisterMediaListObserver(
    rtcDesktopMediaListHandle hMediaList) noexcept;

/**
 * Returns the desktop type for the MediaListObserve.
 *
 * @param hMediaList - Desktop media list handle
 * @return rtcDesktopType - 0 if successful, otherwise an -1
 */
LIB_WEBRTC_API rtcDesktopType LIB_WEBRTC_CALL
RTCDesktopMediaList_GetType(rtcDesktopMediaListHandle hMediaList) noexcept;

/**
 * Updates media sources.
 *
 * @param hMediaList - Desktop media list handle
 * @param force_reload - Force reloads media sources.
 * @param get_thumbnail - Enables thumbnail get.
 * @return int - Returns the number of sources.
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL RTCDesktopMediaList_UpdateSourceList(
    rtcDesktopMediaListHandle hMediaList,
    rtcBool32 force_reload = rtcBool32::kFalse,
    rtcBool32 get_thumbnail = rtcBool32::kTrue) noexcept;

/**
 * Returns the current number of media sources.
 *
 * @param hMediaList - Desktop media list handle
 * @return int - Returns the number of sources.
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL RTCDesktopMediaList_GetSourceCount(
    rtcDesktopMediaListHandle hMediaList) noexcept;

/**
 * Returns the media source whose index is specified.
 *
 * @param hMediaList - Desktop media list handle
 * @param index - Media source index
 * @param pOutRetVal - Returns the media source handle.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopMediaList_GetSource(rtcDesktopMediaListHandle hMediaList, int index,
                              rtcDesktopMediaSourceHandle* pOutRetVal) noexcept;

/**
 * Gets the thumbnail.
 *
 * @param hMediaList - Desktop media list handle
 * @param source - Media source handle
 * @param notify - Triggers the callback method.
 * @return rtcBool32 - kTrue if successful, otherwise an kFalse.
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL RTCDesktopMediaList_GetThumbnail(
    rtcDesktopMediaListHandle hMediaList, rtcDesktopMediaSourceHandle hSource,
    rtcBool32 notify = rtcBool32::kFalse) noexcept;

/*
 * ----------------------------------------------------------------------
 * MediaSource interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Provides id, name and type information about the media source.
 *
 * @param mediaSource - Media source handle
 * @param pOutId - Media source id
 * @param cchOutId - The size of the id.
 * @param pOutName - Media source name
 * @param cchOutName - The size of the name.
 * @param pOutType - The desktop type of media source
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL MediaSource_GetInfo(
    rtcDesktopMediaSourceHandle mediaSource, char* pOutId, int cchOutId,
    char* pOutName, int cchOutName, rtcDesktopType* pOutType) noexcept;

/**
 * Updates the media source thumbnail.
 *
 * @param mediaSource - Media source handle
 * @return rtcBool32 - kTrue if successful, otherwise kFalse
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
MediaSource_UpdateThumbnail(rtcDesktopMediaSourceHandle mediaSource) noexcept;

/**
 * Returns the thumbnail of the media source, jpeg format.
 * At the end of the process, the 'refSizeOfBuffer' value
 * gives the actual size of the buffer area.
 *
 * @param mediaSource - Media source handle
 * @param pBuffer - Address of the buffer area for the thumbnail.
 * @param refSizeOfBuffer - The size of the thumbnail buffer.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
MediaSource_GetThumbnail(rtcDesktopMediaSourceHandle mediaSource,
                         unsigned char* pBuffer, int* refSizeOfBuffer) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCDesktopDevice interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Create desktop capturer
 *
 * @param desktopDevice - Desktop device handle
 * @param source - Media source handle
 * @param pOutRetVal - Returns the created desktop capture handle.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopDevice_CreateDesktopCapturer(
    rtcDesktopDeviceHandle desktopDevice, rtcDesktopMediaSourceHandle source,
    rtcDesktopCapturerHandle* pOutRetVal) noexcept;

/**
 * Returns the desktop media list.
 *
 * @param desktopDevice - Desktop device handle
 * @param type - Desktop type
 * @param pOutRetVal - Returns the created desktop media list handle.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL RTCDesktopDevice_GetDesktopMediaList(
    rtcDesktopDeviceHandle desktopDevice, rtcDesktopType type,
    rtcDesktopMediaListHandle* pOutRetVal) noexcept;

#endif  // RTC_DESKTOP_DEVICE

/*
 * ----------------------------------------------------------------------
 * RTCDataChannel interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_Send(
    rtcDataChannelHandle dataChannel,
    const unsigned char* data,
    int data_size,
    rtcBool32 binary = rtcBool32::kFalse
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_Close(
    rtcDataChannelHandle dataChannel
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_RegisterObserver(
    rtcDataChannelHandle dataChannel,
    rtcDataChannelObserverCallbacks* callbacks
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_UnregisterObserver(
    rtcDataChannelHandle dataChannel
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_GetLabel(
    rtcDataChannelHandle dataChannel,
    char* label,
    int label_size
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_GetId(
    rtcDataChannelHandle dataChannel,
    int* id
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDataChannel_GetState(
    rtcDataChannelHandle dataChannel,
    rtcDataChannelState* state
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCSdpParseError interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Creates a new instance of the RTCSdpParseError.
 * 
 * @param pOutRetVal - Handle of RTCSdpParseError
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCSdpParseError_Create(
    rtcSdpParseErrorHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCSdpParseError_GetLine(
    rtcSdpParseErrorHandle sdpParseError,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCSdpParseError_SetLine(
    rtcSdpParseErrorHandle sdpParseError,
    const char* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCSdpParseError_GetDescription(
    rtcSdpParseErrorHandle sdpParseError,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCSdpParseError_SetDescription(
    rtcSdpParseErrorHandle sdpParseError,
    const char* value
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCIceCandidate interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Creates a new instance of the RTCIceCandidate.
 * 
 * @param sdp - sdp string
 * @param sdp_mid - sdp media id
 * @param sdp_mline_index - sdp media line index
 * @param pOutSdpParseError - sdp parse error handle (optional)
 * @param pOutRetVal - Handle of RTCIceCandidate
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCIceCandidate_Create(
    const char* sdp,
    const char* sdp_mid,
    int sdp_mline_index,
    rtcSdpParseErrorHandle* pOutSdpParseError,
    rtcIceCandidateHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCIceCandidate_GetCandidate(
    rtcIceCandidateHandle iceCandidate,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCIceCandidate_GetSdpMid(
    rtcIceCandidateHandle iceCandidate,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCIceCandidate_GetSdpMlineIndex(
    rtcIceCandidateHandle iceCandidate,
    int* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCMediaStream interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Adds an audio track to the media stream.
 * 
 * @param mediaStream - Media stream handle
 * @param audioTrack - Audio track handle
 * @return rtcBool32 - kTrue if successful, otherwise kFalse.
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
RTCMediaStream_AddAudioTrack(
    rtcMediaStreamHandle mediaStream,
    rtcAudioTrackHandle audioTrack
) noexcept;

/**
 * Adds a video track to the media stream.
 * 
 * @param mediaStream - Media stream handle
 * @param videoTrack - Video track handle
 * @return rtcBool32 - kTrue if successful, otherwise kFalse.
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
RTCMediaStream_AddVideoTrack(
    rtcMediaStreamHandle mediaStream,
    rtcVideoTrackHandle videoTrack
) noexcept;

/**
 * Removes an audio track from the media stream.
 * 
 * @param mediaStream - Media stream handle
 * @param audioTrack - Audio track handle
 * @return rtcBool32 - kTrue if successful, otherwise kFalse.
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
RTCMediaStream_RemoveAudioTrack(
    rtcMediaStreamHandle mediaStream,
    rtcAudioTrackHandle audioTrack
) noexcept;

/**
 * Removes a video track from the media stream.
 * 
 * @param mediaStream - Media stream handle
 * @param videoTrack - Video track handle
 * @return rtcBool32 - kTrue if successful, otherwise kFalse.
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
RTCMediaStream_RemoveVideoTrack(
    rtcMediaStreamHandle mediaStream,
    rtcVideoTrackHandle videoTrack
) noexcept;

/**
 * Returns the number of audio tracks in the media stream.
 * 
 * @param mediaStream - Media stream handle
 * @return int - Number of audio tracks (if successful), otherwise 0.
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCMediaStream_NumberOfAudioTracks(
    rtcMediaStreamHandle mediaStream
) noexcept;

/**
 * Returns the number of video tracks in the media stream.
 * 
 * @param mediaStream - Media stream handle
 * @return int - Number of video tracks (if successful), otherwise 0.
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCMediaStream_NumberOfVideoTracks(
    rtcMediaStreamHandle mediaStream
) noexcept;

/**
 * Returns the audio track with the specified index.
 * 
 * @param mediaStream - Media stream handle
 * @param index - 0-based index value.
 * @param pOutRetVal - The audio track handle value to return.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCMediaStream_GetAudioTrack (
    rtcMediaStreamHandle mediaStream,
    int index,
    rtcAudioTrackHandle* pOutRetVal
) noexcept;

/**
 * Returns the video track with the specified index.
 * 
 * @param mediaStream - Media stream handle
 * @param index - 0-based index value.
 * @param pOutRetVal - The video track handle value to return.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCMediaStream_GetVideoTrack (
    rtcMediaStreamHandle mediaStream,
    int index,
    rtcVideoTrackHandle* pOutRetVal
) noexcept;

/**
 * Returns the audio track with the specified id.
 * 
 * @param mediaStream - Media stream handle
 * @param track_id - Track identifier
 * @param pOutRetVal - The audio track handle value to return.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCMediaStream_FindAudioTrack (
    rtcMediaStreamHandle mediaStream,
    const char* track_id,
    rtcAudioTrackHandle* pOutRetVal
) noexcept;

/**
 * Returns the video track with the specified id.
 * 
 * @param mediaStream - Media stream handle
 * @param track_id - Track identifier
 * @param pOutRetVal - The video track handle value to return.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCMediaStream_FindVideoTrack (
    rtcMediaStreamHandle mediaStream,
    const char* track_id,
    rtcVideoTrackHandle* pOutRetVal
) noexcept;

/**
 * Returns the label value for the media stream.
 * 
 * @param mediaStream - Media stream handle
 * @param value - Value to be returned
 * @param sz_value - Size of value
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCMediaStream_GetLabel (
    rtcMediaStreamHandle mediaStream,
    char* value,
    int sz_value
) noexcept;

/**
 * Returns the id value for the media stream.
 * 
 * @param mediaStream - Media stream handle
 * @param value - Value to be returned
 * @param sz_value - Size of value
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCMediaStream_GetId (
    rtcMediaStreamHandle mediaStream,
    char* value,
    int sz_value
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCMediaStreamList interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Returns the number of MediaStreams.
 * 
 * @param mediaStreamList - Media stream list handle
 * @return int - The number of streams
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCMediaStreamList_GetCount (
    rtcMediaStreamListHandle mediaStreamList
) noexcept;

/**
 * Returns the media stream whose index is specified.
 * 
 * @param mediaStreamList - Media stream list handle
 * @param index - Media stream index
 * @param pOutRetVal - Media stream handle
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCMediaStreamList_GetItem (
    rtcMediaStreamListHandle mediaStreamList,
    int index,
    rtcMediaStreamHandle* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpCodecCapability interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Creates a new instance of RTCRtpCodecCapability.
 * 
 * @param pOutRetVal - Handle of RTCRtpCodecCapability
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_Create(
    rtcRtpCodecCapabilityHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_GetMimeType(
    rtcRtpCodecCapabilityHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_SetMimeType(
    rtcRtpCodecCapabilityHandle handle,
    const char* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_GetClockRate(
    rtcRtpCodecCapabilityHandle handle,
    int* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_SetClockRate(
    rtcRtpCodecCapabilityHandle handle,
    int value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_GetChannels(
    rtcRtpCodecCapabilityHandle handle,
    int* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_SetChannels(
    rtcRtpCodecCapabilityHandle handle,
    int value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_GetSdpFmtpLine(
    rtcRtpCodecCapabilityHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapability_SetSdpFmtpLine(
    rtcRtpCodecCapabilityHandle handle,
    const char* value
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpCodecCapabilityList interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Returns the number of RtpCodecCapabilityList.
 * 
 * @param handle - List handle
 * @return int - The number of items
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCRtpCodecCapabilityList_GetCount (
    rtcRtpCodecCapabilityListHandle handle
) noexcept;

/**
 * Returns the rtp codec capability whose index is specified.
 * 
 * @param handle - List handle
 * @param index - Item index
 * @param pOutRetVal - Item handle
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecCapabilityList_GetItem (
    rtcRtpCodecCapabilityListHandle handle,
    int index,
    rtcRtpCodecCapabilityHandle* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpHeaderExtensionCapability interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_GetUri(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_SetUri(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    const char* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_GetPreferredId(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    int* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_SetPreferredId(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    int value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_GetPreferredEncrypt(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    rtcBool32* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapability_SetPreferredEncrypt(
    rtcRtpHeaderExtensionCapabilityHandle handle,
    rtcBool32 value
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpHeaderExtensionCapabilityList interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Returns the number of RtpHeaderExtensionCapabilityList.
 * 
 * @param handle - List handle
 * @return int - The number of items
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapabilityList_GetCount (
    rtcRtpHeaderExtensionCapabilityListHandle handle
) noexcept;

/**
 * Returns the rtp header extension capability whose index is specified.
 * 
 * @param handle - List handle
 * @param index - Item index
 * @param pOutRetVal - Item handle
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpHeaderExtensionCapabilityList_GetItem (
    rtcRtpHeaderExtensionCapabilityListHandle handle,
    int index,
    rtcRtpHeaderExtensionCapabilityHandle* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpCapabilities interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCapabilities_GetCodecs(
    rtcRtpCapabilitiesHandle handle,
    rtcRtpCodecCapabilityListHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCapabilities_SetCodecs(
    rtcRtpCapabilitiesHandle handle,
    rtcRtpCodecCapabilityListHandle hCodecs
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCapabilities_GetHeaderExtensions(
    rtcRtpCapabilitiesHandle handle,
    rtcRtpHeaderExtensionCapabilityListHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCapabilities_SetHeaderExtensions(
    rtcRtpCapabilitiesHandle handle,
    rtcRtpHeaderExtensionCapabilityListHandle hHeaderExtensions
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtcpFeedback interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Returns the RtcpFeedback type.
 * 
 * @param handle - Object handle
 * @param pOutRetVal - The value to return.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpFeedback_GetType(
    rtcRtcpFeedbackHandle handle,
    rtcRtcpFeedbackType* pOutRetVal
) noexcept;

/**
 * Sets the RtcpFeedback type.
 * 
 * @param handle - Object handle
 * @param value - The new value to set.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpFeedback_SetType(
    rtcRtcpFeedbackHandle handle,
    rtcRtcpFeedbackType value
) noexcept;

/**
 * Returns the RtcpFeedback message type.
 * 
 * @param handle - Object handle
 * @param pOutRetVal - The value to return.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpFeedback_GetMessageType(
    rtcRtcpFeedbackHandle handle,
    rtcRtcpFeedbackMessageType* pOutRetVal
) noexcept;

/**
 * Sets the RtcpFeedback message type.
 * 
 * @param handle - Object handle
 * @param value - The new value to set.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpFeedback_SetMessageType(
    rtcRtcpFeedbackHandle handle,
    rtcRtcpFeedbackMessageType value
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtcpFeedbackList interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Returns the number of RTCRtcpFeedbackList.
 * 
 * @param handle - List handle
 * @return int - The number of items
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCRtcpFeedbackList_GetCount (
    rtcRtcpFeedbackListHandle handle
) noexcept;

/**
 * Returns the Rtcp Feedback capability whose index is specified.
 * 
 * @param handle - List handle
 * @param index - Item index
 * @param pOutRetVal - Item handle
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpFeedbackList_GetItem (
    rtcRtcpFeedbackListHandle handle,
    int index,
    rtcRtcpFeedbackHandle* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpExtension interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Returns the text value for RtpExtension.
 * 
 * @param handle - Object handle
 * @param value - Return string buffer address
 * @param sz_value - Size of value
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtension_ToString (
    rtcRtpExtensionHandle handle,
    char* value,
    int sz_value
) noexcept;

/**
 * Returns the uri for RtpExtension.
 * 
 * @param handle - Object handle
 * @param value - Return string buffer address
 * @param sz_value - Size of value
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtension_GetUri (
    rtcRtpExtensionHandle handle,
    char* value,
    int sz_value
) noexcept;

/**
 * Sets the RtpExtension uri value.
 * 
 * @param handle - Object handle
 * @param value - The new value to set.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtension_SetUri (
    rtcRtpExtensionHandle handle,
    const char* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtension_GetId (
    rtcRtpExtensionHandle handle,
    int* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtension_SetId (
    rtcRtpExtensionHandle handle,
    int value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtension_GetEncrypt (
    rtcRtpExtensionHandle handle,
    rtcBool32* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtension_SetEncrypt (
    rtcRtpExtensionHandle handle,
    rtcBool32 value
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpExtensionList interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Returns the number of RTCRtpExtensionList.
 * 
 * @param handle - List handle
 * @return int - The number of items
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCRtpExtensionList_GetCount (
    rtcRtpExtensionListHandle handle
) noexcept;

/**
 * Returns the rtp extension whose index is specified.
 * 
 * @param handle - List handle
 * @param index - Item index
 * @param pOutRetVal - Item handle
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpExtensionList_GetItem (
    rtcRtpExtensionListHandle handle,
    int index,
    rtcRtpExtensionHandle* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpCodecParametersPair interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Creates a new pair instance.
 * 
 * @param pair - Single pair item
 * @param pOutRetVal - The handle of the object to be returned.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParametersPair_Create(
    const rtcStringPair* pair,
    rtcRtpCodecParametersPairHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParametersPair_GetKey(
    rtcRtpCodecParametersPairHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParametersPair_GetValue(
    rtcRtpCodecParametersPairHandle handle,
    char* value,
    int sz_value
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpCodecParametersMap interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Creates a new pair map.
 * 
 * @param pairs - Array
 * @param length - Count of array
 * @param pOutRetVal - The handle of the object to be returned.
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParametersMap_Create(
    const rtcStringPair* pairs,
    int length,
    rtcRtpCodecParametersPairHandle* pOutRetVal
) noexcept;

/**
 * Returns the number of pairs.
 * 
 * @param handle - Map handle
 * @return int - The number of pairs
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCRtpCodecParametersMap_GetCount (
    rtcRtpCodecParametersMapHandle handle
) noexcept;

/**
 * Returns the pair whose index is specified.
 * 
 * @param handle - Map handle
 * @param index - Pair index
 * @param pOutRetVal - Pair handle
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParametersMap_GetItem (
    rtcRtpCodecParametersMapHandle handle,
    int index,
    rtcRtpCodecParametersPairHandle* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpCodecParameters interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetMimeType (
    rtcRtpCodecParametersHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetName (
    rtcRtpCodecParametersHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetName (
    rtcRtpCodecParametersHandle handle,
    const char* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetKind (
    rtcRtpCodecParametersHandle handle,
    rtcMediaType* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetKind (
    rtcRtpCodecParametersHandle handle,
    rtcMediaType value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetPayloadType (
    rtcRtpCodecParametersHandle handle,
    int* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetPayloadType (
    rtcRtpCodecParametersHandle handle,
    int value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetClockRate (
    rtcRtpCodecParametersHandle handle,
    int* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetClockRate (
    rtcRtpCodecParametersHandle handle,
    int value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetNumChannels (
    rtcRtpCodecParametersHandle handle,
    int* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetNumChannels (
    rtcRtpCodecParametersHandle handle,
    int value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetMaxPTime (
    rtcRtpCodecParametersHandle handle,
    int* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetMaxPTime (
    rtcRtpCodecParametersHandle handle,
    int value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetPTime (
    rtcRtpCodecParametersHandle handle,
    int* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetPTime (
    rtcRtpCodecParametersHandle handle,
    int value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetRtcpFeedBacks (
    rtcRtpCodecParametersHandle handle,
    rtcRtcpFeedbackListHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetRtcpFeedBacks (
    rtcRtpCodecParametersHandle handle,
    rtcRtcpFeedbackListHandle value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_GetParameters (
    rtcRtpCodecParametersHandle handle,
    rtcRtpCodecParametersMapHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParameters_SetParameters (
    rtcRtpCodecParametersHandle handle,
    rtcRtpCodecParametersMapHandle value
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpCodecParametersList interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Returns the number of RTCRtpCodecParametersList.
 * 
 * @param handle - List handle
 * @return int - The number of items
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCRtpCodecParametersList_GetCount (
    rtcRtpCodecParametersListHandle handle
) noexcept;

/**
 * Returns the rtp codec parameters index is specified.
 * 
 * @param handle - List handle
 * @param index - Item index
 * @param pOutRetVal - Item handle
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpCodecParametersList_GetItem (
    rtcRtpCodecParametersListHandle handle,
    int index,
    rtcRtpCodecParametersHandle* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtcpParameters interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_GetSSRC (
    rtcRtcpParametersHandle handle,
    unsigned int* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_SetSSRC (
    rtcRtcpParametersHandle handle,
    unsigned int value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_GetCName (
    rtcRtcpParametersHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_SetCName (
    rtcRtcpParametersHandle handle,
    const char* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_GetReducedSize (
    rtcRtcpParametersHandle handle,
    rtcBool32* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_SetReducedSize (
    rtcRtcpParametersHandle handle,
    rtcBool32 value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_GetMux (
    rtcRtcpParametersHandle handle,
    rtcBool32* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParameters_SetMux (
    rtcRtcpParametersHandle handle,
    rtcBool32 value
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtcpParametersList interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Returns the number of RTCRtcpParametersList.
 * 
 * @param handle - List handle
 * @return int - The number of items
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCRtcpParametersList_GetCount (
    rtcRtcpParametersListHandle handle
) noexcept;

/**
 * Returns the rtcp parameters index is specified.
 * 
 * @param handle - List handle
 * @param index - Item index
 * @param pOutRetVal - Item handle
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtcpParametersList_GetItem (
    rtcRtcpParametersListHandle handle,
    int index,
    rtcRtcpParametersHandle* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpEncodingParameters interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetSSRC (
    rtcRtpEncodingParametersHandle handle,
    unsigned int* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetSSRC (
    rtcRtpEncodingParametersHandle handle,
    unsigned int value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetBitratePriority (
    rtcRtpEncodingParametersHandle handle,
    double* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetBitratePriority (
    rtcRtpEncodingParametersHandle handle,
    double value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetNetworkPriority (
    rtcRtpEncodingParametersHandle handle,
    rtcNetworkPriority* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetNetworkPriority (
    rtcRtpEncodingParametersHandle handle,
    rtcNetworkPriority value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetMaxBitrateBps (
    rtcRtpEncodingParametersHandle handle,
    int* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetMaxBitrateBps (
    rtcRtpEncodingParametersHandle handle,
    int value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetMinBitrateBps (
    rtcRtpEncodingParametersHandle handle,
    int* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetMinBitrateBps (
    rtcRtpEncodingParametersHandle handle,
    int value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetMaxFramerate (
    rtcRtpEncodingParametersHandle handle,
    double* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetMaxFramerate (
    rtcRtpEncodingParametersHandle handle,
    double value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetNumTemporalLayers (
    rtcRtpEncodingParametersHandle handle,
    int* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetNumTemporalLayers (
    rtcRtpEncodingParametersHandle handle,
    int value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetScaleResolutionDownBy (
    rtcRtpEncodingParametersHandle handle,
    double* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetScaleResolutionDownBy (
    rtcRtpEncodingParametersHandle handle,
    double value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetScalabilityMode (
    rtcRtpEncodingParametersHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetScalabilityMode (
    rtcRtpEncodingParametersHandle handle,
    const char* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetActive (
    rtcRtpEncodingParametersHandle handle,
    rtcBool32* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetActive (
    rtcRtpEncodingParametersHandle handle,
    rtcBool32 value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetRid (
    rtcRtpEncodingParametersHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetRid (
    rtcRtpEncodingParametersHandle handle,
    const char* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_GetAdaptivePTime(
    rtcRtpEncodingParametersHandle handle,
    rtcBool32* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParameters_SetAdaptivePTime (
    rtcRtpEncodingParametersHandle handle,
    rtcBool32 value
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpEncodingParametersList interop methods
 * ----------------------------------------------------------------------
 */

/**
 * Returns the number of RTCRtpEncodingParametersList.
 * 
 * @param handle - List handle
 * @return int - The number of items
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCRtpEncodingParametersList_GetCount (
    rtcRtpEncodingParametersListHandle handle
) noexcept;

/**
 * Returns the rtp encoding parameters index is specified.
 * 
 * @param handle - List handle
 * @param index - Item index
 * @param pOutRetVal - Item handle
 * @return rtcResultU4 - 0 if successful, otherwise an error code.
 */
LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpEncodingParametersList_GetItem (
    rtcRtpEncodingParametersListHandle handle,
    int index,
    rtcRtpEncodingParametersHandle* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpParameters interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_GetTransactionId (
    rtcRtpParametersHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_SetTransactionId (
    rtcRtpParametersHandle handle,
    const char* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_GetMid (
    rtcRtpParametersHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_SetMid (
    rtcRtpParametersHandle handle,
    const char* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_GetCodecs (
    rtcRtpParametersHandle handle,
    rtcRtpCodecParametersListHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_SetCodecs (
    rtcRtpParametersHandle handle,
    rtcRtpCodecParametersListHandle value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_GetHeaderExtensions (
    rtcRtpParametersHandle handle,
    rtcRtpExtensionListHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_SetHeaderExtensions (
    rtcRtpParametersHandle handle,
    rtcRtpExtensionListHandle value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_GetEncodings (
    rtcRtpParametersHandle handle,
    rtcRtpEncodingParametersListHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_SetEncodings (
    rtcRtpParametersHandle handle,
    rtcRtpEncodingParametersListHandle value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_GetRtcpParameters (
    rtcRtpParametersHandle handle,
    rtcRtcpParametersHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_SetRtcpParameters (
    rtcRtpParametersHandle handle,
    rtcRtcpParametersHandle value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_GetDegradationPreference (
    rtcRtpParametersHandle handle,
    rtcDegradationPreference* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpParameters_SetDegradationPreference (
    rtcRtpParametersHandle handle,
    rtcDegradationPreference value
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpReceiver interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_GetTrack (
    rtcRtpReceiverHandle handle,
    rtcMediaTrackHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_GetDtlsTransport (
    rtcRtpReceiverHandle handle,
    rtcDtlsTransportHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_GetStreamIds (
    rtcRtpReceiverHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_GetStreams (
    rtcRtpReceiverHandle handle,
    rtcMediaStreamListHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_GetMediaType (
    rtcRtpReceiverHandle handle,
    rtcMediaType* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_GetId (
    rtcRtpReceiverHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_GetParameters (
    rtcRtpReceiverHandle handle,
    rtcRtpParametersHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_SetParameters (
    rtcRtpReceiverHandle handle,
    rtcRtpParametersHandle value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_RegisterObserver (
    rtcRtpReceiverHandle handle,
    rtcRtpReceiverObserverCallbacks* callbacks
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_UnregisterObserver(
    rtcRtpReceiverHandle handle
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiver_SetJitterBufferMinimumDelay (
    rtcRtpReceiverHandle handle,
    double value
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpReceiverList interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCRtpReceiverList_GetCount (
    rtcRtpReceiverListHandle handle
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpReceiverList_GetItem (
    rtcRtpReceiverListHandle handle,
    int index,
    rtcRtpReceiverHandle* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCDtmfSender interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_RegisterObserver (
    rtcDtmfSenderHandle handle,
    rtcDtmfSenderObserverCallbacks* callbacks
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_UnregisterObserver (
    rtcDtmfSenderHandle handle
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_InsertDtmf (
    rtcDtmfSenderHandle handle,
    const char* tones,
    int duration,
    int inter_tone_gap,
    const int* comma_delay = nullptr
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_CanInsertDtmf (
    rtcDtmfSenderHandle handle,
    rtcBool32* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_GetTones (
    rtcDtmfSenderHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_GetDuration (
    rtcDtmfSenderHandle handle,
    int* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_GetInterToneGap (
    rtcDtmfSenderHandle handle,
    int* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDtmfSender_GetCommaDelay (
    rtcDtmfSenderHandle handle,
    int* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpSender interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetTrack (
    rtcRtpSenderHandle handle,
    rtcMediaTrackHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_SetTrack (
    rtcRtpSenderHandle handle,
    rtcMediaTrackHandle value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetDtlsTransport (
    rtcRtpSenderHandle handle,
    rtcDtlsTransportHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetSSRC (
    rtcRtpSenderHandle handle,
    unsigned int* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetMediaType (
    rtcRtpSenderHandle handle,
    rtcMediaType* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetId (
    rtcRtpSenderHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetStreamIds (
    rtcRtpSenderHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_SetStreamIds (
    rtcRtpSenderHandle handle,
    const char* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetInitEncodings (
    rtcRtpSenderHandle handle,
    rtcRtpEncodingParametersListHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetParameters (
    rtcRtpSenderHandle handle,
    rtcRtpParametersHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_SetParameters (
    rtcRtpSenderHandle handle,
    rtcRtpParametersHandle value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSender_GetDtmfSender (
    rtcRtpSenderHandle handle,
    rtcDtmfSenderHandle* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpSenderList interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCRtpSenderList_GetCount (
    rtcRtpSenderListHandle handle
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpSenderList_GetItem (
    rtcRtpSenderListHandle handle,
    int index,
    rtcRtpSenderHandle* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCDtlsTransportInformation interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDtlsTransportInformation_GetState (
    rtcDtlsTransportInformationHandle handle,
    rtcDtlsTransportState* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDtlsTransportInformation_GetSslCipherSuite (
    rtcDtlsTransportInformationHandle handle,
    int* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDtlsTransportInformation_GetSrtpCipherSuite (
    rtcDtlsTransportInformationHandle handle,
    int* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCDtlsTransport interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDtlsTransport_GetInformation (
    rtcDtlsTransportHandle handle,
    rtcDtlsTransportInformationHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDtlsTransport_RegisterObserver (
    rtcDtlsTransportHandle handle,
    rtcDtlsTransportObserverCallbacks* callbacks
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCDtlsTransport_UnregisterObserver (
    rtcDtlsTransportHandle handle
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpTransceiverInit interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiverInit_Create (
    rtcRtpTransceiverDirection direction,
    const char* stream_ids,
    /* [optional, in] */ rtcRtpEncodingParametersListHandle encodings,
    rtcRtpTransceiverInitHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiverInit_GetDirection (
    rtcRtpTransceiverInitHandle handle,
    rtcRtpTransceiverDirection* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiverInit_SetDirection (
    rtcRtpTransceiverInitHandle handle,
    rtcRtpTransceiverDirection value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiverInit_GetStreamIds (
    rtcRtpTransceiverInitHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiverInit_SetStreamIds (
    rtcRtpTransceiverInitHandle handle,
    const char* value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiverInit_GetEncodings (
    rtcRtpTransceiverInitHandle handle,
    rtcRtpEncodingParametersListHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiverInit_SetEncodings (
    rtcRtpTransceiverInitHandle handle,
    rtcRtpEncodingParametersListHandle value
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpTransceiver interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetMediaType (
    rtcRtpTransceiverHandle handle,
    rtcMediaType* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetMid (
    rtcRtpTransceiverHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetSender (
    rtcRtpTransceiverHandle handle,
    rtcRtpSenderHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetReceiver (
    rtcRtpTransceiverHandle handle,
    rtcRtpReceiverHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_Stopped (
    rtcRtpTransceiverHandle handle,
    rtcBool32* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_Stopping (
    rtcRtpTransceiverHandle handle,
    rtcBool32* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetDirection (
    rtcRtpTransceiverHandle handle,
    rtcRtpTransceiverDirection* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_SetDirectionWithError (
    rtcRtpTransceiverHandle handle,
    rtcRtpTransceiverDirection value,
    char* retVal,
    int sz_retVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetCurrentDirection (
    rtcRtpTransceiverHandle handle,
    rtcRtpTransceiverDirection* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetFiredDirection (
    rtcRtpTransceiverHandle handle,
    rtcRtpTransceiverDirection* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_StopStandard (
    rtcRtpTransceiverHandle handle,
    char* value,
    int sz_value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_SetCodecPreferences (
    rtcRtpTransceiverHandle handle,
    rtcRtpCodecCapabilityListHandle value
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiver_GetTransceiverId (
    rtcRtpTransceiverHandle handle,
    char* value,
    int sz_value
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCRtpTransceiverList interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCRtpTransceiverList_GetCount (
    rtcRtpTransceiverListHandle handle
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCRtpTransceiverList_GetItem (
    rtcRtpTransceiverListHandle handle,
    int index,
    rtcRtpTransceiverHandle* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * MediaRTCStatsList interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API int LIB_WEBRTC_CALL
RTCMediaRTCStatsList_GetCount (
    rtcMediaRTCStatsListHandle handle
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCMediaRTCStatsList_GetItem (
    rtcMediaRTCStatsListHandle handle,
    int index,
    rtcMediaRTCStatsHandle* pOutRetVal
) noexcept;

/*
 * ----------------------------------------------------------------------
 * RTCPeerConnection interop methods
 * ----------------------------------------------------------------------
 */

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_IsInitialized (
    rtcPeerConnectionHandle handle,
    rtcBool32* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_AddStream (
    rtcPeerConnectionHandle handle,
    rtcMediaStreamHandle stream,
    int* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_RemoveStream (
    rtcPeerConnectionHandle handle,
    rtcMediaStreamHandle stream,
    int* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_CreateLocalMediaStream (
    rtcPeerConnectionHandle handle,
    const char* stream_id,
    rtcMediaStreamHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_CreateDataChannel (
    rtcPeerConnectionHandle handle,
    const char* label,
    /* [optional, in] */ rtcDataChannelInit* dataChannelDict,
    rtcDataChannelHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_CreateOffer (
    rtcPeerConnectionHandle handle,
    rtcOnGetSdpCallbacks* callbacks,
    rtcMediaConstraintsHandle constraints
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_CreateAnswer (
    rtcPeerConnectionHandle handle,
    rtcOnGetSdpCallbacks* callbacks,
    rtcMediaConstraintsHandle constraints
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_RestartIce (
    rtcPeerConnectionHandle handle
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_Close (
    rtcPeerConnectionHandle handle
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_SetLocalDescription (
    rtcPeerConnectionHandle handle,
    const char* sdp,
    const char* type,
    rtcOnSetSdpCallbacks* callbacks
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_SetRemoteDescription (
    rtcPeerConnectionHandle handle,
    const char* sdp,
    const char* type,
    rtcOnSetSdpCallbacks* callbacks
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetLocalDescription (
    rtcPeerConnectionHandle handle,
    rtcOnGetSdpCallbacks* callbacks
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetRemoteDescription (
    rtcPeerConnectionHandle handle,
    rtcOnGetSdpCallbacks* callbacks
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_AddCandidate (
    rtcPeerConnectionHandle handle,
    const char* mid, 
    int mid_mline_index,
    const char* candiate
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_RegisterObserver (
    rtcPeerConnectionHandle handle,
    rtcPeerConnectionObserverCallbacks* callbacks
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_UnregisterObserver (
    rtcPeerConnectionHandle handle
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetLocalStreams (
    rtcPeerConnectionHandle handle,
    rtcMediaStreamListHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetRemoteStreams (
    rtcPeerConnectionHandle handle,
    rtcMediaStreamListHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetSenderStats (
    rtcPeerConnectionHandle handle,
    rtcRtpSenderHandle sender,
    rtcOnStatsCallbacks* callbacks,
    rtcBool32* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetReceiverStats (
    rtcPeerConnectionHandle handle,
    rtcRtpReceiverHandle receiver,
    rtcOnStatsCallbacks* callbacks,
    rtcBool32* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetStats (
    rtcPeerConnectionHandle handle,
    rtcOnStatsCallbacks* callbacks
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_AddTransceiver1 (
    rtcPeerConnectionHandle handle,
    rtcMediaType media_type,
    rtcRtpTransceiverHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_AddTransceiver2 (
    rtcPeerConnectionHandle handle,
    rtcMediaTrackHandle track,
    rtcRtpTransceiverHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_AddTransceiver3 (
    rtcPeerConnectionHandle handle,
    rtcMediaTrackHandle track,
    rtcRtpTransceiverInitHandle init,
    rtcRtpTransceiverHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_AddTrack (
    rtcPeerConnectionHandle handle,
    rtcMediaTrackHandle track,
    const char* stream_ids,
    rtcRtpSenderHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_RemoveTrack (
    rtcPeerConnectionHandle handle,
    rtcRtpSenderHandle sender,
    rtcBool32* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetSenders (
    rtcPeerConnectionHandle handle,
    rtcRtpSenderListHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetTransceivers (
    rtcPeerConnectionHandle handle,
    rtcRtpTransceiverListHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetReceivers (
    rtcPeerConnectionHandle handle,
    rtcRtpReceiverListHandle* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetSignalingState (
    rtcPeerConnectionHandle handle,
    rtcSignalingState* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetIceConnectionState (
    rtcPeerConnectionHandle handle,
    rtcIceConnectionState* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetStandardizedIceConnectionState (
    rtcPeerConnectionHandle handle,
    rtcIceConnectionState* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetPeerConnectionState (
    rtcPeerConnectionHandle handle,
    rtcPeerConnectionState* pOutRetVal
) noexcept;

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetIceGatheringState (
    rtcPeerConnectionHandle handle,
    rtcIceGatheringState* pOutRetVal
) noexcept;

}  // extern "C"

#endif  // LIB_WEBRTC_INTROP_API_HXX