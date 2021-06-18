#ifndef LIB_WEBRTC_RTC_RTP_SENDER_IMPL_HXX
#define LIB_WEBRTC_RTC_RTP_SENDER_IMPL_HXX

#include "api/rtp_sender_interface.h"
#include "api/scoped_refptr.h"
#include "rtc_rtp_sender.h"

namespace libwebrtc {
class RTCRtpSenderImpl : public RTCRtpSender {
 public:
  RTCRtpSenderImpl(rtc::scoped_refptr<webrtc::RtpSenderInterface> rtp_sender);

  virtual bool SetTrack(scoped_refptr<RTCMediaTrack> track) override;
  virtual scoped_refptr<RTCMediaTrack> GetTrack() const override;
  virtual scoped_refptr<RTCDtlsTransport> GetDtlsTransport() const override;
  virtual uint32_t GetSsrc() const override;
  virtual RTCMediaType GetMediaType() const override;
  virtual String Id() const override;
  virtual Vector<String> GetStreamIds() const override;
  virtual Vector<scoped_refptr<RTCRtpEncodingParameters>> InitSendEncodings()
      const override;
  virtual scoped_refptr<RTCRtpParameters> GetParameters() const override;
  virtual const char* SetParameters(
      const scoped_refptr<RTCRtpParameters> parameters) override;
  virtual scoped_refptr<RTCDtmfSender> GetDtmfSender() const override;

  rtc::scoped_refptr<webrtc::RtpSenderInterface> rtp_sender();

 private:
  rtc::scoped_refptr<webrtc::RtpSenderInterface> rtp_sender_;

};
}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_RTP_SENDER_IMPL_HXX