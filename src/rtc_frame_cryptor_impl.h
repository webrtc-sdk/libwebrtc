#ifndef LIB_RTC_FRAME_CYRPTOR_IMPL_H_
#define LIB_RTC_FRAME_CYRPTOR_IMPL_H_

#include "rtc_frame_cryptor.h"
#include "rtc_rtp_receiver_impl.h"
#include "rtc_rtp_sender_impl.h"

#include "api/crypto/frame_crypto_transformer.h"
#include "api/rtp_receiver_interface.h"
#include "api/rtp_sender_interface.h"

namespace libwebrtc {
class DefaultKeyProviderImpl : public KeyProvider {
 public:
  DefaultKeyProviderImpl(KeyProviderOptions* options) {
    webrtc::KeyProviderOptions rtc_options;
    rtc_options.shared_key = options->shared_key;
    rtc_options.ratchet_salt = options->ratchet_salt.std_vector();
    rtc_options.uncrypted_magic_bytes =
        options->uncrypted_magic_bytes.std_vector();
    rtc_options.ratchet_window_size = options->ratchet_window_size;
    rtc_options.failure_tolerance = options->failure_tolerance;
    impl_ =
        new rtc::RefCountedObject<webrtc::DefaultKeyProviderImpl>(rtc_options);
  }
  ~DefaultKeyProviderImpl() {}

  bool SetSharedKey(int index, vector<uint8_t> key) override {
    return impl_->SetSharedKey(index, key.std_vector());
  }

  vector<uint8_t> RatchetSharedKey(int key_index) override {
    return impl_->RatchetSharedKey(key_index);
  }

  vector<uint8_t> ExportSharedKey(int key_index) override {
    return impl_->ExportSharedKey(key_index);
  }

  /// Set the key at the given index.
  bool SetKey(const string participant_id,
              int index,
              vector<uint8_t> key) override {
    return impl_->SetKey(participant_id.std_string(), index, key.std_vector());
  }

  vector<uint8_t> RatchetKey(const string participant_id,
                             int key_index) override {
    return impl_->RatchetKey(participant_id.std_string(), key_index);
  }

  vector<uint8_t> ExportKey(const string participant_id,
                            int key_index) override {
    return impl_->ExportKey(participant_id.std_string(), key_index);
  }

  void SetSifTrailer(vector<uint8_t> trailer) override {
    impl_->SetSifTrailer(trailer.std_vector());
  }

  rtc::scoped_refptr<webrtc::KeyProvider> rtc_key_provider() { return impl_; }

 private:
  rtc::scoped_refptr<webrtc::DefaultKeyProviderImpl> impl_;
};

class RTCFrameCryptorImpl : public RTCFrameCryptor,
                            public webrtc::FrameCryptorTransformerObserver {
 public:
  RTCFrameCryptorImpl(const string participant_id,
                      Algorithm algorithm,
                      scoped_refptr<KeyProvider> key_provider,
                      scoped_refptr<RTCRtpSender> sender);

  RTCFrameCryptorImpl(const string participant_id,
                      Algorithm algorithm,
                      scoped_refptr<KeyProvider> key_provider,
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

  void OnFrameCryptionStateChanged(const std::string participant_id,
                                   webrtc::FrameCryptionState error) override;

 private:
  string participant_id_;
  mutable webrtc::Mutex mutex_;
  bool enabled_;
  int key_index_;
  rtc::scoped_refptr<webrtc::FrameCryptorTransformer> e2ee_transformer_;
  scoped_refptr<KeyProvider> key_provider_;
  scoped_refptr<RTCRtpSender> sender_;
  scoped_refptr<RTCRtpReceiver> receiver_;
  RTCFrameCryptorObserver* observer_ = nullptr;
};

}  // namespace libwebrtc

#endif  // LIB_RTC_FRAME_CYRPTOR_IMPL_H_