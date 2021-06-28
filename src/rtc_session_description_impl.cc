#include "rtc_session_description_impl.h"

namespace libwebrtc {

scoped_refptr<RTCSessionDescription> RTCSessionDescription::Create(
    const string type,
    const string sdp,
    SdpParseError* error) {
  webrtc::SdpParseError sdp_error;
  std::unique_ptr<webrtc::SessionDescriptionInterface> rtc_description(
      webrtc::CreateSessionDescription(type.c_str(), sdp.c_str(), &sdp_error));
  error->description = sdp_error.description.c_str();
  error->line = sdp_error.line.c_str();
  scoped_refptr<RTCSessionDescriptionImpl> session_description =
      scoped_refptr<RTCSessionDescriptionImpl>(
          new RefCountedObject<RTCSessionDescriptionImpl>(
              std::move(rtc_description)));
  return session_description;
}

RTCSessionDescriptionImpl::RTCSessionDescriptionImpl(
    std::unique_ptr<webrtc::SessionDescriptionInterface> description)
    : description_(std::move(description)) {}

const string RTCSessionDescriptionImpl::sdp() const {
  description_->ToString((std::string*)&sdp_);
  return sdp_.c_str();
}

RTCSessionDescription::SdpType RTCSessionDescriptionImpl::GetType() {
  return (RTCSessionDescription::SdpType)description_->GetType();
}

const string RTCSessionDescriptionImpl::type() {
  type_ = description_->type();
  return type_.c_str();
}

bool RTCSessionDescriptionImpl::ToString(string& out) {
  std::string tmp;
  if (description_->ToString(&tmp)) {
    out = tmp.c_str();
    return true;
  }
  return false;
}

}  // namespace libwebrtc