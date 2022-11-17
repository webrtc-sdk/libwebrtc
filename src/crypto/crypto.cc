#include "crypto.h"

#include <iostream>

namespace libwebrtc {

Crypto::Crypto() {}

Crypto::~Crypto() {}

std::vector<uint8_t> Crypto::key;
std::vector<uint8_t> Crypto::iv;


bool Crypto::generateAesKey(const Cipher& cihper,
                            std::vector<uint8_t>& aes_key,
                            std::vector<uint8_t>& aes_iv) {
  uint8_t* aesKey = (uint8_t*)malloc(cihper.key_length());
  uint8_t* aesIv = (uint8_t*)malloc(cihper.iv_length());

  if (aesKey == NULL || aesIv == NULL) {
    return false;
  }
  if (RAND_bytes(aesKey, cihper.key_length()) == 0) {
    std::cout << "RAND_bytes failed" << std::endl;
    return false;
  }

  if (RAND_bytes(aesIv, cihper.iv_length()) == 0) {
    std::cout << "RAND_bytes failed" << std::endl;
    return false;
  }

  aes_key.assign(aesKey, aesKey + cihper.key_length());
  aes_iv.assign(aesIv, aesIv + cihper.iv_length());

  free(aesKey);
  free(aesIv);
  return true;
}

Crypto::Cipher::Cipher(Algorithm algorithm, Type type)
    : algorithm_(algorithm), type_(type), ctx_(EVP_CIPHER_CTX_new()) {}

Crypto::Cipher::~Cipher() {
  EVP_CIPHER_CTX_free(ctx_);
}

bool Crypto::Cipher::initialize() {
  if (initialized_) {
    return false;
  }
  EVP_CIPHER_CTX_init(ctx_);

  switch (algorithm_) {
    case Algorithm::kAes128Gcm:
      cipher_ = EVP_aes_128_gcm();
      break;
    case Algorithm::kAes256Gcm:
      cipher_ = EVP_aes_256_gcm();
      break;
    case Algorithm::kAes128Cbc:
      cipher_ = EVP_aes_128_cbc();
      break;
    case Algorithm::kAes256Cbc:
      cipher_ = EVP_aes_256_cbc();
      break;
    case Algorithm::kNull:
      break;
    default:
      break;
  }

  if (cipher_ == nullptr) {
    std::cout << "cipher not found!" << std::endl;
    return false;
  }

  if (EVP_CipherInit_ex(ctx_, cipher_, NULL, NULL, NULL, 1) != 1) {
    std::cout << "EVP_CipherInit_ex failed" << std::endl;
    return false;
  }

  /* Now we can set key and IV lengths */
  aesKeyLength = EVP_CIPHER_CTX_key_length(ctx_);
  aesIvLength = EVP_CIPHER_CTX_iv_length(ctx_);
  initialized_ = true;
  return true;
}

bool Crypto::Cipher::setkey(const std::vector<uint8_t> key,
                            const std::vector<uint8_t> iv) {
  if (!initialized_) {
    std::cout << "Cipher not initialized" << std::endl;
    return false;
  }

  key_ = key;
  iv_ = iv;

  switch (type_) {
    case Type::kEncrypt:
      if (!EVP_EncryptInit_ex(ctx_, cipher_, NULL, (uint8_t*)key_.data(),
                              (uint8_t*)iv_.data())) {
        std::cout << "EVP_EncryptInit_ex failed" << std::endl;
        return false;
      }
      break;

    case Type::kDecrypt:
      if (!EVP_DecryptInit_ex(ctx_, cipher_, NULL, (uint8_t*)key_.data(),
                              (uint8_t*)iv_.data())) {
        std::cout << "EVP_DecryptInit_ex failed" << std::endl;
        return false;
      }
  }
  return true;
}

bool Crypto::Cipher::update(const uint8_t* in,
                            size_t in_len,
                            uint8_t* out,
                            size_t* out_len) {
  //int output_len = 0;
  //int len = 0;
  //uint8_t* out_data = out;  // new uint8_t[in_len + EVP_MAX_BLOCK_LENGTH];

  //memcpy(out, in, in_len);
  for (size_t i = 0; i < in_len; i++) {
    out[i] = int8_t(in[i]) ^int8_t('a');
  }
  *out_len = in_len;
  return true;
  /*
  if (!initialized_) {
    std::cout << "Cipher not initialized" << std::endl;
    return false;
  }

    switch (type_) {
    case Type::kEncrypt:
      if (!EVP_EncryptInit_ex(ctx_, cipher_, NULL, (uint8_t*)key_.data(),
                              (uint8_t*)iv_.data())) {
        std::cout << "EVP_EncryptInit_ex failed" << std::endl;
        return false;
      }
      break;

    case Type::kDecrypt:
      if (!EVP_DecryptInit_ex(ctx_, cipher_, NULL, (uint8_t*)key_.data(),
                              (uint8_t*)iv_.data())) {
        std::cout << "EVP_DecryptInit_ex failed" << std::endl;
        return false;
      }
  }

  if (EVP_CipherUpdate(ctx_, out_data, &len, in, in_len) != 1) {
    std::cout << "EVP_CipherUpdate failed" << std::endl;
    return false;
  }
  output_len += len;
  if (EVP_CipherFinal_ex(ctx_, out_data + output_len, &len) != 1) {
    std::cout << "EVP_CipherFinal_ex failed" << std::endl;
    return false;
  }
  output_len += len;
  *out_len = output_len;
  return true;
  */
}

size_t Crypto::Cipher::key_length() const {
  return aesKeyLength;
}

size_t Crypto::Cipher::iv_length() const {
  return aesIvLength;
}

}  // namespace libwebrtc
