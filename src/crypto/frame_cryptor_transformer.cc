#include "frame_cryptor_transformer.h"
#include "api/array_view.h"
#include "rtc_base/logging.h"

#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#define CRYPTO_BUFFER_SIZE 65535
#define GCM_TAG_SIZE 16
#define IV_SIZE 12
#define KEY_SIZE 32

namespace libwebrtc {

uint8_t get_unencrypted_bytes(webrtc::TransformableFrameInterface* frame,
                              FrameCryptorTransformer::MediaType type);
size_t crypto_encrypt_buffer(const uint8_t* key,
                             const uint8_t* iv,
                             const uint8_t* in,
                             size_t inlen,
                             uint8_t* out,
                             size_t outlen);
size_t crypto_decrypt_buffer(const uint8_t* key,
                             const uint8_t* iv,
                             const uint8_t* in,
                             size_t inlen,
                             uint8_t* out,
                             size_t outlen);
std::string to_hex(unsigned char* data, int len);

FrameCryptorTransformer::FrameCryptorTransformer(MediaType type) : type_(type) {
  aesKey_ = {97,  145, 133, 203, 63,  197, 49,  232, 87,  159, 169,
             200, 59,  195, 77,  75,  150, 173, 189, 232, 44,  39,
             8,   149, 250, 6,   238, 170, 255, 17,  110, 107};
}

void FrameCryptorTransformer::RegisterTransformedFrameSinkCallback(
    rtc::scoped_refptr<webrtc::TransformedFrameCallback> callback,
    uint32_t ssrc) {
  webrtc::MutexLock lock(&mutex_);
  sink_callback_ = callback;
}

void FrameCryptorTransformer::UnregisterTransformedFrameSinkCallback(
    uint32_t ssrc) {
  webrtc::MutexLock lock(&mutex_);
  sink_callback_ = nullptr;
}

void FrameCryptorTransformer::RegisterTransformedFrameCallback(
    rtc::scoped_refptr<webrtc::TransformedFrameCallback> callback) {
  webrtc::MutexLock lock(&mutex_);
  sink_callback_ = callback;
}

void FrameCryptorTransformer::UnregisterTransformedFrameCallback() {
  webrtc::MutexLock lock(&mutex_);
  sink_callback_ = nullptr;
}

void FrameCryptorTransformer::Transform(
    std::unique_ptr<webrtc::TransformableFrameInterface> frame) {
  webrtc::MutexLock lock(&mutex_);
  if (sink_callback_ == nullptr)
    return;
  if (frame->GetData().size() == 0 && sink_callback_) {
    sink_callback_->OnTransformedFrame(std::move(frame));
    return;
  }
  // auto type_str = type_ == MediaType::kAudioFrame ? "audio" : "video";
  // do encrypt or decrypt here...
  switch (frame->GetDirection()) {
    case webrtc::TransformableFrameInterface::Direction::kSender:
      // do encrypt
      encryptFrame(std::move(frame));
      break;
    case webrtc::TransformableFrameInterface::Direction::kReceiver:
      // do decrypt
      decryptFrame(std::move(frame));
      break;
    case webrtc::TransformableFrameInterface::Direction::kUnknown:
      // do nothing
      RTC_LOG(LS_INFO) << "FrameCryptorTransformer::Transform() kUnknown";
      if (sink_callback_)
        sink_callback_->OnTransformedFrame(std::move(frame));
      break;
  }
}

void FrameCryptorTransformer::encryptFrame(
    std::unique_ptr<webrtc::TransformableFrameInterface> frame) {
  uint8_t unencrypted_bytes = get_unencrypted_bytes(frame.get(), type_);

  rtc::ArrayView<const uint8_t> date_in = frame->GetData();

  rtc::Buffer frameHeader(unencrypted_bytes);
  for (size_t i = 0; i < unencrypted_bytes; i++) {
    frameHeader[i] = date_in[i];
  }

  // TODO:
  uint8_t keyIndex = 0;
  rtc::Buffer frameTrailer(2);
  frameTrailer[0] = IV_SIZE;
  frameTrailer[1] = keyIndex;
  auto iv = makeIv(frame->GetSsrc(), frame->GetTimestamp());

  rtc::Buffer payload(date_in.size() - unencrypted_bytes);
  for (size_t i = unencrypted_bytes; i < date_in.size(); i++) {
    payload[i - unencrypted_bytes] = date_in[i];
  }

  uint8_t encbuffer[CRYPTO_BUFFER_SIZE];
  size_t enclen =
      crypto_encrypt_buffer(aesKey_.data(), iv.data(), payload.data(),
                            payload.size(), encbuffer, sizeof(encbuffer));

  rtc::Buffer encrypted_payload(encbuffer, enclen);
  rtc::Buffer data_out;
  data_out.AppendData(frameHeader);
  data_out.AppendData(encrypted_payload);
  data_out.AppendData(iv);
  data_out.AppendData(frameTrailer);
  frame->SetData(data_out);

  RTC_LOG(LS_INFO) << "FrameCryptorTransformer::encryptFrame() ivLength="
                   << static_cast<int>(iv.size())
                   << " keyIndex=" << static_cast<int>(keyIndex)
                   << " aesKey=" << to_hex(aesKey_.data(), aesKey_.size())
                   << " iv=" << to_hex(iv.data(), iv.size());

  if (sink_callback_)
    sink_callback_->OnTransformedFrame(std::move(frame));
}

void FrameCryptorTransformer::decryptFrame(
    std::unique_ptr<webrtc::TransformableFrameInterface> frame) {
  uint8_t unencrypted_bytes = get_unencrypted_bytes(frame.get(), type_);
  rtc::ArrayView<const uint8_t> date_in = frame->GetData();
  rtc::Buffer frameHeader(unencrypted_bytes);
  for (size_t i = 0; i < unencrypted_bytes; i++) {
    frameHeader[i] = date_in[i];
  }

  rtc::Buffer frameTrailer(2);
  frameTrailer[0] = date_in[date_in.size() - 2];
  frameTrailer[1] = date_in[date_in.size() - 1];
  uint8_t ivLength = frameTrailer[0];
  uint8_t keyIndex = frameTrailer[1];

  rtc::Buffer iv = rtc::Buffer(ivLength);
  for (size_t i = 0; i < ivLength; i++) {
    iv[i] = date_in[date_in.size() - 2 - ivLength + i];
  }

  rtc::Buffer encrypted_payload(date_in.size() - unencrypted_bytes - ivLength -
                                2);
  for (size_t i = unencrypted_bytes; i < date_in.size() - ivLength - 2; i++) {
    encrypted_payload[i - unencrypted_bytes] = date_in[i];
  }

  uint8_t decbuffer[CRYPTO_BUFFER_SIZE];
  size_t declen = crypto_decrypt_buffer(
      aesKey_.data(), iv.data(), encrypted_payload.data(),
      encrypted_payload.size(), decbuffer, sizeof decbuffer);

  rtc::Buffer payload(decbuffer, declen);
  rtc::Buffer data_out;
  data_out.AppendData(frameHeader);
  data_out.AppendData(payload);
  frame->SetData(data_out);

  RTC_LOG(LS_INFO) << "FrameCryptorTransformer::decryptFrame() ivLength="
                   << static_cast<int>(ivLength)
                   << " keyIndex=" << static_cast<int>(keyIndex)
                   << " aesKey=" << to_hex(aesKey_.data(), aesKey_.size())
                   << " iv=" << to_hex(iv.data(), iv.size());

  if (sink_callback_)
    sink_callback_->OnTransformedFrame(std::move(frame));
}

rtc::Buffer FrameCryptorTransformer::makeIv(uint32_t ssrc, uint32_t timestamp) {
  rtc::Buffer iv = rtc::Buffer(IV_SIZE);
  rtc::ArrayView<uint32_t> ivView = rtc::ArrayView<uint32_t>(
      reinterpret_cast<uint32_t*>(iv.data()), IV_SIZE / 4);
  if (!sendCounts_[ssrc]) {
    // TOOD:
    // sendCounts_[ssrc] = Math.floor(Math.random() * 0xFFFF);
  }
  uint32_t sendCount = sendCounts_[ssrc];
  ivView[0] = ssrc;
  ivView[1] = timestamp;
  ivView[2] = sendCount % 0xFFFF;
  sendCounts_[ssrc] = sendCount + 1;
  return iv;
}

uint8_t get_unencrypted_bytes(webrtc::TransformableFrameInterface* frame,
                              FrameCryptorTransformer::MediaType type) {
  switch (type) {
    case FrameCryptorTransformer::MediaType::kAudioFrame:
      return 1;
    case FrameCryptorTransformer::MediaType::kVideoFrame: {
      auto videoFrame =
          static_cast<webrtc::TransformableVideoFrameInterface*>(frame);
      return videoFrame->IsKeyFrame() ? 10 : 3;
    }
    default:
      return 0;
  }
}

size_t crypto_encrypt_buffer(const uint8_t* key,
                             const uint8_t* iv,
                             const uint8_t* in,
                             size_t inlen,
                             uint8_t* out,
                             size_t outlen) {
  EVP_CIPHER_CTX* ctx;
  uint8_t tag[GCM_TAG_SIZE];
  int olen;
  int len = 0;
  int ret = 0;

  /* output buffer does not have enough room */
  if (outlen < inlen + sizeof tag + sizeof iv)
    return 0;

  ctx = EVP_CIPHER_CTX_new();
  if (ctx == NULL)
    return 0;

  EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv);

  /* encrypt buffer */
  if (!EVP_EncryptUpdate(ctx, out + len, &olen, in, inlen))
    goto end;
  len += olen;

  /* finalize and write last chunk if any */
  if (!EVP_EncryptFinal(ctx, out + len, &olen))
    goto end;
  len += olen;

  /* get and append tag */
  EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, sizeof tag, tag);
  memcpy(out + len, tag, sizeof tag);
  ret = len + sizeof tag;

end:
  EVP_CIPHER_CTX_free(ctx);
  return ret;
}

size_t crypto_decrypt_buffer(const uint8_t* key,
                             const uint8_t* iv,
                             const uint8_t* in,
                             size_t inlen,
                             uint8_t* out,
                             size_t outlen) {
  EVP_CIPHER_CTX* ctx;
  uint8_t tag[GCM_TAG_SIZE];
  int olen;
  int len = 0;
  int ret = 0;

  /* out does not have enough room */
  if (outlen < inlen - sizeof tag)
    return 0;

  /* extract tag */
  memcpy(tag, in + inlen - sizeof tag, sizeof tag);
  inlen -= sizeof tag;

  ctx = EVP_CIPHER_CTX_new();
  if (ctx == NULL)
    return 0;

  EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv);

  /* set expected tag */
  EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, sizeof tag, tag);

  /* decrypt buffer */
  if (!EVP_DecryptUpdate(ctx, out, &olen, in, inlen))
    goto end;
  len += olen;

  /* finalize, write last chunk if any and perform authentication check */
  if (!EVP_DecryptFinal_ex(ctx, out + len, &olen))
    goto end;
  ret = len + olen;

end:
  EVP_CIPHER_CTX_free(ctx);
  return ret;
}

std::string to_hex(unsigned char* data, int len) {
  std::stringstream ss;
  ss << std::uppercase << std::hex << std::setfill('0');
  for (int i = 0; i < len; i++) {
    ss << std::setw(2) << static_cast<unsigned>(data[i]);
  }
  return ss.str();
}

}  // namespace libwebrtc