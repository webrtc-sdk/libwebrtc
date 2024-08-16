#include "rtc_mediaconstraints_impl.h"

namespace libwebrtc {

scoped_refptr<RTCMediaConstraints> RTCMediaConstraints::Create() {
  scoped_refptr<RTCMediaConstraintsImpl> constraints =
      scoped_refptr<RTCMediaConstraintsImpl>(
          new RefCountedObject<RTCMediaConstraintsImpl>());
  return constraints;
}

string RTCMediaConstraints::kDAEchoCancellation = "googDAEchoCancellation";

string RTCMediaConstraints::kGoogEchoCancellation =
    webrtc::MediaConstraints::kGoogEchoCancellation;
string RTCMediaConstraints::kAutoGainControl =
    webrtc::MediaConstraints::kAutoGainControl;
string RTCMediaConstraints::kNoiseSuppression =
    webrtc::MediaConstraints::kNoiseSuppression;
string RTCMediaConstraints::kHighpassFilter =
    webrtc::MediaConstraints::kHighpassFilter;
string RTCMediaConstraints::kAudioMirroring =
    webrtc::MediaConstraints::kAudioMirroring;
string RTCMediaConstraints::kAudioNetworkAdaptorConfig =
    webrtc::MediaConstraints::kAudioNetworkAdaptorConfig;
string RTCMediaConstraints::kOfferToReceiveVideo =
    webrtc::MediaConstraints::kOfferToReceiveVideo;
string RTCMediaConstraints::kOfferToReceiveAudio =
    webrtc::MediaConstraints::kOfferToReceiveAudio;
string RTCMediaConstraints::kVoiceActivityDetection =
    webrtc::MediaConstraints::kVoiceActivityDetection;
string RTCMediaConstraints::kIceRestart =
    webrtc::MediaConstraints::kIceRestart;
string RTCMediaConstraints::kUseRtpMux =
    webrtc::MediaConstraints::kUseRtpMux;
string RTCMediaConstraints::kValueTrue =
    webrtc::MediaConstraints::kValueTrue;
string RTCMediaConstraints::kValueFalse =
    webrtc::MediaConstraints::kValueFalse;
string RTCMediaConstraints::kEnableDscp =
    webrtc::MediaConstraints::kEnableDscp;
string RTCMediaConstraints::kEnableIPv6 = "enable_ipv6";
string RTCMediaConstraints::kEnableVideoSuspendBelowMinBitrate =
    webrtc::MediaConstraints::kEnableVideoSuspendBelowMinBitrate;
//string RTCMediaConstraints::kCombinedAudioVideoBwe =
//    webrtc::MediaConstraints::kCombinedAudioVideoBwe;
string RTCMediaConstraints::kScreencastMinBitrate =
    webrtc::MediaConstraints::kScreencastMinBitrate;
string RTCMediaConstraints::kCpuOveruseDetection =
    webrtc::MediaConstraints::kCpuOveruseDetection;
string RTCMediaConstraints::kNumSimulcastLayers =
    webrtc::MediaConstraints::kNumSimulcastLayers;

void RTCMediaConstraintsImpl::AddMandatoryConstraint(const string key,
                                                     const string value) {
  webrtc::MediaConstraints::Constraint constraint(to_std_string(key),
                                                  to_std_string(value));
  mandatory_.push_back(constraint);
}
void RTCMediaConstraintsImpl::AddOptionalConstraint(const string key,
                                                    const string value) {
  webrtc::MediaConstraints::Constraint constraint(to_std_string(key),
                                                  to_std_string(value));
  optional_.push_back(constraint);
}
}  // namespace libwebrtc
