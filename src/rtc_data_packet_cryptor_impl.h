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
  RTCDataPacketCryptorImpl(
      scoped_refptr<KeyProvider> key_provider,
      webrtc::FrameCryptorTransformer::Algorithm algorithm);
  ~RTCDataPacketCryptorImpl() {}

  scoped_refptr<EncryptedPacket> encrypt(string participant_id, int key_index,
                                         vector<uint8_t> data) override;

  vector<uint8_t> decrypt(
      string participant_id, int key_index,
      scoped_refptr<EncryptedPacket> encrypted_packet) override;

 private:
  webrtc::scoped_refptr<webrtc::DataPacketCryptor> data_packet_cryptor_;
  scoped_refptr<KeyProvider> key_provider_;
};

}  // namespace libwebrtc

#endif  // LIB_RTC_DATA_PACKET_CRYPTOR_IMPL_H_