#ifndef LIB_RTC_DATA_PACKET_CRYPTOR_H_
#define LIB_RTC_DATA_PACKET_CRYPTOR_H_

#include "base/refcount.h"
#include "rtc_frame_cryptor.h"
#include "rtc_types.h"

namespace libwebrtc {

class EncryptedPacket : public RefCountInterface {
 public:
  LIB_WEBRTC_API static scoped_refptr<EncryptedPacket> Create(
      vector<uint8_t> data, vector<uint8_t> iv, uint8_t key_index);
  virtual vector<uint8_t> data() = 0;
  virtual vector<uint8_t> iv() = 0;
  virtual uint8_t key_index() = 0;
};

class RTCDataPacketCryptor : public RefCountInterface {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCDataPacketCryptor> Create(
      scoped_refptr<KeyProvider> key_provider, FrameCryptorAlgorithm algorithm);

  virtual scoped_refptr<EncryptedPacket> encrypt(string participant_id,
                                                 int key_index,
                                                 vector<uint8_t> data) = 0;

  virtual vector<uint8_t> decrypt(
      string participant_id, int key_index,
      scoped_refptr<EncryptedPacket> encrypted_packet) = 0;

 protected:
  virtual ~RTCDataPacketCryptor() {}
};

}  // namespace libwebrtc

#endif  // LIB_RTC_DATA_PACKET_CRYPTOR_H_