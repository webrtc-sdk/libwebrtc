#ifndef LIB_RTC_DATA_PACKET_CRYPTOR_IMPL_H_
#define LIB_RTC_DATA_PACKET_CRYPTOR_IMPL_H_

#include "api/crypto/frame_crypto_transformer.h"
#include "base/portable.h"
#include "rtc_data_packet_cryptor.h"
#include "rtc_frame_cryptor_impl.h"

namespace libwebrtc {

class EncryptedPacketImpl : public EncryptedPacket {
 public:
  EncryptedPacketImpl(
      webrtc::scoped_refptr<webrtc::EncryptedPacket> encrypted_packet)
      : encrypted_packet_(encrypted_packet) {}
  EncryptedPacketImpl(vector<uint8_t> data, vector<uint8_t> iv,
                      uint8_t key_index)
      : encrypted_packet_(webrtc::make_ref_counted<webrtc::EncryptedPacket>(
            data.std_vector(), iv.std_vector(), key_index)) {}
  vector<uint8_t> data() override {
    return vector<uint8_t>(encrypted_packet_->data);
  }
  vector<uint8_t> iv() override {
    return vector<uint8_t>(encrypted_packet_->iv);
  }
  uint8_t key_index() override { return encrypted_packet_->key_index; }

  webrtc::scoped_refptr<webrtc::EncryptedPacket> rtc_encrypted_packet() {
    return encrypted_packet_;
  }

 private:
  webrtc::scoped_refptr<webrtc::EncryptedPacket> encrypted_packet_;
};

class RTCDataPacketCryptorImpl : public RTCDataPacketCryptor {
 public:
  scoped_refptr<EncryptedPacket> encrypt(string participant_id, int key_index,
                                         vector<uint8_t> data) {
    webrtc::RTCErrorOr<webrtc::scoped_refptr<webrtc::EncryptedPacket>> value =
        data_packet_cryptor_->Encrypt(participant_id.std_string(), key_index,
                                      data.std_vector());
    if (value.ok()) {
      return new RefCountedObject<EncryptedPacketImpl>(value.value());
    } else {
      return nullptr;
    }
  }

  vector<uint8_t> decrypt(string participant_id, int key_index,
                          scoped_refptr<EncryptedPacket> encrypted_packet) {
    EncryptedPacketImpl* encrypted_packet_impl =
        static_cast<EncryptedPacketImpl*>(encrypted_packet.get());
    webrtc::RTCErrorOr<std::vector<uint8_t>> value =
        data_packet_cryptor_->Decrypt(
            participant_id.std_string(),
            encrypted_packet_impl->rtc_encrypted_packet());
    if (value.ok()) {
      return vector<uint8_t>(value.value());
    } else {
      return vector<uint8_t>();
    }
  }

  RTCDataPacketCryptorImpl(
      webrtc::scoped_refptr<webrtc::KeyProvider> key_provider,
      webrtc::FrameCryptorTransformer::Algorithm algorithm) {
    data_packet_cryptor_ = webrtc::make_ref_counted<webrtc::DataPacketCryptor>(
        algorithm, key_provider);
    key_provider_ = key_provider;
  }
  ~RTCDataPacketCryptorImpl() {}

 private:
  webrtc::scoped_refptr<webrtc::DataPacketCryptor> data_packet_cryptor_;
  webrtc::scoped_refptr<webrtc::KeyProvider> key_provider_;
};

}  // namespace libwebrtc

#endif  // LIB_RTC_DATA_PACKET_CRYPTOR_IMPL_H_