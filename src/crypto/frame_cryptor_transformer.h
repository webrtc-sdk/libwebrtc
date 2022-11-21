#ifndef LIB_WEBRTC_FRAME_CRYPTOR_TRANSFORMER_H_
#define LIB_WEBRTC_FRAME_CRYPTOR_TRANSFORMER_H_

#include "api/frame_transformer_interface.h"
#include "rtc_base/synchronization/mutex.h"
#include "rtc_base/buffer.h"

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
  virtual void SetKey(const std::string& key);

 private:
  void encryptFrame(std::unique_ptr<webrtc::TransformableFrameInterface> frame);
  void decryptFrame(std::unique_ptr<webrtc::TransformableFrameInterface> frame);
  rtc::Buffer makeIv(uint32_t ssrc, uint32_t timestamp);

 private:
  mutable webrtc::Mutex mutex_;
  MediaType type_;
  rtc::scoped_refptr<webrtc::TransformedFrameCallback> sink_callback_;
  std::vector<uint8_t> aesKey_;
  std::map<uint32_t, uint32_t> sendCounts_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_FRAME_CRYPTOR_TRANSFORMER_H_