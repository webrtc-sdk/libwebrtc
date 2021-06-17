#ifndef LIB_WEBRTC_RTC_RTP_SENDER_IMPL_HXX
#define LIB_WEBRTC_RTC_RTP_SENDER_IMPL_HXX

#include "api/rtp_sender_interface.h"
#include "api/scoped_refptr.h"
#include "rtc_rtp_sender.h"

namespace libwebrtc {
class RTCRtpSenderImpl : public RTCRtpSender {
 public:
  RTCRtpSenderImpl(rtc::scoped_refptr<webrtc::RtpSenderInterface> rtp_sender);

  rtc::scoped_refptr<webrtc::RtpSenderInterface> rtp_sender();

 private:
  rtc::scoped_refptr<webrtc::RtpSenderInterface> rtp_sender_;
};
}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_RTP_SENDER_IMPL_HXX