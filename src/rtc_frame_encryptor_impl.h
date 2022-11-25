#ifndef RTC_FRAME_ENCRYPTOR_IMPL_HXX
#define RTC_FRAME_ENCRYPTOR_IMPL_HXX

#include "rtc_frame_encryptor.h"

#include "api/crypto/frame_encryptor_interface.h"

namespace libwebrtc {

class RTCFrameEncryptorInternal : public webrtc::FrameEncryptorInterface {
 public:
  RTCFrameEncryptorInternal(
      EncryptCallback encrypt,
      GetMaxCiphertextByteSizeCallback get_max_ciphertext_byte_size)
      : encrypt_callback_(encrypt),
        get_max_ciphertext_byte_size_callback_(get_max_ciphertext_byte_size) {}

  virtual int Encrypt(cricket::MediaType media_type,
                      webrtc::VideoFrameType* frame_type,
                      uint32_t ssrc,
                      uint32_t timestamp,
                      rtc::ArrayView<const uint8_t> additional_data,
                      rtc::ArrayView<const uint8_t> frame,
                      rtc::ArrayView<uint8_t> encrypted_frame,
                      size_t* bytes_written) override {
    // return encrypt_callback_(ConvertMediaType(media_type), ssrc,
    // additional_data, frame,
    //                         encrypted_frame, bytes_written);
    return 0;
  }

  virtual size_t GetMaxCiphertextByteSize(cricket::MediaType media_type,
                                          webrtc::VideoFrameType* frame_type,
                                          size_t frame_size) override {
    return get_max_ciphertext_byte_size_callback_(ConvertMediaType(media_type),
                                                  frame_size);
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
  EncryptCallback encrypt_callback_;
  GetMaxCiphertextByteSizeCallback get_max_ciphertext_byte_size_callback_;
};

class RTCFrameEncryptorImpl : public RTCFrameEncryptor {
 public:
  RTCFrameEncryptorImpl(
      EncryptCallback encrypt,
      GetMaxCiphertextByteSizeCallback get_max_ciphertext_byte_size);
  ~RTCFrameEncryptorImpl() override;

  rtc::scoped_refptr<webrtc::FrameEncryptorInterface> rtc_frame_encryptor() {
    return frame_encryptor_;
  }

 protected:
  rtc::scoped_refptr<webrtc::FrameEncryptorInterface> frame_encryptor_;
};
}  // namespace libwebrtc
#endif  // RTC_FRAME_ENCRYPTOR_IMPL_HXX