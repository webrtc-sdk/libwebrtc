#include "rtc_ice_candidate_impl.h"

namespace libwebrtc {

scoped_refptr<RTCIceCandidate> RTCIceCandidate::Create(const string sdp,
                                                       const string sdp_mid,
                                                       int sdp_mline_index,
                                                       SdpParseError* error) {
  webrtc::SdpParseError sdp_error;
  std::unique_ptr<webrtc::IceCandidateInterface> rtc_candidate(
      webrtc::CreateIceCandidate(to_std_string(sdp_mid), sdp_mline_index,
                                 to_std_string(sdp), &sdp_error));
  error->description = sdp_error.description;
  error->line = sdp_error.line;
  scoped_refptr<RTCIceCandidate> candidate = scoped_refptr<RTCIceCandidateImpl>(
      new RefCountedObject<RTCIceCandidateImpl>(std::move(rtc_candidate)));

  return candidate;
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

const string RTCIceCandidateImpl::sdp_mid() const {
  return sdp_mid_;
}

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