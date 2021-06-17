#include "rtc_rtp_receive_imp.h"

namespace libwebrtc {
RTCRtpReceiverImpl::RTCRtpReceiverImpl(
    rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver)
    : rtp_receiver_(rtp_receiver) {}

rtc::scoped_refptr<webrtc::RtpReceiverInterface>
RTCRtpReceiverImpl::rtp_receiver() {
  return rtp_receiver_;
}
}  // namespace libwebrtc
