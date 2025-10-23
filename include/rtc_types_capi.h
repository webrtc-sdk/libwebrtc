#ifndef LIB_WEBRTC_RTC_TYPES_HXX_CAPI
#define LIB_WEBRTC_RTC_TYPES_HXX_CAPI

#ifdef LIB_WEBRTC_API_EXPORTS
#define LIB_WEBRTC_CAPI __declspec(dllexport)
#elif defined(LIB_WEBRTC_API_DLL)
#define LIB_WEBRTC_CAPI __declspec(dllimport)
#elif !defined(WIN32)
#define LIB_WEBRTC_CAPI __attribute__((visibility("default")))
#else
#define LIB_WEBRTC_CAPI
#endif

#include <stdint.h>

extern "C" {
typedef void* rtcRefCountedObjectHandle;

typedef struct {
  const char* message;
} rtcError;

enum class rtcBoolean : int { kFalse = 0, kTrue = 1 };

enum class rtcResult : uint32_t {
  kSuccess = 0,
  kInvalidPointer = 1,
};

const int kMaxIceServerSize = 8;

enum class MediaSecurityType : int {
  kSRTP_None = 0,
  kSDES_SRTP = 1,
  kDTLS_SRTP = 2
};

enum class RTCMediaType : int {
  AUDIO = 0,
  VIDEO = 1,
  DATA = 2,
  UNSUPPORTED = 3
};

struct IceServer {
  const char* uri = nullptr;
  const char* username = nullptr;
  const char* password = nullptr;
};

enum class IceTransportsType { kNone, kRelay, kNoHost, kAll };

enum class TcpCandidatePolicy {
  kTcpCandidatePolicyEnabled,
  kTcpCandidatePolicyDisabled
};

enum class CandidateNetworkPolicy {
  kCandidateNetworkPolicyAll,
  kCandidateNetworkPolicyLowCost
};

enum class RtcpMuxPolicy {
  kRtcpMuxPolicyNegotiate,
  kRtcpMuxPolicyRequire,
};

enum BundlePolicy {
  kBundlePolicyBalanced,
  kBundlePolicyMaxBundle,
  kBundlePolicyMaxCompat
};

enum class SdpSemantics { kPlanB, kUnifiedPlan };

struct RTCConfiguration {
  IceServer ice_servers[kMaxIceServerSize];
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
  bool offer_to_receive_audio = true;
  bool offer_to_receive_video = true;

  bool disable_ipv6 = false;
  bool disable_ipv6_on_wifi = false;
  int max_ipv6_networks = 5;
  bool disable_link_local_networks = false;
  int screencast_min_bitrate = -1;

  // private
  bool use_rtp_mux = true;
  uint32_t local_audio_bandwidth = 128;
  uint32_t local_video_bandwidth = 512;
};

struct SdpParseError {
  // The sdp line that causes the error.
  const char* line = nullptr;
  // Explains the error.
  const char* description = nullptr;
};

enum DesktopType { kScreen, kWindow };

#define CHECK_POINTER_EX(p, r) \
  if ((p) == nullptr) {        \
    return (r);                \
  }

#define CHECK_POINTER(p) CHECK_POINTER_EX(p, rtcResult::kInvalidPointer)
}

#endif  // LIB_WEBRTC_RTC_TYPES_HXX
