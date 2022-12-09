#ifndef LIB_WEBRTC_RTC_RTP_RECEIVER_IMPL_HXX
#define LIB_WEBRTC_RTC_RTP_RECEIVER_IMPL_HXX

#include "api/rtp_receiver_interface.h"
#include "rtc_rtp_receiver.h"

#include "api/crypto/frame_crypto_transformer.h"

namespace libwebrtc {
class RTCRtpReceiverImpl : public RTCRtpReceiver,
                           webrtc::RtpReceiverObserverInterface {
 public:
  RTCRtpReceiverImpl(
      rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver);

  virtual scoped_refptr<RTCMediaTrack> track() const override;
  virtual scoped_refptr<RTCDtlsTransport> dtls_transport() const override;
  virtual const vector<string> stream_ids() const override;
  virtual vector<scoped_refptr<RTCMediaStream>> streams() const override;
  virtual RTCMediaType media_type() const override;
  virtual const string id() const override;
  virtual scoped_refptr<RTCRtpParameters> parameters() const override;
  virtual bool set_parameters(
      scoped_refptr<RTCRtpParameters> parameters) override;
  virtual void SetObserver(RTCRtpReceiverObserver* observer) override;
  virtual void SetJitterBufferMinimumDelay(double delay_seconds) override;
  rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver();

  virtual bool EnableGcmCryptoSuites(const vector<uint8_t>& key) override {
    if (!e2ee_transformer_) {

      if (!key_manager_) {
        key_manager_ = std::make_shared<webrtc::DefaultKeyManager>();
      }
      
      auto mediaType =
          rtp_receiver_->track()->kind() == "audio"
              ? webrtc::FrameCryptorTransformer::MediaType::kAudioFrame
              : webrtc::FrameCryptorTransformer::MediaType::kVideoFrame;
      e2ee_transformer_ = rtc::scoped_refptr<webrtc::FrameCryptorTransformer>(
          new webrtc::FrameCryptorTransformer(
              mediaType, webrtc::FrameCryptorTransformer::Algorithm::kAesGcm,
              key_manager_));
    }
    key_manager_->SetKey(0, key.std_vector());
    rtp_receiver_->SetDepacketizerToDecoderFrameTransformer(e2ee_transformer_);
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
  rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver_;
  RTCRtpReceiverObserver* observer_;
  rtc::scoped_refptr<webrtc::FrameCryptorTransformer> e2ee_transformer_;
  std::shared_ptr<webrtc::DefaultKeyManager> key_manager_;
  void OnFirstPacketReceived(cricket::MediaType media_type) override;
};  // namespace libwebrtc

}  // namespace libwebrtc

#endif  // !LIB_WEBRTC_RTP_RECEIV