#include "rtc_session_description_impl.h"

namespace libwebrtc {

scoped_refptr<RTCSessionDescription> CreateRTCSessionDescription(
    const char* type,
    const char* sdp,
    SdpParseError* error) {
  webrtc::SdpParseError sdp_error;
  std::unique_ptr<webrtc::SessionDescriptionInterface> rtc_description(
      webrtc::CreateSessionDescription(type, sdp, &sdp_error));
  strncpy(error->description, sdp_error.description.c_str(),
          sdp_error.description.size());
  strncpy(error->line, sdp_error.line.c_str(), sdp_error.line.size());
  scoped_refptr<RTCSessionDescriptionImpl> session_description =
      scoped_refptr<RTCSessionDescriptionImpl>(
          new RefCountedObject<RTCSessionDescriptionImpl>(std::move(rtc_description)));
  return session_description;
}

RTCSessionDescriptionImpl::RTCSessionDescriptionImpl(
    std::unique_ptr<webrtc::SessionDescriptionInterface> description)
    : description_(std::move(description)) {}

const char* RTCSessionDescriptionImpl::sdp() const {
  description_->ToString((std::string*)&sdp_);
  return sdp_.c_str();
}

RTCSessionDescription::SdpType RTCSessionDescriptionImpl::GetType() {
  return (RTCSessionDescription::SdpType)description_->GetType();
}

const char* RTCSessionDescriptionImpl::type(){
  type_ = description_->type();
  return type_.c_str();
}

bool RTCSessionDescriptionImpl::ToString(char* out, int length) {
  std::string tmp;
  if (description_->ToString(&tmp)) {
    strncpy(out, tmp.c_str(), length);
    return true;
  }
  return false;
}

} // namespace libwebrtc