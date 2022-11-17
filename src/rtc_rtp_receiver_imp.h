#ifndef LIB_WEBRTC_RTC_RTP_RECEIVER_IMPL_HXX
#define LIB_WEBRTC_RTC_RTP_RECEIVER_IMPL_HXX

#include "api/rtp_receiver_interface.h"
#include "rtc_rtp_receiver.h"

#include "crypto/gcm_frame_decryptor.h"
#include "rtc_frame_decryptor_impl.h"


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

  virtual void SetFrameDecryptor(
      scoped_refptr<RTCFrameDecryptor> frame_decryptor) override {
    frame_decryptor_ = frame_decryptor;
    if (frame_decryptor_ != nullptr) {
      rtp_receiver_->SetFrameDecryptor(
          static_cast<RTCFrameDecryptorImpl*>(frame_decryptor.get())
              ->rtc_frame_decryptor());
    } else {
      rtp_receiver_->SetFrameDecryptor(nullptr);
    }
  }

  virtual scoped_refptr<RTCFrameDecryptor> GetFrameDecryptor() const override {
    return frame_decryptor_;
  }

  virtual void OnFirstPacketReceived(cricket::MediaType media_type) override;

  virtual bool EnableGcmCryptoSuites(vector<uint8_t> key) override {
    if (!gcm_frame_decryptor_) {
      gcm_frame_decryptor_ = rtc::scoped_refptr<webrtc::GCMFrameDecryptor>(
          new webrtc::GCMFrameDecryptor());
    }
    gcm_frame_decryptor_->SetKey(key.std_vector());
    rtp_receiver_->SetFrameDecryptor(gcm_frame_decryptor_);
    return true;
  }

  virtual bool DisableGcmCryptoSuites() override {
    rtp_receiver_->SetFrameDecryptor(nullptr);
    return true;
  }

 private:
  rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver_;
  RTCRtpReceiverObserver* observer_;
  scoped_refptr<RTCFrameDecryptor> frame_decryptor_;
  rtc::scoped_refptr<webrtc::GCMFrameDecryptor> gcm_frame_decryptor_;
};  // namespace libwebrtc

}  // namespace libwebrtc

#endif  // !LIB_WEBRTC_RTP_RECEIV