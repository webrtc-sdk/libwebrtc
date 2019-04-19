#include "rtc_mediaconstraints_impl.h"

namespace libwebrtc {

scoped_refptr<RTCMediaConstraints> RTCMediaConstraints::Create() {
  scoped_refptr<RTCMediaConstraintsImpl> constraints =
      scoped_refptr<RTCMediaConstraintsImpl>(
          new RefCountedObject<RTCMediaConstraintsImpl>());
  return constraints;
}

const char* RTCMediaConstraints::kMinAspectRatio =
    webrtc::MediaConstraintsInterface::kMinAspectRatio;
const char* RTCMediaConstraints::kMaxAspectRatio =
    webrtc::MediaConstraintsInterface::kMaxAspectRatio;
const char* RTCMediaConstraints::kMinWidth =
    webrtc::MediaConstraintsInterface::kMinWidth;
const char* RTCMediaConstraints::kMaxWidth =
    webrtc::MediaConstraintsInterface::kMaxWidth;
const char* RTCMediaConstraints::kMinHeight =
    webrtc::MediaConstraintsInterface::kMinHeight;
const char* RTCMediaConstraints::kMaxHeight =
    webrtc::MediaConstraintsInterface::kMaxHeight;
const char* RTCMediaConstraints::kMinFrameRate =
    webrtc::MediaConstraintsInterface::kMinFrameRate;
const char* RTCMediaConstraints::kMaxFrameRate =
    webrtc::MediaConstraintsInterface::kMaxFrameRate;
const char* RTCMediaConstraints::kAudioNetworkAdaptorConfig =
    webrtc::MediaConstraintsInterface::kAudioNetworkAdaptorConfig;

const char* RTCMediaConstraints::kIceRestart =
    webrtc::MediaConstraintsInterface::kIceRestart;
const char* RTCMediaConstraints::kOfferToReceiveAudio =
    webrtc::MediaConstraintsInterface::kOfferToReceiveAudio;
const char* RTCMediaConstraints::kOfferToReceiveVideo =
    webrtc::MediaConstraintsInterface::kOfferToReceiveVideo;
const char* RTCMediaConstraints::kVoiceActivityDetection =
    webrtc::MediaConstraintsInterface::kVoiceActivityDetection;

const char* RTCMediaConstraints::kValueTrue =
    webrtc::MediaConstraintsInterface::kValueTrue;
const char* RTCMediaConstraints::kValueFalse =
    webrtc::MediaConstraintsInterface::kValueFalse;

void RTCMediaConstraintsImpl::AddMandatoryConstraint(const char* key,
                                                     const char* value) {
  webrtc::MediaConstraintsInterface::Constraint constraint(key,value);
  mandatory_.push_back(constraint);
}
void RTCMediaConstraintsImpl::AddOptionalConstraint(const char* key,
                                                    const char* value) {
  webrtc::MediaConstraintsInterface::Constraint constraint(key, value);
  optional_.push_back(constraint);
}
};  // namespace libwebrtc
