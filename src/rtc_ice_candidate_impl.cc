#include "rtc_ice_candidate_impl.h"

namespace libwebrtc {

scoped_refptr<RTCIceCandidate> CreateRTCIceCandidate(const char* sdp,
                                                     const char* sdp_mid,
                                                     int sdp_mline_index,
                                                     SdpParseError* error) {
  webrtc::SdpParseError sdp_error;
  std::unique_ptr<webrtc::IceCandidateInterface> rtc_candidate(
      webrtc::CreateIceCandidate(sdp_mid, sdp_mline_index, sdp, &sdp_error));
  strncpy(error->description, sdp_error.description.c_str(),
          sdp_error.description.size());
  strncpy(error->line, sdp_error.line.c_str(), sdp_error.line.size());
  scoped_refptr<RTCIceCandidate> candidate = scoped_refptr<RTCIceCandidateImpl>(
      new RefCountedObject<RTCIceCandidateImpl>(std::move(rtc_candidate)));

  return candidate;
}

RTCIceCandidateImpl::RTCIceCandidateImpl(
    std::unique_ptr<webrtc::IceCandidateInterface> rtc_candidate)
    : candidate_(std::move(rtc_candidate)) {
  sdp_mid_ = candidate_->sdp_mid();
}

const char* RTCIceCandidateImpl::candidate() const {
  candidate_->ToString((std::string*)&sdp_);
  return sdp_.c_str();
}

const char* RTCIceCandidateImpl::sdp_mid() const {
  return sdp_mid_.c_str();
}

int RTCIceCandidateImpl::sdp_mline_index() const {
  return candidate_->sdp_mline_index();
}

bool RTCIceCandidateImpl::ToString(char* out, int length) {
  std::string tmp;
  if (candidate_->ToString(&tmp)) {
    strncpy(out, length, tmp.c_str(), tmp.size());
    return true;
  }
  return false;
}

} // namespace libwebrtc