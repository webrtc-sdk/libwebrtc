#ifndef LIB_WEBRTC_RTC_RTP_SENDER_IMPL_HXX
#define LIB_WEBRTC_RTC_RTP_SENDER_IMPL_HXX

#include "api/rtp_sender_interface.h"
#include "api/scoped_refptr.h"
#include "rtc_base/ref_counted_object.h"

#include "crypto/frame_cryptor_transformer.h"
#include "rtc_frame_encryptor_impl.h"
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
  virtual void SetFrameEncryptor(
      scoped_refptr<RTCFrameEncryptor> frame_encryptor) override {
    frame_encryptor_ = frame_encryptor;
    if (frame_encryptor_ != nullptr) {
      rtp_sender_->SetFrameEncryptor(
          static_cast<RTCFrameEncryptorImpl*>(frame_encryptor.get())
              ->rtc_frame_encryptor());
    } else {
      rtp_sender_->SetFrameEncryptor(nullptr);
    }
  }

  virtual scoped_refptr<RTCFrameEncryptor> GetFrameEncryptor() const override {
    return frame_encryptor_;
  }

  virtual bool EnableGcmCryptoSuites(vector<uint8_t> key) override {
    /*
    if (!gcm_frame_encryptor_) {
      gcm_frame_encryptor_ = rtc::scoped_refptr<webrtc::GCMFrameEncryptor>(
          new webrtc::GCMFrameEncryptor());
    }
    gcm_frame_encryptor_->SetKey(key.std_vector());
    rtp_sender_->SetFrameEncryptor(gcm_frame_encryptor_);
    */
    if (!e2ee_transformer_) {
      e2ee_transformer_ = rtc::scoped_refptr<FrameCryptorTransformer>(
          new FrameCryptorTransformer(
              rtp_sender_->track()->kind() == "audio"
                  ? FrameCryptorTransformer::MediaType::kAudioFrame
                  : FrameCryptorTransformer::MediaType::kVideoFrame));
    }
    e2ee_transformer_->SetKey(key.std_vector());
    rtp_sender_->SetEncoderToPacketizerFrameTransformer(e2ee_transformer_);
    return true;
  }

  virtual bool DisableGcmCryptoSuites() override {
    // rtp_sender_->SetFrameEncryptor(nullptr);
    return true;
  }

 private:
  rtc::scoped_refptr<webrtc::RtpSenderInterface> rtp_sender_;
  scoped_refptr<RTCFrameEncryptor> frame_encryptor_;
  rtc::scoped_refptr<libwebrtc::FrameCryptorTransformer> e2ee_transformer_;
};
}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_RTP_SENDER_IMPL_HXX