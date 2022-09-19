#include "rtc_frame_encryptor_impl.h"

#include "rtc_base/ref_counted_object.h"

namespace libwebrtc {

scoped_refptr<RTCFrameEncryptor> RTCFrameEncryptor::Create(
    EncryptCallback encrypt,
    GetMaxCiphertextByteSizeCallback get_max_ciphertext_byte_size) {
  return new RefCountedObject<RTCFrameEncryptorImpl>(
      encrypt, get_max_ciphertext_byte_size);
}

RTCFrameEncryptorImpl::RTCFrameEncryptorImpl(
    EncryptCallback encrypt,
    GetMaxCiphertextByteSizeCallback get_max_ciphertext_byte_size) {
  frame_encryptor_ = new rtc::RefCountedObject<RTCFrameEncryptorInternal>(
      encrypt, get_max_ciphertext_byte_size);
}

RTCFrameEncryptorImpl::~RTCFrameEncryptorImpl() {}

}  // namespace libwebrtc
