#ifndef LIB_WEBRTC_RTC_RTP_RECEIVER_IMPL_HXX
#define LIB_WEBRTC_RTC_RTP_RECEIVER_IMPL_HXX

#include "api/rtp_receiver_interface.h"
#include "rtc_rtp_receiver.h"

namespace libwebrtc {
class RTCRtpReceiverImpl : public RTCRtpReceiver {
 public:
  RTCRtpReceiverImpl(
      rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver);

  rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver();

 private:
  rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver_;
};  // namespace libwebrtc

}  // namespace libwebrtc

#endif  // !LIB_WEBRTC_RTP_RECEIV