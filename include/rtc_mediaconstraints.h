#ifndef LIB_WEBRTC_RTC_MEDIA_CONSTRAINTS_HXX
#define LIB_WEBRTC_RTC_MEDIA_CONSTRAINTS_HXX

#include "rtc_types.h"

namespace libwebrtc {

class RTCMediaConstraints : public RefCountInterface {
 public:
  // These keys are google specific.
  LIB_WEBRTC_API static string
      kGoogEchoCancellation;  // googEchoCancellation
  LIB_WEBRTC_API static string
      kDAEchoCancellation;                         // googDAEchoCancellation
  LIB_WEBRTC_API static string kAutoGainControl;  // googAutoGainControl
  LIB_WEBRTC_API static string kNoiseSuppression;  // googNoiseSuppression
  LIB_WEBRTC_API static string kHighpassFilter;    // googHighpassFilter
  LIB_WEBRTC_API static string kAudioMirroring;    // googAudioMirroring
  LIB_WEBRTC_API static string
      kAudioNetworkAdaptorConfig;  // goodAudioNetworkAdaptorConfig

  // Constraint keys for CreateOffer / CreateAnswer
  // Specified by the W3C PeerConnection spec
  LIB_WEBRTC_API static string
      kOfferToReceiveVideo;  // OfferToReceiveVideo
  LIB_WEBRTC_API static string
      kOfferToReceiveAudio;  // OfferToReceiveAudio
  LIB_WEBRTC_API static string
      kVoiceActivityDetection;                    // VoiceActivityDetection
  LIB_WEBRTC_API static string kIceRestart;  // IceRestart
  // These keys are google specific.
  LIB_WEBRTC_API static string kUseRtpMux;  // googUseRtpMUX

  // Constraints values.
  LIB_WEBRTC_API static string kValueTrue;   // true
  LIB_WEBRTC_API static string kValueFalse;  // false

  // PeerConnection constraint keys.
  // Google-specific constraint keys.
  // Temporary pseudo-constraint for enabling DSCP through JS.
  LIB_WEBRTC_API static string kEnableDscp;  // googDscp
  // Constraint to enable IPv6 through JS.
  LIB_WEBRTC_API static string kEnableIPv6;  // googIPv6
  // Temporary constraint to enable suspend below min bitrate feature.
  LIB_WEBRTC_API static string kEnableVideoSuspendBelowMinBitrate;
  // googSuspendBelowMinBitrate
  // Constraint to enable combined audio+video bandwidth estimation.
  //LIB_WEBRTC_API static string
  //    kCombinedAudioVideoBwe;  // googCombinedAudioVideoBwe
  LIB_WEBRTC_API static string
      kScreencastMinBitrate;  // googScreencastMinBitrate
  LIB_WEBRTC_API static string
      kCpuOveruseDetection;  // googCpuOveruseDetection

  // Specifies number of simulcast layers for all video tracks
  // with a Plan B offer/answer
  // (see RTCOfferAnswerOptions::num_simulcast_layers).
  LIB_WEBRTC_API static string kNumSimulcastLayers;

 public:
  LIB_WEBRTC_API static scoped_refptr<RTCMediaConstraints> Create();

  virtual void AddMandatoryConstraint(const string key, const string value) = 0;

  virtual void AddOptionalConstraint(const string key, const string value) = 0;

 protected:
  virtual ~RTCMediaConstraints() {}
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_MEDIA_CONSTRAINTS_HXX
