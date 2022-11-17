#include "gcm_frame_decryptor.h"

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <stddef.h>
#include <stdio.h>

#include <vector>

#include "rtc_base/logging.h"

namespace webrtc {

GCMFrameDecryptor::GCMFrameDecryptor() {
  cipher_ = std::make_unique<libwebrtc::Crypto::Cipher>(
      libwebrtc::Crypto::Algorithm::kAes128Gcm,
      libwebrtc::Crypto::Type::kDecrypt);
  cipher_->initialize();
  if (libwebrtc::Crypto::key.size() == 0 && libwebrtc::Crypto::iv.size() == 0) {
    libwebrtc::Crypto::generateAesKey(*cipher_.get(), libwebrtc::Crypto::key,
                                      libwebrtc::Crypto::iv);
  }
  cipher_->setkey(libwebrtc::Crypto::key, libwebrtc::Crypto::iv);
}

GCMFrameDecryptor::Result GCMFrameDecryptor::Decrypt(
    cricket::MediaType media_type,
    const std::vector<uint32_t>& csrcs,
    rtc::ArrayView<const uint8_t> additional_data,
    rtc::ArrayView<const uint8_t> encrypted_frame,
    rtc::ArrayView<uint8_t> frame) {

  if (encrypted_frame.size() == 0) {
    return Result(Status::kOk, encrypted_frame.size());
  }
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

  for (size_t i = 0; i < unencrypted_bytes; i++) {
    frame[i] = encrypted_frame[i];
  }

  for (size_t i = unencrypted_bytes; i < frame.size(); i++) {
    frame[i] =  encrypted_frame[i] ^ 0x55;
  }

  RTC_LOG(LS_INFO) << "FrameDecrypt, ssrc_size = " << int(csrcs.size())
                   << ", ssrc = " << (csrcs.size() > 1 ? csrcs[1] : -1)
                   << ", frame size = " << frame.size()
                   << ", encrypted_frame size = " << encrypted_frame.size()
                   << ", unencrypted_bytes = " << unencrypted_bytes;
  if (encrypted_frame.size() - unencrypted_bytes > 0) {
    return Result(Status::kOk, frame.size());
  } else {
    for (size_t i = 0; i < encrypted_frame.size(); i++) {
      frame[i] = encrypted_frame[i];
    }
    return Result(Status::kOk, encrypted_frame.size());
  }
}

size_t GCMFrameDecryptor::GetMaxPlaintextByteSize(cricket::MediaType media_type,
                                                  size_t encrypted_frame_size) {
  return encrypted_frame_size;
}

void GCMFrameDecryptor::SetKey(std::vector<uint8_t> key_bytes) {
 
}
}  // namespace webrtc