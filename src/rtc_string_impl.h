#ifndef LIBWEBRTC_RTC_STRING_IMPL_H
#define LIBWEBRTC_RTC_STRING_IMPL_H

#include "rtc_string.h"

namespace libwebrtc {
class RTCStringImpl : public RTCString {
 public:
  RTCStringImpl(char* data, size_t size);
  ~RTCStringImpl();

  virtual size_t Copy(char* data, size_t size) override;
  virtual const char* Data() override;
  virtual size_t Size() override;

 private:
  char* data_;
  size_t size_;
};
}



#endif //LIBWEBRTC_RTC_STRING_IMPL_H
