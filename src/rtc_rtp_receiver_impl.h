#ifndef LIB_WEBRTC_RTC_RTP_RECEIVER_IMPL_HXX
#define LIB_WEBRTC_RTC_RTP_RECEIVER_IMPL_HXX

#include "api/rtp_receiver_interface.h"
#include "rtc_rtp_receiver.h"

namespace libwebrtc {

/**
 * class RTCRtpReceiverObserverImpl
 */
class RTCRtpReceiverObserverImpl : public RTCRtpReceiverObserver
{
 public:
   RTCRtpReceiverObserverImpl(void* callbacks /* rtcRtpReceiverObserverCallbacks* */);
   ~RTCRtpReceiverObserverImpl();

   void OnFirstPacketReceived(RTCMediaType media_type) override;

 private:
   void* callbacks_ /* rtcRtpReceiverObserverCallbacks* */;
}; // end class RTCRtpReceiverObserverImpl

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
  virtual RTCRtpReceiverObserver* GetObserver() override { return observer_; }

 private:
  rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver_;
  RTCRtpReceiverObserver* observer_;

  virtual void OnFirstPacketReceived(cricket::MediaType media_type) override;

};  // namespace libwebrtc

/**
 * class RTCRtpReceiverListImpl
 */
class RTCRtpReceiverListImpl : public RTCRtpReceiverList {
 public:
  RTCRtpReceiverListImpl(const vector<scoped_refptr<RTCRtpReceiver>>& source);
  ~RTCRtpReceiverListImpl();
}; // end class RTCRtpReceiverListImpl

}  // namespace libwebrtc

#endif  // !LIB_WEBRTC_RTP_RECEIV