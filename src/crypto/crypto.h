#ifndef LIBWEBRTC_CRYPTO_HXX
#define LIBWEBRTC_CRYPTO_HXX

#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

namespace libwebrtc {

class Crypto {
 public:
  enum class Algorithm {
    kAes128Gcm = 0,
    kAes256Gcm,
    kAes128Cbc,
    kAes256Cbc,
    kNull
  };
  
  enum class Type { kEncrypt = 0, kDecrypt };

  class Cipher {
   public:
    Cipher(Algorithm algorithm, Type type);
    virtual ~Cipher();

    virtual bool initialize();

    virtual bool setkey(const std::vector<uint8_t> key,
                        std::vector<uint8_t> iv);

    virtual bool update(const uint8_t* in,
                        size_t in_len,
                        uint8_t* out,
                        size_t* out_len);

    virtual size_t key_length() const;

    virtual size_t iv_length() const;

    virtual const std::vector<uint8_t>& key() { return key_; }

    virtual const std::vector<uint8_t>& iv() { return iv_; }

   private:
    Algorithm algorithm_;
    Type type_;
    EVP_CIPHER_CTX* ctx_ = nullptr;
    const EVP_CIPHER* cipher_ = nullptr;
    size_t aesKeyLength = 0;
    size_t aesIvLength = 0;
    bool initialized_ = false;
    std::vector<uint8_t> key_;
    std::vector<uint8_t> iv_;
  };

 public:
  Crypto();
  ~Crypto();

  static bool generateAesKey(const Cipher& chpper,
                      std::vector<uint8_t>& aesKey,
                      std::vector<uint8_t>& aesIv);

  static std::vector<uint8_t> key;
  static std::vector<uint8_t> iv;
};

}  // namespace libwebrtc

#endif //LIBWEBRTC_CRYPTO_HXX