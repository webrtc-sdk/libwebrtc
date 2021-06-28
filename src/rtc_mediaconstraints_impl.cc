#include "rtc_mediaconstraints_impl.h"

namespace libwebrtc {

scoped_refptr<RTCMediaConstraints> RTCMediaConstraints::Create() {
  scoped_refptr<RTCMediaConstraintsImpl> constraints =
      scoped_refptr<RTCMediaConstraintsImpl>(
          new RefCountedObject<RTCMediaConstraintsImpl>());
  return constraints;
}

const char* RTCMediaConstraints::kGoogEchoCancellation = webrtc::MediaConstraints::kGoogEchoCancellation;
const char* RTCMediaConstraints::kAutoGainControl = webrtc::MediaConstraints::kAutoGainControl;
const char* RTCMediaConstraints::kExperimentalAutoGainControl = webrtc::MediaConstraints::kExperimentalAutoGainControl;
const char* RTCMediaConstraints::kNoiseSuppression = webrtc::MediaConstraints::kNoiseSuppression;
const char* RTCMediaConstraints::kExperimentalNoiseSuppression = webrtc::MediaConstraints::kExperimentalNoiseSuppression;
const char* RTCMediaConstraints::kHighpassFilter = webrtc::MediaConstraints::kHighpassFilter;
const char* RTCMediaConstraints::kTypingNoiseDetection = webrtc::MediaConstraints::kTypingNoiseDetection;
const char* RTCMediaConstraints::kAudioMirroring = webrtc::MediaConstraints::kAudioMirroring;
const char* RTCMediaConstraints::kAudioNetworkAdaptorConfig = webrtc::MediaConstraints::kAudioNetworkAdaptorConfig;
const char* RTCMediaConstraints::kOfferToReceiveVideo = webrtc::MediaConstraints::kOfferToReceiveVideo;
const char* RTCMediaConstraints::kOfferToReceiveAudio = webrtc::MediaConstraints::kOfferToReceiveAudio;
const char* RTCMediaConstraints::kVoiceActivityDetection = webrtc::MediaConstraints::kVoiceActivityDetection;
const char* RTCMediaConstraints::kIceRestart = webrtc::MediaConstraints::kIceRestart;
const char* RTCMediaConstraints::kUseRtpMux = webrtc::MediaConstraints::kUseRtpMux;
const char* RTCMediaConstraints::kValueTrue = webrtc::MediaConstraints::kValueTrue;
const char* RTCMediaConstraints::kValueFalse = webrtc::MediaConstraints::kValueFalse;
const char* RTCMediaConstraints::kEnableDtlsSrtp = webrtc::MediaConstraints::kEnableDtlsSrtp;
const char* RTCMediaConstraints::kEnableDscp = webrtc::MediaConstraints::kEnableDscp;
const char* RTCMediaConstraints::kEnableIPv6 = webrtc::MediaConstraints::kEnableIPv6;
const char* RTCMediaConstraints::kEnableVideoSuspendBelowMinBitrate = webrtc::MediaConstraints::kEnableVideoSuspendBelowMinBitrate;
const char* RTCMediaConstraints::kCombinedAudioVideoBwe = webrtc::MediaConstraints::kCombinedAudioVideoBwe;
const char* RTCMediaConstraints::kScreencastMinBitrate = webrtc::MediaConstraints::kScreencastMinBitrate;
const char* RTCMediaConstraints::kCpuOveruseDetection = webrtc::MediaConstraints::kCpuOveruseDetection;
const char* RTCMediaConstraints::kNumSimulcastLayers = webrtc::MediaConstraints::kNumSimulcastLayers;

void RTCMediaConstraintsImpl::AddMandatoryConstraint(const string key,
                                                     const string value) {
  webrtc::MediaConstraints::Constraint constraint(key.c_str(), value.c_str());
  mandatory_.push_back(constraint);
}
void RTCMediaConstraintsImpl::AddOptionalConstraint(const string key,
                                                    const string value) {
  webrtc::MediaConstraints::Constraint constraint(key.c_str(), value.c_str());
  optional_.push_back(constraint);
}
} // namespace libwebrtc
