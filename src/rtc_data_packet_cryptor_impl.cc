#include "rtc_data_packet_cryptor_impl.h"

namespace libwebrtc {

scoped_refptr<EncryptedPacket> RTCDataPacketCryptorImpl::encrypt(
    string participant_id, int key_index, vector<uint8_t> data) {
  webrtc::RTCErrorOr<webrtc::scoped_refptr<webrtc::EncryptedPacket>> value =
      data_packet_cryptor_->Encrypt(participant_id.std_string(), key_index,
                                    data.std_vector());
  if (value.ok()) {
    return new RefCountedObject<EncryptedPacketImpl>(value.value());
  } else {
    return nullptr;
  }
}

vector<uint8_t> RTCDataPacketCryptorImpl::decrypt(
    string participant_id, int key_index,
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

RTCDataPacketCryptorImpl::RTCDataPacketCryptorImpl(
    scoped_refptr<KeyProvider> key_provider,
    webrtc::FrameCryptorTransformer::Algorithm algorithm) {
  auto key_provider_impl =
      static_cast<DefaultKeyProviderImpl*>(key_provider.get());
  data_packet_cryptor_ = webrtc::make_ref_counted<webrtc::DataPacketCryptor>(
      algorithm, key_provider_impl->rtc_key_provider());
  key_provider_ = key_provider;
}

scoped_refptr<EncryptedPacket> EncryptedPacket::Create(vector<uint8_t> data,
                                                       vector<uint8_t> iv,
                                                       uint8_t key_index) {
  return new RefCountedObject<EncryptedPacketImpl>(data, iv, key_index);
}

scoped_refptr<RTCDataPacketCryptor> RTCDataPacketCryptor::Create(
    scoped_refptr<KeyProvider> key_provider, FrameCryptorAlgorithm algorithm) {
  return new RefCountedObject<RTCDataPacketCryptorImpl>(
      key_provider, AlgorithmToFrameCryptorAlgorithm(algorithm));
}

}  // namespace libwebrtc