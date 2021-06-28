#ifndef LIB_WEBRTC_RTC_ICE_CANDIDATE_HXX
#define LIB_WEBRTC_RTC_ICE_CANDIDATE_HXX

#include "rtc_types.h"

namespace libwebrtc {

class RTCIceCandidate : public RefCountInterface {
 public:
  virtual const string candidate() const = 0;

  virtual const string sdp_mid() const = 0;

  virtual int sdp_mline_index() const = 0;

  virtual bool ToString(string& out) = 0;

 protected:
  virtual ~RTCIceCandidate() {}
};

LIB_WEBRTC_API scoped_refptr<RTCIceCandidate> CreateRTCIceCandidate(
    const string sdp,
    const string sdp_mid,
    int sdp_mline_index,
    SdpParseError* error);

} // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_ICE_CANDIDATE_HXX
