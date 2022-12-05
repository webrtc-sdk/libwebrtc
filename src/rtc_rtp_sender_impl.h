#ifndef LIB_WEBRTC_RTC_RTP_SENDER_IMPL_HXX
#define LIB_WEBRTC_RTC_RTP_SENDER_IMPL_HXX

#include "api/rtp_sender_interface.h"
#include "api/scoped_refptr.h"
#include "rtc_base/ref_counted_object.h"

#include "api/crypto/frame_crypto_transformer.h"
#include "rtc_rtp_sender.h"

namespace libwebrtc {

class RTCRtpSenderImpl : public RTCRtpSender {
 public:
  RTCRtpSenderImpl(rtc::scoped_refptr<webrtc::RtpSenderInterface> rtp_sender);

  virtual bool set_track(scoped_refptr<RTCMediaTrack> track) override;
  virtual scoped_refptr<RTCMediaTrack> track() const override;
  virtual scoped_refptr<RTCDtlsTransport> dtls_transport() const override;
  virtual uint32_t ssrc() const override;
  virtual RTCMediaType media_type() const override;
  virtual const string id() const override;
  virtual const vector<string> stream_ids() const override;
  virtual void set_stream_ids(const vector<string> stream_ids) const override;
  virtual const vector<scoped_refptr<RTCRtpEncodingParameters>>
  init_send_encodings() const override;
  virtual scoped_refptr<RTCRtpParameters> parameters() const override;
  virtual bool set_parameters(
      const scoped_refptr<RTCRtpParameters> parameters) override;
  virtual scoped_refptr<RTCDtmfSender> dtmf_sender() const override;
  rtc::scoped_refptr<webrtc::RtpSenderInterface> rtc_rtp_sender() {
    return rtp_sender_;
  }
  virtual bool EnableGcmCryptoSuites(const vector<uint8_t>& key) override {
    if (!e2ee_transformer_) {
      if (!key_manager_) {
        key_manager_ = std::make_shared<webrtc::KeyManagerImpl>();
      }

      auto mediaType =
          rtp_sender_->track()->kind() == "audio"
              ? webrtc::FrameCryptorTransformer::MediaType::kAudioFrame
              : webrtc::FrameCryptorTransformer::MediaType::kVideoFrame;
      e2ee_transformer_ = rtc::scoped_refptr<webrtc::FrameCryptorTransformer>(
          new webrtc::FrameCryptorTransformer(
              mediaType, webrtc::FrameCryptorTransformer::Algorithm::kAesGcm,
              key_manager_));
    }
    key_manager_->SetKey(0, key.std_vector());
    rtp_sender_->SetEncoderToPacketizerFrameTransformer(e2ee_transformer_);
    e2ee_transformer_->SetEnabled(true);
    return true;
  }

  virtual bool DisableGcmCryptoSuites() override {
    if(e2ee_transformer_) {
      e2ee_transformer_->SetEnabled(false);
      return true;
    }
    return false;
  }

 private:
  rtc::scoped_refptr<webrtc::RtpSenderInterface> rtp_sender_;
  rtc::scoped_refptr<webrtc::FrameCryptorTransformer> e2ee_transformer_;
  std::shared_ptr<webrtc::KeyManagerImpl> key_manager_;
};
}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_RTP_SENDER_IMPL_HXX