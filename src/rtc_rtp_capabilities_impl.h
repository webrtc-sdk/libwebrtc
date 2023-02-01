#ifndef LIB_WBBRTC_RTC_RTP_CAPABILITIES_IMPL_HXX
#define LIB_WBBRTC_RTC_RTP_CAPABILITIES_IMPL_HXX

#include "api/rtp_parameters.h"
#include "rtc_rtp_capabilities.h"

namespace libwebrtc {

class RTCRtpCapabilitiesImpl : public RTCRtpCapabilities {
 public:
  RTCRtpCapabilitiesImpl(webrtc::RtpCapabilities rtp_capabilities);
  virtual const vector<scoped_refptr<RTCRtpCodecCapability>> codecs() override;
  virtual void set_codecs(
      vector<scoped_refptr<RTCRtpCodecCapability>> codecs) override;
  virtual const vector<scoped_refptr<RTCRtpHeaderExtensionCapability>>
  header_extensions() override;
  virtual void set_header_extensions(
      vector<scoped_refptr<RTCRtpHeaderExtensionCapability>> header_extensions)
      override;

  webrtc::RtpCapabilities rtp_capabilities();

 private:
  webrtc::RtpCapabilities rtp_capabilities_;
};

class RTCRtpCodecCapabilityImpl : public RTCRtpCodecCapability {
 public:
  RTCRtpCodecCapabilityImpl() {}
  RTCRtpCodecCapabilityImpl(webrtc::RtpCodecCapability rtp_codec_capability);
  virtual void set_mime_type(const string& mime_type) override;
  virtual void set_clock_rate(int clock_rate) override;
  virtual void set_channels(int channels) override;
  virtual void set_sdp_fmtp_line(const string& sdp_fmtp_line) override;

  virtual string mime_type() const override;
  virtual int clock_rate() const override;
  virtual int channels() const override;
  virtual string sdp_fmtp_line() const override;

  webrtc::RtpCodecCapability rtp_codec_capability();

 private:
  webrtc::RtpCodecCapability rtp_codec_capability_;
};

class RTCRtpHeaderExtensionCapabilityImpl
    : public RTCRtpHeaderExtensionCapability {
 public:
  RTCRtpHeaderExtensionCapabilityImpl(
      webrtc::RtpHeaderExtensionCapability rtp_header_extension_capability);
  virtual const string uri() override;
  virtual void set_uri(const string uri) override;

  virtual int preferred_id() override;
  virtual void set_preferred_id(int value) override;

  virtual bool preferred_encrypt() override;
  virtual void set_preferred_encrypt(bool value) override;

  webrtc::RtpHeaderExtensionCapability rtp_header_extension_capability();

 private:
  webrtc::RtpHeaderExtensionCapability rtp_header_extension_capability_;
};

}  // namespace libwebrtc

#endif  // LIB_WBBRTC_RTC_RTP_CAPABILITIES_IMPL_HXX
