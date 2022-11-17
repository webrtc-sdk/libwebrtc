#include <stddef.h>
#include <stdint.h>

#include <vector>

#include "api/array_view.h"
#include "api/crypto/frame_decryptor_interface.h"
#include "api/media_types.h"
#include "rtc_base/ref_counted_object.h"

#include "crypto.h"

namespace webrtc {
class GCMFrameDecryptor final
    : public rtc::RefCountedObject<FrameDecryptorInterface> {
 public:
  explicit GCMFrameDecryptor();

  Result Decrypt(cricket::MediaType media_type,
                 const std::vector<uint32_t>& csrcs,
                 rtc::ArrayView<const uint8_t> additional_data,
                 rtc::ArrayView<const uint8_t> encrypted_frame,
                 rtc::ArrayView<uint8_t> frame) override;
  size_t GetMaxPlaintextByteSize(cricket::MediaType media_type,
                                 size_t encrypted_frame_size) override;
  void SetKey(std::vector<uint8_t> key_bytes);

 private:
  std::unique_ptr<libwebrtc::Crypto::Cipher> cipher_;
  std::vector<uint8_t> aesKey_;
  std::vector<uint8_t> aesIv_;
};
}  // namespace webrtc