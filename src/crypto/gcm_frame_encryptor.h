#include <stddef.h>
#include <stdint.h>

#include <vector>

#include "api/array_view.h"
#include "api/crypto/frame_encryptor_interface.h"
#include "api/media_types.h"
#include "rtc_base/ref_counted_object.h"

#include "crypto.h"

namespace webrtc {

class GCMFrameEncryptor
    : public rtc::RefCountedObject<FrameEncryptorInterface> {
 public:
  explicit GCMFrameEncryptor();
  int Encrypt(cricket::MediaType media_type,
              uint32_t ssrc,
              rtc::ArrayView<const uint8_t> additional_data,
              rtc::ArrayView<const uint8_t> frame,
              rtc::ArrayView<uint8_t> encrypted_frame,
              size_t* bytes_written) override;

  size_t GetMaxCiphertextByteSize(cricket::MediaType media_type,
                                  size_t frame_size) override;
  void SetKey(std::vector<uint8_t> key_bytes);

 private:
  std::unique_ptr<libwebrtc::Crypto::Cipher> cipher_;
  std::vector<uint8_t> aesKey_;
  std::vector<uint8_t> aesIv_;
};
}  // namespace webrtc