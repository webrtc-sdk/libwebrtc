#include "gcm_frame_encryptor.h"

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <stdio.h>

#include "rtc_base/logging.h"

namespace webrtc {

GCMFrameEncryptor::GCMFrameEncryptor() {
  cipher_ = std::make_unique<libwebrtc::Crypto::Cipher>(
      libwebrtc::Crypto::Algorithm::kAes128Gcm,
      libwebrtc::Crypto::Type::kEncrypt);
  cipher_->initialize();
  if (libwebrtc::Crypto::key.size() == 0 && libwebrtc::Crypto::iv.size() == 0) {
    libwebrtc::Crypto::generateAesKey(*cipher_.get(), libwebrtc::Crypto::key,
                                      libwebrtc::Crypto::iv);
  }
  cipher_->setkey(libwebrtc::Crypto::key, libwebrtc::Crypto::iv);
}

int GCMFrameEncryptor::Encrypt(cricket::MediaType media_type,
                               uint32_t ssrc,
                               rtc::ArrayView<const uint8_t> additional_data,
                               rtc::ArrayView<const uint8_t> frame,
                               rtc::ArrayView<uint8_t> encrypted_frame,
                               size_t* bytes_written) {
  uint8_t unencrypted_bytes = 1;
  switch (media_type) {
    case cricket::MEDIA_TYPE_AUDIO:
      unencrypted_bytes = 1;
      break;
    case cricket::MEDIA_TYPE_VIDEO:
      unencrypted_bytes = 10;
      break;
    case cricket::MEDIA_TYPE_DATA:
      break;
    case cricket::MEDIA_TYPE_UNSUPPORTED:
      break;
  }

  /*
  unencrypted_bytes {
  key: 10,
  delta: 3,
  audio: 1, // frame.type is not set on audio, so this is set manually
  empty: 0
  }
  */

  for (size_t i = 0; i < unencrypted_bytes; i++) {
    encrypted_frame[i] = frame[i];
  }

  for (size_t i = unencrypted_bytes; i < frame.size(); i++) {
    encrypted_frame[i] =  frame[i] ^ 0x55;
  }

  RTC_LOG(LS_INFO) << "FrameEncrypt, ssrc = " << ssrc << ", frame size = " << frame.size() << ", encrypted_frame size = " << encrypted_frame.size() << ", unencrypted_bytes = " << unencrypted_bytes;
  *bytes_written = encrypted_frame.size();
  return 0;
}

size_t GCMFrameEncryptor::GetMaxCiphertextByteSize(
    cricket::MediaType media_type,
    size_t frame_size) {
  return frame_size;
}

void GCMFrameEncryptor::SetKey(std::vector<uint8_t> key_bytes) {

}
}  // namespace webrtc
