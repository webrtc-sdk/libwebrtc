#include "frame_cryptor_transformer.h"
#include "api/array_view.h"
#include "rtc_base/byte_buffer.h"
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

#define IV_SIZE 12

enum class EncryptOrDecrypt { kEncrypt = 0, kDecrypt };

#define Success 0
#define ErrorUnexpected -1
#define OperationError -2
#define ErrorDataTooSmall -3
#define ErrorInvalidAesGcmTagLength -4

const EVP_AEAD* GetAesGcmAlgorithmFromKeySize(size_t key_size_bytes) {
  switch (key_size_bytes) {
    case 16:
      return EVP_aead_aes_128_gcm();
    case 32:
      return EVP_aead_aes_256_gcm();
    default:
      return nullptr;
  }
}

int AeadEncryptDecrypt(EncryptOrDecrypt mode,
                       const std::vector<uint8_t> raw_key,
                       const rtc::ArrayView<uint8_t> data,
                       unsigned int tag_length_bytes,
                       rtc::ArrayView<uint8_t> iv,
                       rtc::ArrayView<uint8_t> additional_data,
                       const EVP_AEAD* aead_alg,
                       std::vector<uint8_t>* buffer) {
  bssl::ScopedEVP_AEAD_CTX ctx;

  if (!aead_alg)
    return ErrorUnexpected;

  if (!EVP_AEAD_CTX_init(ctx.get(), aead_alg, raw_key.data(), raw_key.size(),
                         tag_length_bytes, nullptr)) {
    return OperationError;
  }

  size_t len;
  int ok;

  if (mode == EncryptOrDecrypt::kDecrypt) {
    if (data.size() < tag_length_bytes)
      return ErrorDataTooSmall;

    buffer->resize(data.size() - tag_length_bytes);

    ok = EVP_AEAD_CTX_open(ctx.get(), buffer->data(), &len, buffer->size(),
                           iv.data(), iv.size(), data.data(), data.size(),
                           additional_data.data(), additional_data.size());
  } else {
    buffer->resize(data.size() + EVP_AEAD_max_overhead(aead_alg));

    ok = EVP_AEAD_CTX_seal(ctx.get(), buffer->data(), &len, buffer->size(),
                           iv.data(), iv.size(), data.data(), data.size(),
                           additional_data.data(), additional_data.size());
  }

  if (!ok)
    return OperationError;
  buffer->resize(len);

  return Success;
}

int AesGcmEncryptDecrypt(EncryptOrDecrypt mode,
                         const std::vector<uint8_t>& raw_key,
                         rtc::ArrayView<uint8_t> iv,
                         rtc::ArrayView<uint8_t> additional_data,
                         const rtc::ArrayView<uint8_t> data,
                         std::vector<uint8_t>* buffer) {
  unsigned int tag_length_bits = 128;
  return AeadEncryptDecrypt(
      mode, raw_key, data, tag_length_bits / 8, iv, additional_data,
      GetAesGcmAlgorithmFromKeySize(raw_key.size()), buffer);
}

namespace libwebrtc {

uint8_t get_unencrypted_bytes(webrtc::TransformableFrameInterface* frame,
                              FrameCryptorTransformer::MediaType type);
std::string to_hex(unsigned char* data, int len);

FrameCryptorTransformer::FrameCryptorTransformer(MediaType type) : type_(type) {
  aesKey_.resize(32);
}

void FrameCryptorTransformer::SetKey(const std::vector<uint8_t>& key) {
  if (key.size() != 16 && key.size() != 32) {
    RTC_LOG(LS_ERROR) << "key size must be 16 or 32 bytes";
    return;
  }
  aesKey_ = key;
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
  rtc::Buffer iv = makeIv(frame->GetSsrc(), frame->GetTimestamp());

  rtc::Buffer payload(date_in.size() - unencrypted_bytes);
  for (size_t i = unencrypted_bytes; i < date_in.size(); i++) {
    payload[i - unencrypted_bytes] = date_in[i];
  }

  std::vector<uint8_t> buffer;
  AesGcmEncryptDecrypt(EncryptOrDecrypt::kEncrypt, aesKey_, iv, frameHeader,
                       payload, &buffer);

  rtc::Buffer encrypted_payload(buffer.data(), buffer.size());
  rtc::Buffer data_out;
  data_out.AppendData(frameHeader);
  data_out.AppendData(encrypted_payload);
  data_out.AppendData(iv);
  data_out.AppendData(frameTrailer);

  RTC_CHECK_EQ(data_out.size(), frameHeader.size() + encrypted_payload.size() +
                                    iv.size() + frameTrailer.size());

  frame->SetData(data_out);

  RTC_LOG(LS_INFO) << "FrameCryptorTransformer::encryptFrame() ivLength="
                   << static_cast<int>(iv.size())
                   << " unencrypted_bytes=" << static_cast<int>(unencrypted_bytes)
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

  if (ivLength != IV_SIZE  || (keyIndex < 0 || keyIndex >= 10)) {
    return;
  }

  rtc::Buffer iv = rtc::Buffer(ivLength);
  for (size_t i = 0; i < ivLength; i++) {
    iv[i] = date_in[date_in.size() - 2 - ivLength + i];
  }

  rtc::Buffer encrypted_payload(date_in.size() - unencrypted_bytes - ivLength -
                                2);
  for (size_t i = unencrypted_bytes; i < date_in.size() - ivLength - 2; i++) {
    encrypted_payload[i - unencrypted_bytes] = date_in[i];
  }
  std::vector<uint8_t> buffer;
  AesGcmEncryptDecrypt(EncryptOrDecrypt::kDecrypt, aesKey_, iv, frameHeader,
                       encrypted_payload, &buffer);
  rtc::Buffer payload(buffer.data(), buffer.size());
  rtc::Buffer data_out;
  data_out.AppendData(frameHeader);
  data_out.AppendData(payload);
  frame->SetData(data_out);

  RTC_LOG(LS_INFO) << "FrameCryptorTransformer::decryptFrame() ivLength="
                   << static_cast<int>(ivLength)
                   << " unencrypted_bytes=" << static_cast<int>(unencrypted_bytes)
                   << " keyIndex=" << static_cast<int>(keyIndex)
                   << " aesKey=" << to_hex(aesKey_.data(), aesKey_.size())
                   << " iv=" << to_hex(iv.data(), iv.size());

  if (sink_callback_)
    sink_callback_->OnTransformedFrame(std::move(frame));
}

rtc::Buffer FrameCryptorTransformer::makeIv(uint32_t ssrc, uint32_t timestamp) {
  uint32_t sendCount = 0;
  if (sendCounts_.find(ssrc) == sendCounts_.end()) {
    srand((unsigned)time(NULL));
    sendCounts_[ssrc] = floor(rand() * 0xFFFF);
  } else {
    sendCount = sendCounts_[ssrc];
  }
  rtc::ByteBufferWriter buf;
  buf.WriteUInt32(ssrc);
  buf.WriteUInt32(timestamp);
  buf.WriteUInt32(sendCount % 0xFFFF);
  sendCounts_[ssrc] = sendCount + 1;

  RTC_CHECK_EQ(buf.Length(), IV_SIZE);

  return rtc::Buffer(buf.Data(), buf.Length());
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

std::string to_hex(unsigned char* data, int len) {
  std::stringstream ss;
  ss << std::uppercase << std::hex << std::setfill('0');
  for (int i = 0; i < len; i++) {
    ss << std::setw(2) << static_cast<unsigned>(data[i]);
  }
  return ss.str();
}

}  // namespace libwebrtc