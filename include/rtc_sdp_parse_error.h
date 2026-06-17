#ifndef LIB_WEBRTC_RTC_SDP_PARSE_ERROR_HXX
#define LIB_WEBRTC_RTC_SDP_PARSE_ERROR_HXX

#include "rtc_types.h"

namespace libwebrtc {

class RTCSdpParseError : public RefCountInterface {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCSdpParseError> Create();

 public:
  // The sdp line that causes the error.
  virtual const string line() const = 0;
  virtual void set_line(const string value) = 0;
  
  // Explains the error.
  virtual const string description() const = 0;
  virtual void set_description(const string value) = 0;

 protected:
  virtual ~RTCSdpParseError() {}
};

} // namespace libwebrtc

#endif // LIB_WEBRTC_RTC_SDP_PARSE_ERROR_HXX
