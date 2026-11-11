#include "rtc_ice_candidate_impl.h"
#include "rtc_sdp_parse_error.h"

namespace libwebrtc {

scoped_refptr<RTCIceCandidate> RTCIceCandidate::Create(const string sdp,
                                                       const string sdp_mid,
                                                       int sdp_mline_index,
                                                       RTCSdpParseError* error) {
  webrtc::SdpParseError sdp_error;
  std::unique_ptr<webrtc::IceCandidateInterface> rtc_candidate(
      webrtc::CreateIceCandidate(to_std_string(sdp_mid), sdp_mline_index,
                                 to_std_string(sdp), &sdp_error));
  if (error) {
    error->set_description(sdp_error.description);
    error->set_line(sdp_error.line);
  }
  if (rtc_candidate) {
    return scoped_refptr<RTCIceCandidateImpl>(
        new RefCountedObject<RTCIceCandidateImpl>(std::move(rtc_candidate)));
  }

  return nullptr;
}

RTCIceCandidateImpl::RTCIceCandidateImpl(
    std::unique_ptr<webrtc::IceCandidateInterface> rtc_candidate)
    : candidate_(std::move(rtc_candidate)) {
  sdp_mid_ = candidate_->sdp_mid();
}

const string RTCIceCandidateImpl::candidate() const {
  candidate_->ToString((std::string*)&sdp_);
  return sdp_;
}

const string RTCIceCandidateImpl::sdp_mid() const { return sdp_mid_; }

int RTCIceCandidateImpl::sdp_mline_index() const {
  return candidate_->sdp_mline_index();
}

bool RTCIceCandidateImpl::ToString(string& out) {
  std::string tmp;
  if (candidate_->ToString(&tmp)) {
    out = tmp;
    return true;
  }
  return false;
}

}  // namespace libwebrtc