#include "rtc_rtp_sender_impl.h"

namespace libwebrtc {
RTCRtpSenderImpl::RTCRtpSenderImpl(
    rtc::scoped_refptr<webrtc::RtpSenderInterface> rtp_sender)
    : rtp_sender_(rtp_sender) {}

rtc::scoped_refptr<webrtc::RtpSenderInterface>
libwebrtc::RTCRtpSenderImpl::rtp_sender() {
  return rtp_sender_;
}
}  // namespace libwebrtc
