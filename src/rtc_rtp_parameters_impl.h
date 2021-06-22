#ifndef LIB_WBBRTC_RTC_RTP_PARAMETERS_IMPL_HXX
#define LIB_WBBRTC_RTC_RTP_PARAMETERS_IMPL_HXX

#include "api/rtp_parameters.h"
#include "rtc_rtp_parameters.h"

namespace libwebrtc {

class RTCRtcpFeedbackImpl : public RTCRtcpFeedback {
 public:
  RTCRtcpFeedbackImpl(webrtc::RtcpFeedback rtcp_feedback);

  virtual RTCRtcpFeedbackType GetType() override;
  virtual void GetType(RTCRtcpFeedbackType value) override;
  virtual RTCRtcpFeedbackMessageType GetMessageType() override;
  virtual void GetMessageType(RTCRtcpFeedbackMessageType value) override;
  virtual bool operator==(scoped_refptr<RTCRtcpFeedback> o) override;
  virtual bool operator!=(scoped_refptr<RTCRtcpFeedback> o) override;

  webrtc::RtcpFeedback rtcp_feedback();

 private:
  webrtc::RtcpFeedback rtcp_feedback_;

};

class RTCRtpCodecParametersImpl : public RTCRtpCodecParameters {
 public:
  RTCRtpCodecParametersImpl(webrtc::RtpCodecParameters rtp_codec_parameters);

  virtual void GetMimeType(OnString on) const override;
  virtual void GetName(OnString on) override;
  virtual void SetName(char* p ,size_t size) override;
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
  virtual int GetPtime() override;
  virtual void SetPtime(int value) override;
  virtual Vector<scoped_refptr<RTCRtcpFeedback>> GetRtcpFeedback() override;
  virtual void SetRtcpFeedback(
      Vector<scoped_refptr<RTCRtcpFeedback>> value) override;
  virtual void GetParameters(OnStringAndString on) override;
  virtual void SetParameters(OnMapStringAndString on) override;
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
  virtual void GetUri(OnString on) override;
  virtual void SetUri(char* uri,size_t size) override;
  virtual int GetId() override;
  virtual void SetId(int value) override;
  virtual bool GetEncrypt() override;
  virtual void SetEncrypt(bool value) override;
  virtual void ToString(OnString on) const override;

  webrtc::RtpExtension rtp_extension();

 private:
  webrtc::RtpExtension rtp_extension_;

};

class RTCRtcpParametersImpl : public RTCRtcpParameters {
 public:
  RTCRtcpParametersImpl(webrtc::RtcpParameters rtcp_parameters);

  virtual uint32_t GetSsrc() override;
  virtual void SetSsrc(uint32_t value) override;
  virtual void GetCname(OnString on) override;
  virtual void SetCname(char* p, size_t size) override;
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

  virtual void GetTransactionId(OnString on) override;
  virtual void SetTransactionId(char* p, size_t size) override;
  virtual void GetMid(OnString on) override;
  virtual void SetMid(char* p, size_t size) override;
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

  virtual void GetScalabilityMode(OnString on) override;
  virtual void SetScalabilityMode(char* p, size_t size) override;

  virtual bool GetActive() override;
  virtual void SetActive(bool value) override;

  virtual void GetRid(OnString on) override;
  virtual void SetRid(char* p, size_t size) override;

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