#include "rtc_data_packet_cryptor_impl.h"

namespace libwebrtc {

scoped_refptr<EncryptedPacket> EncryptedPacket::Create(vector<uint8_t> data,
                                                       vector<uint8_t> iv,
                                                       uint8_t key_index) {
  return new RefCountedObject<EncryptedPacketImpl>(data, iv, key_index);
}

scoped_refptr<RTCDataPacketCryptor>
RTCDataPacketCryptor::Create(KeyProvider* key_provider, Algorithm algorithm) {
  DefaultKeyProviderImpl* key_provider_impl =
      static_cast<DefaultKeyProviderImpl*>(key_provider);
  return new RefCountedObject<RTCDataPacketCryptorImpl>(
      key_provider_impl->rtc_key_provider(),
      AlgorithmToFrameCryptorAlgorithm(algorithm));
}

} // namespace libwebrtc