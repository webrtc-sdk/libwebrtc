#include "rtc_frame_decryptor_impl.h"

#include "rtc_base/ref_counted_object.h"

namespace libwebrtc {

scoped_refptr<RTCFrameDecryptor> RTCFrameDecryptor::Create(
    DecryptCallback decrypt,
    GetMaxPlaintextByteSizeCallback get_max_plaintext_byte_size) {
  return new RefCountedObject<RTCFrameDecryptorImpl>(
      decrypt, get_max_plaintext_byte_size);
}

RTCFrameDecryptorImpl::RTCFrameDecryptorImpl(
    DecryptCallback decrypt,
    GetMaxPlaintextByteSizeCallback get_max_plaintext_byte_size) {
  frame_decryptor_ = new rtc::RefCountedObject<RTCFrameDecryptorInternal>(
      decrypt, get_max_plaintext_byte_size);
}

RTCFrameDecryptorImpl::~RTCFrameDecryptorImpl() {}
}  // namespace libwebrtc
