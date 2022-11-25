#ifndef RTC_FRAME_DECRYPTOR_IMPL_HXX
#define RTC_FRAME_DECRYPTOR_IMPL_HXX

#include "api/crypto/frame_decryptor_interface.h"
#include "rtc_frame_decryptor.h"

namespace libwebrtc {

class RTCFrameDecryptorInternal : public webrtc::FrameDecryptorInterface {
 public:
  RTCFrameDecryptorInternal(
      DecryptCallback decrypt,
      GetMaxPlaintextByteSizeCallback get_max_plaintext_byte_size)
      : decrypt_callback_(decrypt),
        get_max_plaintext_byte_size_callback_(get_max_plaintext_byte_size) {}

  virtual Result Decrypt(cricket::MediaType media_type,
                         const webrtc::VideoFrameType* frame_type,
                         const std::vector<uint32_t>& csrcs,
                         rtc::ArrayView<const uint8_t> additional_data,
                         rtc::ArrayView<const uint8_t> encrypted_frame,
                         rtc::ArrayView<uint8_t> frame) override {
    // DecryptResult res =
    //      decrypt_callback_(ConvertMediaType(media_type), csrcs,
    //      additional_data,
    //                        encrypted_frame, frame);

    return webrtc::FrameDecryptorInterface::Result(
        webrtc::FrameDecryptorInterface::Status::kOk, 0);
  }

  virtual size_t GetMaxPlaintextByteSize(cricket::MediaType media_type,
                                         const webrtc::VideoFrameType* frame_type,
                                         size_t encrypted_frame_size) override {
    return get_max_plaintext_byte_size_callback_(ConvertMediaType(media_type),
                                                 encrypted_frame_size);
  }

  webrtc::FrameDecryptorInterface::Status ConvertResultStatus(
      DecryptResultStatus status) {
    switch (status) {
      case DecryptResultStatus::kOk:
        return webrtc::FrameDecryptorInterface::Status::kOk;
      case DecryptResultStatus::kRecoverable:
        return webrtc::FrameDecryptorInterface::Status::kRecoverable;
      case DecryptResultStatus::kFailedToDecrypt:
        return webrtc::FrameDecryptorInterface::Status::kFailedToDecrypt;
      case DecryptResultStatus::kUnknown:
        return webrtc::FrameDecryptorInterface::Status::kUnknown;
    }
    return webrtc::FrameDecryptorInterface::Status::kUnknown;
  }

  RTCMediaType ConvertMediaType(cricket::MediaType media_type) {
    switch (media_type) {
      case cricket::MEDIA_TYPE_AUDIO:
        return RTCMediaType::AUDIO;
      case cricket::MEDIA_TYPE_VIDEO:
        return RTCMediaType::VIDEO;
      case cricket::MEDIA_TYPE_DATA:
        return RTCMediaType::DATA;
      default:
        return RTCMediaType::ANY;
    }
  }

 private:
  DecryptCallback decrypt_callback_;
  GetMaxPlaintextByteSizeCallback get_max_plaintext_byte_size_callback_;
};

class RTCFrameDecryptorImpl : public RTCFrameDecryptor {
 public:
  RTCFrameDecryptorImpl(
      DecryptCallback decrypt,
      GetMaxPlaintextByteSizeCallback get_max_plaintext_byte_size);
  ~RTCFrameDecryptorImpl() override;

  rtc::scoped_refptr<webrtc::FrameDecryptorInterface> rtc_frame_decryptor() {
    return frame_decryptor_;
  }

 private:
  rtc::scoped_refptr<RTCFrameDecryptorInternal> frame_decryptor_;
};

}  // namespace libwebrtc

#endif  // RTC_FRAME_DECRYPTOR_IMPL_HXX