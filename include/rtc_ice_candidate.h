#ifndef LIB_WEBRTC_RTC_ICE_CANDIDATE_HXX
#define LIB_WEBRTC_RTC_ICE_CANDIDATE_HXX

#include "rtc_types.h"
#include "rtc_sdp_parse_error.h"

namespace libwebrtc {

class RTCIceCandidate : public RefCountInterface {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCIceCandidate> Create(
      const string sdp, const string sdp_mid, int sdp_mline_index,
      RTCSdpParseError* error);

 public:
  virtual const string candidate() const = 0;

  virtual const string sdp_mid() const = 0;

  virtual int sdp_mline_index() const = 0;

  virtual bool ToString(string& out) = 0;

 protected:
  virtual ~RTCIceCandidate() {}
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_ICE_CANDIDATE_HXX
