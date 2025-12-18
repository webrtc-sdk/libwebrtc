#include "rtc_session_description_impl.h"
#include "rtc_sdp_parse_error.h"
#include "api/jsep.h"

namespace libwebrtc {

scoped_refptr<RTCSessionDescription> RTCSessionDescription::Create(
    const string type, const string sdp, RTCSdpParseError* error) {
  webrtc::SdpParseError sdp_error;
  std::optional<webrtc::SdpType> maybe_type = webrtc::SdpTypeFromString(to_std_string(type));
  if (!maybe_type) {
    return nullptr;
  }
  std::unique_ptr<webrtc::SessionDescriptionInterface> rtc_description(
      webrtc::CreateSessionDescription(*maybe_type, to_std_string(sdp),
                                       &sdp_error));
  if (error) {
    error->set_description(sdp_error.description);
    error->set_line(sdp_error.line);
  }                          
  if (rtc_description) {
    return scoped_refptr<RTCSessionDescriptionImpl>(
        new RefCountedObject<RTCSessionDescriptionImpl>(
            std::move(rtc_description)));
  }

  return nullptr;
}

RTCSessionDescriptionImpl::RTCSessionDescriptionImpl(
    std::unique_ptr<webrtc::SessionDescriptionInterface> description)
    : description_(std::move(description)) {}

const string RTCSessionDescriptionImpl::sdp() const {
  description_->ToString((std::string*)&sdp_);
  return sdp_;
}

RTCSessionDescription::SdpType RTCSessionDescriptionImpl::GetType() {
  return (RTCSessionDescription::SdpType)description_->GetType();
}

const string RTCSessionDescriptionImpl::type() {
  type_ = description_->type();
  return type_;
}

bool RTCSessionDescriptionImpl::ToString(string& out) {
  std::string tmp;
  if (description_->ToString(&tmp)) {
    out = tmp;
    return true;
  }
  return false;
}

}  // namespace libwebrtc