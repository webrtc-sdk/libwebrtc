#ifndef LIBWEBRTC_RTC_STRING_H
#define LIBWEBRTC_RTC_STRING_H

#include "rtc_types.h"
#include "base/refcount.h"
#include "base/scoped_ref_ptr.h"

namespace libwebrtc {
class RTCString : public RefCountInterface {
 public:
 LIB_WEBRTC_API static scoped_refptr<RTCString> Create(char* data, size_t size);

  virtual size_t Copy(char* data, size_t size) = 0;

  virtual const char* Data() = 0;
  virtual size_t Size() = 0;
};
}  // namespace libwebrtc
#endif  // LIBWEBRTC_RTC_STRING_H
