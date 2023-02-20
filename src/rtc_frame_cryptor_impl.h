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
  DefaultKeyManagerImpl() {
    impl_ =
        new rtc::RefCountedObject<DefaultKeyManagerImpl::KeyManagerImpl>(this);
  }
  ~DefaultKeyManagerImpl() {}
  /// Set the key at the given index.
  bool SetKey(const string participant_id,
              int index,
              vector<uint8_t> key) override {
    webrtc::MutexLock lock(&mutex_);
    auto id = participant_id.std_string();
    if (keys_.find(id) == keys_.end()) {
      keys_[id] = std::vector<std::vector<uint8_t>>();
    }

    if (index + 1 > (int)keys_[id].size()) {
      keys_[id].resize(index + 1);
    }
    keys_[id][index] = key.std_vector();
    return true;
  }

  /// Set the keys.
  bool SetKeys(const string participant_id,
               vector<vector<uint8_t>> keys) override {
    webrtc::MutexLock lock(&mutex_);
    auto id = participant_id.std_string();
    if (keys_.find(id) == keys_.end()) {
      keys_[id] = std::vector<std::vector<uint8_t>>();
    }

    keys_[id].clear();
    for (auto key : keys.std_vector()) {
      keys_[id].push_back(key.std_vector());
    }
    return true;
  }

  const vector<vector<uint8_t>> GetKeys(
      const string participant_id) const override {
    return keys(participant_id.std_string());
  }

  const std::vector<std::vector<uint8_t>> keys(
      const std::string participant_id) const {
    webrtc::MutexLock lock(&mutex_);
    if (keys_.find(participant_id) == keys_.end()) {
      return std::vector<std::vector<uint8_t>>();
    }

    return keys_.find(participant_id)->second;
  }

  rtc::scoped_refptr<webrtc::KeyManager> rtc_key_manager() { return impl_; }

 private:
  class KeyManagerImpl : public webrtc::KeyManager {
   public:
    KeyManagerImpl(DefaultKeyManagerImpl* parent) : parent_(parent) {}
    ~KeyManagerImpl() {}
    const std::vector<std::vector<uint8_t>> keys(
        const std::string participant_id) const override {
      return parent_->keys(participant_id);
    }

   private:
    DefaultKeyManagerImpl* parent_;
  };

 private:
  rtc::scoped_refptr<KeyManagerImpl> impl_;
  std::map<std::string, std::vector<std::vector<uint8_t>>> keys_;
  mutable webrtc::Mutex mutex_;
};

class RTCFrameCryptorImpl : public RTCFrameCryptor,
                            public webrtc::FrameCryptorTransformerObserver {
 public:
  RTCFrameCryptorImpl(const string participant_id,
                      Algorithm algorithm,
                      scoped_refptr<KeyManager> key_manager,
                      scoped_refptr<RTCRtpSender> sender);

  RTCFrameCryptorImpl(const string participant_id,
                      Algorithm algorithm,
                      scoped_refptr<KeyManager> key_manager,
                      scoped_refptr<RTCRtpReceiver> receiver);
  ~RTCFrameCryptorImpl();

  void RegisterRTCFrameCryptorObserver(
      RTCFrameCryptorObserver* observer) override;

  void DeRegisterRTCFrameCryptorObserver() override;

  bool SetEnabled(bool enabled) override;
  bool enabled() const override;
  bool SetKeyIndex(int index) override;
  int key_index() const override;
  const string participant_id() const override { return participant_id_; }

  void OnFrameCryptionError(const std::string participant_id,
                            webrtc::FrameCryptionError error) override;

 private:
  string participant_id_;
  mutable webrtc::Mutex mutex_;
  bool enabled_;
  int key_index_;
  rtc::scoped_refptr<webrtc::FrameCryptorTransformer> e2ee_transformer_;
  scoped_refptr<KeyManager> key_manager_;
  scoped_refptr<RTCRtpSender> sender_;
  scoped_refptr<RTCRtpReceiver> receiver_;
  RTCFrameCryptorObserver* observer_ = nullptr;
};

}  // namespace libwebrtc

#endif  // LIB_RTC_FRAME_CYRPTOR_IMPL_H_