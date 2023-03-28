#ifndef LIB_WEBRTC_RTC_ICE_CANDIDATE_IMPL_HXX
#define LIB_WEBRTC_RTC_ICE_CANDIDATE_IMPL_HXX

#include "api/jsep.h"
#include "rtc_ice_candidate.h"
#include "rtc_types.h"

namespace libwebrtc {

class RTCIceCandidateImpl : public RTCIceCandidate {
 public:
  RTCIceCandidateImpl(std::unique_ptr<webrtc::IceCandidateInterface> candidate);

  virtual const string candidate() const override;

  virtual const string sdp_mid() const override;

  virtual int sdp_mline_index() const override;

  virtual bool ToString(string& out) override;

  webrtc::IceCandidateInterface* candidate() { return candidate_.get(); }

 protected:
  virtual ~RTCIceCandidateImpl() {}

 private:
  std::unique_ptr<webrtc::IceCandidateInterface> candidate_;
  std::string sdp_;
  std::string sdp_mid_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_ICE_CANDIDATE_IMPL_HXX
