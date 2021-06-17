#ifndef LIB_WBBRTC_RTC_RTP_PARAMETERS_HXX
#define LIB_WBBRTC_RTC_RTP_PARAMETERS_HXX

#include "rtc_types.h"
#include "base/scoped_ref_ptr.h"
#include "base/refcount.h"
#include "rtc_priority.h"

namespace libwebrtc {

enum class FecMechanism {
  RED,
  RED_AND_ULPFEC,
  FLEXFEC,
};

enum class RtcpFeedbackType {
  CCM,
  LNTF, 
  NACK,
  REMB, 
  TRANSPORT_CC,
};

enum class RtcpFeedbackMessageType {
  GENERIC_NACK,
  PLI, 
  FIR, 
};

enum class DtxStatus {
  DISABLED,
  ENABLED,
};


enum class DegradationPreference {
  DISABLED,
  MAINTAIN_FRAMERATE,
  MAINTAIN_RESOLUTION,
  BALANCED,
};

struct RTCRtpParameters : public RefCountInterface {
 public:
  //static scoped_refptr<RTCRtpParameters> Create();
  //virtual String GetTransactionId() = 0;
  //virtual void SetTransactionId(String value) = 0;

  //virtual String GetMid() = 0;
  //virtual void SetMid(String value) = 0;

  //virtual Vector<RtpCodecParameters> GetCodecs() = 0;
  //virtual void SetCodecs(Vector<RtpCodecParameters> value) = 0;

  //virtual Vector<RtpExtension> GetHeaderExtensions() = 0;
  //virtual void SetHeaderExtensions(Vector<RtpExtension> value) = 0;

  //virtual Vector<RtpEncodingParameters> GetEncodings() = 0;
  //virtual void SetEncodings(Vector<RtpEncodingParameters> value) = 0;

  //virtual RtcpParameters GetRtcp() = 0;
  //virtual void SetRtcp(RtcpParameters value) = 0;

  //virtual DegradationPreference GetDegradationPreference() = 0;
  //virtual void SetDegradationPreference(DegradationPreference value) = 0;

  //virtual bool operator==(const RtpParameters& o) const = 0;
  //virtual bool operator!=(const RtpParameters& o) const = 0;
};

class RTCRtpEncodingParameters : public RefCountInterface {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCRtpEncodingParameters> Create();

  virtual uint32_t GetSsrc() = 0;
  virtual void SetSsrc(uint32_t value) = 0;

  virtual double GetBitratePriority() = 0;
  virtual void SetBitratePriority(double value) = 0;

  virtual RTCPriority GetNetworkPriority() = 0;
  virtual void SetNetworkPriority(RTCPriority value) = 0;

  virtual int GetMaxBitrateBps() = 0;
  virtual void SetMaxBitrateBps(int value) = 0;

  virtual int GetMinBitrateBps() = 0;
  virtual void SetMinBitrateBps(int value) = 0;

  virtual double GetMaxFramerate() = 0;
  virtual void SetMaxFramerate(double value) = 0;

  virtual int GetNumTemporalLayers() = 0;
  virtual void SetNumTemporalLayers(int value) = 0;

  virtual double GetScaleResolutionDownBy() = 0;
  virtual void SetScaleResolutionDownBy(double value) = 0;

  virtual String GetScalabilityMode() = 0;
  virtual void SetScalabilityMode(String value) = 0;

  virtual bool GetActive() = 0;
  virtual void SetActive(bool value) = 0;

  virtual String GetRid() = 0;
  virtual void SetRid(String value) = 0;

  virtual bool GetAdaptivePtime() = 0;
  virtual void SetAdaptivePtime(bool value) = 0;

  virtual bool operator==(scoped_refptr<RTCRtpEncodingParameters> o) const = 0;
  virtual bool operator!=(scoped_refptr<RTCRtpEncodingParameters> o) const = 0;

};

}  // namespace libwebrtc
#endif  // WINDOWS_RTP_PARAMETERS_H
