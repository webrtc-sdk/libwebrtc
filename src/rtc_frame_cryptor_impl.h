#ifndef LIB_RTC_FRAME_CYRPTOR_IMPL_H_
#define LIB_RTC_FRAME_CYRPTOR_IMPL_H_

#include "rtc_frame_cryptor.h"
#include "rtc_rtp_receiver_impl.h"
#include "rtc_rtp_sender_impl.h"

#include "api/crypto/frame_crypto_transformer.h"
#include "api/rtp_receiver_interface.h"
#include "api/rtp_sender_interface.h"


namespace libwebrtc {

class DefaultKeyManagerImpl : public KeyManager {
 public:
  DefaultKeyManagerImpl();
  ~DefaultKeyManagerImpl();
  /// Set the key at the given index.
  bool SetKey(int index, vector<uint8_t> key) override;

  /// Set the keys.
  bool SetKeys(vector<vector<uint8_t>> keys) override;

  const vector<vector<uint8_t>> GetKeys() const override;

  const std::vector<std::vector<uint8_t>> keys() const;

  rtc::scoped_refptr<webrtc::KeyManager> rtc_key_manager() { return impl_; }

 private:
  class KeyManagerImpl : public webrtc::KeyManager {
   public:
    KeyManagerImpl(DefaultKeyManagerImpl* parent) : parent_(parent) {}
    ~KeyManagerImpl() {}
    const std::vector<std::vector<uint8_t>> keys() const override {
      return parent_->keys();
    }

   private:
    DefaultKeyManagerImpl* parent_;
  };

 private:
  rtc::scoped_refptr<KeyManagerImpl> impl_;
  std::vector<std::vector<uint8_t>> keys_;
  mutable webrtc::Mutex mutex_;
};

class RTCFrameCryptorImpl : public RTCFrameCryptor {
 public:
  RTCFrameCryptorImpl(Algorithm algorithm,
                      scoped_refptr<KeyManager> key_manager,
                      scoped_refptr<RTCRtpSender> sender);

  RTCFrameCryptorImpl(Algorithm algorithm,
                      scoped_refptr<KeyManager> key_manager,
                      scoped_refptr<RTCRtpReceiver> receiver);
  ~RTCFrameCryptorImpl();

  bool SetEnabled(bool enabled) override;
  bool enabled() const override;
  bool SetKeyIndex(int index) override;
  int key_index() const override;

 private:
  mutable webrtc::Mutex mutex_;
  bool enabled_;
  int key_index_;
  rtc::scoped_refptr<webrtc::FrameCryptorTransformer> e2ee_transformer_;
  scoped_refptr<KeyManager> key_manager_;
  scoped_refptr<RTCRtpSender> sender_;
  scoped_refptr<RTCRtpReceiver> receiver_;
};

}  // namespace libwebrtc

#endif  // LIB_RTC_FRAME_CYRPTOR_IMPL_H_