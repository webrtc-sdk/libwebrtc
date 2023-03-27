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
  DefaultKeyManagerImpl(KeyProviderOptions* options) {
    webrtc::KeyProviderOptions rtc_options;
    rtc_options.shared_key = options->shared_key;
    rtc_options.ratchet_salt = options->ratchet_salt.std_vector();
    rtc_options.ratchet_window_size = options->ratchet_window_size;
    impl_ =
        new rtc::RefCountedObject<webrtc::DefaultKeyManagerImpl>(rtc_options);
  }
  ~DefaultKeyManagerImpl() {}
  /// Set the key at the given index.
  bool SetKey(const string participant_id,
              int index,
              vector<uint8_t> key) override {
    return impl_->SetKey(participant_id.std_string(), index, key.std_vector());
  }

  void RatchetKey(const string participant_id, int key_index) override {
    impl_->RatchetKey(participant_id.std_string(), key_index);
  }

  rtc::scoped_refptr<webrtc::KeyManager> rtc_key_manager() { return impl_; }

 private:
  rtc::scoped_refptr<webrtc::DefaultKeyManagerImpl> impl_;
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