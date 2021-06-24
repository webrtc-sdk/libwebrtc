#ifndef LIB_WEBRTC_RTC_RTP_RECEIVER_IMPL_HXX
#define LIB_WEBRTC_RTC_RTP_RECEIVER_IMPL_HXX

#include "api/rtp_receiver_interface.h"
#include "rtc_rtp_receiver.h"

namespace libwebrtc {
class RTCRtpReceiverImpl : public RTCRtpReceiver,
                           webrtc:: RtpReceiverObserverInterface {
 public:
  RTCRtpReceiverImpl(
      rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver);

  virtual scoped_refptr<RTCMediaTrack> Track() const override;
  virtual scoped_refptr<RTCDtlsTransport> DtlsTransport() const override;
  virtual void StreamIds(OnString on) const override;
  virtual void Streams(OnRTCMediaStream on) const override;
  virtual RTCMediaType MediaType() const override;
  virtual void Id(OnString on) const override;
  virtual scoped_refptr<RTCRtpParameters> GetParameters() const override;
  virtual bool SetParameters(
      scoped_refptr<RTCRtpParameters> parameters) override;
  virtual void SetObserver(RTCRtpReceiverObserver* observer) override;
  virtual void SetJitterBufferMinimumDelay(double delay_seconds) override;
  rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver();

 private:
  rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver_;
  RTCRtpReceiverObserver* observer_;

virtual void OnFirstPacketReceived(cricket::MediaType media_type) override;

};  // namespace libwebrtc

}  // namespace libwebrtc

#endif  // !LIB_WEBRTC_RTP_RECEIV