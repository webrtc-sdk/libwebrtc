#ifndef LIB_WEBRTC_RTC_RTP_TRANSCEIVER_IMPL_HXX
#define LIB_WEBRTC_RTC_RTP_TRANSCEIVER_IMPL_HXX

#include "api/rtp_transceiver_interface.h"
#include "api/scoped_refptr.h"
#include "rtc_rtp_transceiver.h"

namespace libwebrtc {
class RTCRtpTransceiverInitImpl : public RTCRtpTransceiverInit {
 public:
  RTCRtpTransceiverInitImpl();

  virtual RTCRtpTransceiverDirection GetDirection() override;
  virtual void SetDirection(RTCRtpTransceiverDirection value) override;
  virtual Vector<String> GetStreamIds() override;
  virtual void SetStreamIds(Vector<String> value) override;
  virtual Vector<scoped_refptr<RTCRtpEncodingParameters>> GetSendEncodings()
      override;
  virtual void SetSendEncodings(
      Vector<scoped_refptr<RTCRtpEncodingParameters>> value) override;

  webrtc::RtpTransceiverInit rtp_transceiver_init();

 private:
  webrtc::RtpTransceiverInit rtp_transceiver_init_;
};

class RTCRtpTransceiverImpl : public RTCRtpTransceiver {
 public:
  RTCRtpTransceiverImpl(
      rtc::scoped_refptr<webrtc::RtpTransceiverInterface> rtp_transceiver);

  virtual RTCMediaType GetMediaType() const override;
  virtual String GetMid() const override;
  virtual scoped_refptr<RTCRtpSender> Sender() const override;
  virtual scoped_refptr<RTCRtpReceiver> Receiver() const override;
  virtual bool Stopped() const override;
  virtual bool Stopping() const override;
  virtual RTCRtpTransceiverDirection Direction() const override;
  virtual String SetDirectionWithError(
      RTCRtpTransceiverDirection new_direction) override;
  virtual RTCRtpTransceiverDirection CurrentDirection() const override;
  virtual RTCRtpTransceiverDirection FiredDirection() const override;
  virtual String StopStandard() override;
  virtual void StopInternal() override;

  rtc::scoped_refptr<webrtc::RtpTransceiverInterface> rtp_transceiver();

 private:
  rtc::scoped_refptr<webrtc::RtpTransceiverInterface> rtp_transceiver_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_TYPES_HXX
