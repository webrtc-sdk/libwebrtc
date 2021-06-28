#include "rtc_ice_candidate_impl.h"

namespace libwebrtc {

scoped_refptr<RTCIceCandidate> CreateRTCIceCandidate(const string sdp,
                                                     const string sdp_mid,
                                                     int sdp_mline_index,
                                                     SdpParseError* error) {
  webrtc::SdpParseError sdp_error;
  std::unique_ptr<webrtc::IceCandidateInterface> rtc_candidate(
      webrtc::CreateIceCandidate(sdp_mid.c_str(), sdp_mline_index, sdp.c_str(),
                                 &sdp_error));
  error->description = sdp_error.description.c_str();
  error->line = sdp_error.line.c_str();
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
  return sdp_.c_str();
}

const string RTCIceCandidateImpl::sdp_mid() const {
  return sdp_mid_.c_str();
}

int RTCIceCandidateImpl::sdp_mline_index() const {
  return candidate_->sdp_mline_index();
}

bool RTCIceCandidateImpl::ToString(string& out) {
  std::string tmp;
  if (candidate_->ToString(&tmp)) {
    out = tmp.c_str();
    return true;
  }
  return false;
}

}  // namespace libwebrtc