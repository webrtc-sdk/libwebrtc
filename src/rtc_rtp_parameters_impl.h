#ifndef LIB_WBBRTC_RTC_RTP_PARAMETERS_IMPL_HXX
#define LIB_WBBRTC_RTC_RTP_PARAMETERS_IMPL_HXX

#include "api/rtp_parameters.h"
#include "rtc_rtp_parameters.h"

namespace libwebrtc {

class RtpParametersImpl : public RTCRtpParameters {
 public:
  RtpParametersImpl(webrtc::RtpParameters rtp_parameters);
  /* ~RtpParametersImpl();

   virtual String GetTransactionId() override;
   virtual void SetTransactionId(String value) override;

   virtual String GetMid() override;
   virtual void SetMid(String value) override;

   virtual std::vector<RtpCodecParameters> GetCodecs() override;
   virtual void SetCodecs(std::vector<RtpCodecParameters> value) override;

   virtual std::vector<RtpExtension> GetHeaderExtensions() override;
   virtual void SetHeaderExtensions(std::vector<RtpExtension> value) override;

   virtual std::vector<RtpEncodingParameters> GetEncodings() override;
   virtual void SetEncodings(std::vector<RtpEncodingParameters> value) override;

   virtual RtcpParameters GetRtcp() override;
   virtual void SetRtcp(RtcpParameters value) override;

   virtual DegradationPreference GetDegradationPreference() override;
   virtual void SetDegradationPreference(DegradationPreference value) override;

   virtual bool operator==(const RtpParameters& o) const override;
   virtual bool operator!=(const RtpParameters& o) const override;*/

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
