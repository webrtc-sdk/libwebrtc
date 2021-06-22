#include "rtc_string_impl.h"
#include "rtc_types.h"

namespace libwebrtc {
LIB_WEBRTC_API scoped_refptr<RTCString> RTCString::Create(char* data, size_t size) {
  return new RefCountedObject<RTCStringImpl>(data, size);
}

RTCStringImpl::RTCStringImpl(char* data, size_t size) {
  data_ = (char*)malloc(size + 1);
  size_ = size;
  strncpy(data_, size_ + 1, data, size);
}

RTCStringImpl::~RTCStringImpl() {
  if (nullptr != data_) {
    free(data_);
    data_ = nullptr;
  }
  size_ = 0;
}

size_t RTCStringImpl::Copy(char* data, size_t size) {
  return strncpy(data, size, data_, size - 1);
  ;
}

const char* RTCStringImpl::Data() {
  return data_;
}

size_t RTCStringImpl::Size() {
  return size_;
}
}  // namespace libwebrtc