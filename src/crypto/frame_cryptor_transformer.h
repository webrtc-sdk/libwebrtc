#ifndef LIB_WEBRTC_FRAME_CRYPTOR_TRANSFORMER_H_
#define LIB_WEBRTC_FRAME_CRYPTOR_TRANSFORMER_H_

#include "api/frame_transformer_interface.h"
#include "rtc_base/synchronization/mutex.h"

namespace libwebrtc {

class FrameCryptorTransformer
    : public rtc::RefCountedObject<webrtc::FrameTransformerInterface> {
 public:
  enum class MediaType {
    kAudioFrame = 0,
    kVideoFrame,
  };

 public:
  explicit FrameCryptorTransformer(MediaType type);
  virtual void RegisterTransformedFrameCallback(
      rtc::scoped_refptr<webrtc::TransformedFrameCallback>) override;
  virtual void RegisterTransformedFrameSinkCallback(
      rtc::scoped_refptr<webrtc::TransformedFrameCallback>,
      uint32_t ssrc) override;
  virtual void UnregisterTransformedFrameSinkCallback(uint32_t ssrc) override;
  virtual void UnregisterTransformedFrameCallback() override;
  virtual void Transform(
      std::unique_ptr<webrtc::TransformableFrameInterface> frame) override;
  virtual void SetKey(std::vector<uint8_t> key_bytes) {
    aesKey_ = key_bytes;
  }

 private:
  virtual void encryptFrame(
      std::unique_ptr<webrtc::TransformableFrameInterface> frame);
  virtual void decryptFrame(
      std::unique_ptr<webrtc::TransformableFrameInterface> frame);
  mutable webrtc::Mutex mutex_;
  MediaType type_;
  rtc::scoped_refptr<webrtc::TransformedFrameCallback> sink_callback_;
  std::vector<uint8_t> aesKey_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_FRAME_CRYPTOR_TRANSFORMER_H_