#ifndef LIB_WEBRTC_RTC_SDP_PARSE_ERROR_IMPL_HXX
#define LIB_WEBRTC_RTC_SDP_PARSE_ERROR_IMPL_HXX

#include "rtc_sdp_parse_error.h"
#include "rtc_types.h"

namespace libwebrtc {

class RTCSdpParseErrorImpl : public RTCSdpParseError {
 public:
  RTCSdpParseErrorImpl();

  virtual const string line() const override;
  virtual void set_line(const string value) override;

  virtual const string description() const override;
  virtual void set_description(const string value) override;

 protected:
  virtual ~RTCSdpParseErrorImpl() {}

 private:
   std::string line_; // The sdp line that causes the error.
   std::string description_; // Explains the error.
};

} // namespace libwebrtc

#endif // LIB_WEBRTC_RTC_SDP_PARSE_ERROR_IMPL_HXX