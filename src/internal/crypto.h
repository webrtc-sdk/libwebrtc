#ifndef CRYPTO_HXX
#define CRYPTO_HXX

#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#include <stdio.h>
#include <string.h>
#include <string>

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

  class Cipher {
   public:
    enum class Type { kEncrypt = 0, kDecrypt };

   public:
    Cipher(Algorithm algorithm, Type type);
    ~Cipher();

    virtual bool initialize();

    virtual bool setkey(const std::string& key, const std::string& iv);

    virtual bool update(const uint8_t* in,
                        size_t in_len,
                        uint8_t* out,
                        size_t* out_len);

    virtual size_t key_length() const;

    virtual size_t iv_length() const;

   private:
    Algorithm algorithm_;
    Type type_;
    EVP_CIPHER_CTX* ctx_ = nullptr;
    const EVP_CIPHER* cipher_ = nullptr;
    size_t aesKeyLength = 0;
    size_t aesIvLength = 0;
    bool initialized_ = false;
  };

 public:
  Crypto();
  ~Crypto();

  virtual int initialize();

  bool generateAesKey(const Cipher& chpper,
                      std::string* aesKey,
                      std::string* aesIv);
};

}  // namespace libwebrtc

#endif