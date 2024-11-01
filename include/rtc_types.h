#ifndef LIB_WEBRTC_RTC_TYPES_HXX
#define LIB_WEBRTC_RTC_TYPES_HXX

#include "rtc_config.h"

#include <map>

#ifdef USE_PORTABLE_TYPES
#include "base/portable.h"
#include "base/fixed_size_function.h"
#else
#include <string>
#include <vector>
#include <functional>
#endif

#include "base/refcount.h"
#include "base/scoped_ref_ptr.h"

namespace libwebrtc {

#ifdef USE_PORTABLE_TYPES
using string = portable::string;

inline std::string to_std_string(const string& str) { return str.std_string(); }

template <typename Key, typename T>
using map = std::map<Key, T>;

template <typename T>
using vector = portable::vector<T>;

template <typename T>
inline std::vector<T> to_std_vector(const vector<T>& vec) {
  return vec.std_vector();
}

#define callback_function_t fixed_size_function

#else
using string = std::string;
#define to_std_string(str) str

template <typename T>
using vector = std::vector<T>;
#define to_std_vector(vec) vec

template <typename Key, typename T>
using map = std::map<Key, T>;

#define callback_function_t std::function

#endif

enum { kMaxIceServerSize = 8 };

enum class MediaSecurityType { kSRTP_None = 0, kSDES_SRTP, kDTLS_SRTP };

enum class RTCMediaType { AUDIO, VIDEO, DATA, UNSUPPORTED };

struct IceServer {
  string uri;
  string username;
  string password;
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
  public:
  RTCConfiguration() {
    for (int i = 0; i < kMaxIceServerSize; i++) {
      ice_servers[i].uri = "";
      ice_servers[i].username = "";
      ice_servers[i].password = "";
    }
  }

  RTCConfiguration& operator=(const RTCConfiguration& other) {
    for (int i = 0; i < kMaxIceServerSize; i++) {
      ice_servers[i].uri = other.ice_servers[i].uri;
      ice_servers[i].username = other.ice_servers[i].username;
      ice_servers[i].password = other.ice_servers[i].password;
    }
    type = other.type;
    bundle_policy = other.bundle_policy;
    rtcp_mux_policy = other.rtcp_mux_policy;
    candidate_network_policy = other.candidate_network_policy;
    tcp_candidate_policy = other.tcp_candidate_policy;
    ice_candidate_pool_size = other.ice_candidate_pool_size;
    srtp_type = other.srtp_type;
    sdp_semantics = other.sdp_semantics;
    offer_to_receive_audio = other.offer_to_receive_audio;
    offer_to_receive_video = other.offer_to_receive_video;
    disable_ipv6 = other.disable_ipv6;
    disable_ipv6_on_wifi = other.disable_ipv6_on_wifi;
    max_ipv6_networks = other.max_ipv6_networks;
    disable_link_local_networks = other.disable_link_local_networks;
    screencast_min_bitrate = other.screencast_min_bitrate;
    use_rtp_mux = other.use_rtp_mux;
    local_audio_bandwidth = other.local_audio_bandwidth;
    local_video_bandwidth = other.local_video_bandwidth;
    return *this;
  }
};

struct SdpParseError {
 public:
  // The sdp line that causes the error.
  string line;
  // Explains the error.
  string description;
};

enum DesktopType { kScreen, kWindow };

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_TYPES_HXX
