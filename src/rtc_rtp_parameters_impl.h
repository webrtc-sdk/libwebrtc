#ifndef LIB_WBBRTC_RTC_RTP_PARAMETERS_IMPL_HXX
#define LIB_WBBRTC_RTC_RTP_PARAMETERS_IMPL_HXX

#include "api/rtp_parameters.h"
#include "rtc_rtp_parameters.h"

namespace libwebrtc {

class RTCRtpCodecParametersImpl : public RTCRtpCodecParameters {
 public:
  RTCRtpCodecParametersImpl(webrtc::RtpCodecParameters rtp_codec_parameters);

  virtual String GetMimeType() const override;
  virtual String GetName() override;
  virtual void SetName(String value) override;
  virtual RTCMediaType GetKind() override;
  virtual void SetKind(RTCMediaType value) override;
  virtual int GetPayloadType() override;
  virtual void SetPayloadType(int value) override;
  virtual int GetClockRate() override;
  virtual void SetClockRate(int value) override;
  virtual int GetNumChannels() override;
  virtual void SetNumChannels(int value) override;
  virtual int GetMaxPtime() override;
  virtual void GetMaxPtime(int value) override;
  virtual int Getptime() override;
  virtual void Setptime(int value) override;
  virtual Vector<scoped_refptr<RTCRtcpFeedback>> GetRtcpFeedback() override;
  virtual void SetRtcpFeedback(
      Vector<scoped_refptr<RTCRtcpFeedback>> value) override;
  virtual Map<String, String> GetParameters() override;
  virtual void SetParameters(Map<String, String> value) override;
  virtual bool operator==(scoped_refptr<RTCRtpCodecParameters> o) override;
  virtual bool operator!=(scoped_refptr<RTCRtpCodecParameters> o) override;

  webrtc::RtpCodecParameters rtp_codec_parameters();

 private:
  webrtc::RtpCodecParameters rtp_codec_parameters_;
};

class RTCRtpExtensionImpl : public RTCRtpExtension {
 public:
  RTCRtpExtensionImpl(webrtc::RtpExtension rtp_extension);

  virtual bool operator==(scoped_refptr<RTCRtpExtension> o) const override;
  virtual String GetUri() override;
  virtual void SetUri(String value) override;
  virtual int GetId() override;
  virtual void SetId(int value) override;
  virtual bool GetEncrypt() override;
  virtual void etEncrypt(bool value) override;
  virtual String ToString() const override;

  webrtc::RtpExtension rtp_extension();

 private:
  webrtc::RtpExtension rtp_extension_;

};

class RTCRtcpParametersImpl : public RTCRtcpParameters {
 public:
  RTCRtcpParametersImpl(webrtc::RtcpParameters rtcp_parameters);

  virtual uint32_t GetSsrc() override;
  virtual uint32_t SetSsrc() override;
  virtual String GetCname() override;
  virtual void SetCname(String value) override;
  virtual bool GetReducedSize() override;
  virtual void SetReducedSize(bool value) override;
  virtual bool GetMux() override;
  virtual void SetMux(bool value) override;
  virtual bool operator==(scoped_refptr<RTCRtcpParameters> o) const override;
  virtual bool operator!=(scoped_refptr<RTCRtcpParameters> o) const override;

  webrtc::RtcpParameters rtcp_parameters();

 private:
  webrtc::RtcpParameters rtcp_parameters_;
};

class RTCRtpParametersImpl : public RTCRtpParameters {
 public:
  RTCRtpParametersImpl(webrtc::RtpParameters rtp_parameters);

  virtual String GetTransactionId() override;
  virtual void SetTransactionId(String value) override;
  virtual String GetMid() override;
  virtual void SetMid(String value) override;
  virtual Vector<scoped_refptr<RTCRtpCodecParameters>> GetCodecs() override;
  virtual void SetCodecs(
      Vector<scoped_refptr<RTCRtpCodecParameters>> value) override;
  virtual Vector<scoped_refptr<RTCRtpExtension>> GetHeaderExtensions() override;
  virtual void SetHeaderExtensions(
      Vector<scoped_refptr<RTCRtpExtension>> value) override;
  virtual Vector<scoped_refptr<RTCRtpEncodingParameters>> GetEncodings()
      override;
  virtual void SetEncodings(
      Vector<scoped_refptr<RTCRtpEncodingParameters>> value) override;
  virtual scoped_refptr<RTCRtcpParameters> GetRtcp() override;
  virtual void SetRtcp(scoped_refptr<RTCRtcpParameters> value) override;
  virtual bool operator==(scoped_refptr<RTCRtpParameters> o) const override;
  virtual bool operator!=(scoped_refptr<RTCRtpParameters> o) const override;

  webrtc::RtpParameters rtp_parameters();

 private:
  webrtc::RtpParameters rtp_parameters_;
};

class RTCRtpEncodingParametersImpl : public RTCRtpEncodingParameters {
 public:
  RTCRtpEncodingParametersImpl();

  RTCRtpEncodingParametersImpl(
      webrtc::RtpEncodingParameters& rtp_encoding_parameters);

  virtual uint32_t GetSsrc() override;
  virtual void SetSsrc(uint32_t value) override;

  virtual double GetBitratePriority() override;
  virtual void SetBitratePriority(double value) override;

  virtual RTCPriority GetNetworkPriority() override;
  virtual void SetNetworkPriority(RTCPriority value) override;

  virtual int GetMaxBitrateBps() override;
  virtual void SetMaxBitrateBps(int value) override;

  virtual int GetMinBitrateBps() override;
  virtual void SetMinBitrateBps(int value) override;

  virtual double GetMaxFramerate() override;
  virtual void SetMaxFramerate(double value) override;

  virtual int GetNumTemporalLayers() override;
  virtual void SetNumTemporalLayers(int value) override;

  virtual double GetScaleResolutionDownBy() override;
  virtual void SetScaleResolutionDownBy(double value) override;

  virtual String GetScalabilityMode() override;
  virtual void SetScalabilityMode(String value) override;

  virtual bool GetActive() override;
  virtual void SetActive(bool value) override;

  virtual String GetRid() override;
  virtual void SetRid(String value) override;

  virtual bool GetAdaptivePtime() override;
  virtual void SetAdaptivePtime(bool value) override;

  virtual bool operator==(
      scoped_refptr<RTCRtpEncodingParameters> o) const override;
  virtual bool operator!=(
      scoped_refptr<RTCRtpEncodingParameters> o) const override;

  webrtc::RtpEncodingParameters rtp_parameters();

 private:
  webrtc::RtpEncodingParameters rtp_encoding_parameters_;
};

}  // namespace libwebrtc
#endif  // WINDOWS_RTP_PARAMETERS_H