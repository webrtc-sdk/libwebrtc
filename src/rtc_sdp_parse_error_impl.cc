#include "rtc_sdp_parse_error_impl.h"

namespace libwebrtc {

scoped_refptr<RTCSdpParseError> RTCSdpParseError::Create() {
  return scoped_refptr<RTCSdpParseErrorImpl>(
    new RefCountedObject<RTCSdpParseErrorImpl>()); 
}

RTCSdpParseErrorImpl::RTCSdpParseErrorImpl()
 : line_(), description_() {
}

const string RTCSdpParseErrorImpl::line() const {
  return line_;
}

void RTCSdpParseErrorImpl::set_line(const string value) {
  line_ = value.std_string();
}

const string RTCSdpParseErrorImpl::description() const {
  return description_;
}

void RTCSdpParseErrorImpl::set_description(const string value) {
  description_ = value.std_string();
}

}  // namespace libwebrtc