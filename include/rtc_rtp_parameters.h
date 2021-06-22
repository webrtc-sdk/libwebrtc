#ifndef LIB_WBBRTC_RTC_RTP_PARAMETERS_HXX
#define LIB_WBBRTC_RTC_RTP_PARAMETERS_HXX

#include "base/refcount.h"
#include "base/scoped_ref_ptr.h"
#include "rtc_media_types.h"
#include "rtc_priority.h"
#include "rtc_rtp_transceiver_direction.h"
#include "rtc_types.h"

namespace libwebrtc {

enum class RTCFecMechanism {
  RED,
  RED_AND_ULPFEC,
  FLEXFEC,
};

enum class RTCRtcpFeedbackType {
  CCM,
  LNTF,
  NACK,
  REMB,
  TRANSPORT_CC,
};

enum class RTCRtcpFeedbackMessageType {
  GENERIC_NACK,
  PLI,
  FIR,
};

enum class RTCDtxStatus {
  DISABLED,
  ENABLED,
};

enum class RTCDegradationPreference {
  DISABLED,
  MAINTAIN_FRAMERATE,
  MAINTAIN_RESOLUTION,
  BALANCED,
};

class RTCRtcpFeedback : public RefCountInterface {
  virtual RTCRtcpFeedbackType GetType() = 0;
  virtual void GetType(RTCRtcpFeedbackType value) = 0;

  virtual RTCRtcpFeedbackMessageType GetMessageType() = 0;
  virtual void GetMessageType(RTCRtcpFeedbackMessageType value) = 0;

  virtual bool operator==(scoped_refptr<RTCRtcpFeedback> o) = 0;
  virtual bool operator!=(scoped_refptr<RTCRtcpFeedback> o) = 0;
};

class RTCRtpCodecCapability : public RefCountInterface {
  virtual void GetMimeType(OnString on) const = 0;

  virtual void GetName(OnString on) = 0;
  virtual void SetName(char* p,size_t size) = 0;

  virtual RTCMediaType GetKind() = 0;
  virtual void SetKind(RTCMediaType value) = 0;

  virtual int GetClockRate() = 0;
  virtual void SetClockRate(int value) = 0;

  virtual int GetPreferredPayloadType() = 0;
  virtual void SetPreferredPayloadType(int value) = 0;

  virtual int GetMaxPtime() = 0;
  virtual void SetMaxPtime(int value) = 0;

  virtual int GetPtime() = 0;
  virtual void SetPtime(int value) = 0;

  virtual int GetNumChannels() = 0;
  virtual void SetNumChannels(int value) = 0;

  virtual Vector<RTCRtcpFeedback> GetRtcpFeedback() = 0;
  virtual void SetRtcpFeedback(Vector<RTCRtcpFeedback> value) = 0;

  virtual void GetParameters(OnStringAndString on) = 0;
  virtual void SetParameters(OnMapStringAndString on) = 0;

  virtual void GetOptions(OnStringAndString on) = 0;
  virtual void SetOptions(OnMapStringAndString on) = 0;

  virtual int GetMaxTemporalLayerExtensions() = 0;
  virtual void SetMaxTemporalLayerExtensions(int value) = 0;

  virtual int GetMaxSpatialLayerExtensions() = 0;
  virtual void SetMaxSpatialLayerExtensions(int value) = 0;

  virtual bool GetSvcMultiStreamSupport() = 0;
  virtual void SetSvcMultiStreamSupport(bool value) = 0;

  virtual bool operator==(scoped_refptr<RTCRtpCodecCapability> o) const = 0;
  virtual bool operator!=(scoped_refptr<RTCRtpCodecCapability> o) const = 0;
};

class RTCRtpHeaderExtensionCapability : public RefCountInterface {
  virtual void GetUri(OnString on) = 0;
  virtual void SetUri(char* p, size_t size) = 0;

  virtual int GetPreferredId() = 0;
  virtual void SetPreferredId(int value) = 0;

  virtual bool GetPreferredEncrypt() = 0;
  virtual void SetPreferredEncrypt(bool value) = 0;

  virtual RTCRtpTransceiverDirection GetDirection() = 0;
  virtual void SetDirection(RTCRtpTransceiverDirection value) = 0;

  virtual bool operator==(
      scoped_refptr<RTCRtpHeaderExtensionCapability> o) const = 0;
  virtual bool operator!=(
      scoped_refptr<RTCRtpHeaderExtensionCapability> o) const = 0;
};

class RTCRtpExtension : public RefCountInterface {
 public:
  enum RTCFilter {
    kDiscardEncryptedExtension,
    kPreferEncryptedExtension,
    kRequireEncryptedExtension,
  };

  static bool IsSupportedForAudio(char* uri, size_t size);
  static bool IsSupportedForVideo(char* uri, size_t size);
  static bool IsEncryptionSupported(char* uri, size_t size);

  static const scoped_refptr<RTCRtpExtension> FindHeaderExtensionByUri(
      const Vector<scoped_refptr<RTCRtpExtension>>& extensions,
      char* uri,
      size_t size,
      RTCFilter filter);

  static const scoped_refptr<RTCRtpExtension> FindHeaderExtensionByUri(
      const Vector<scoped_refptr<RTCRtpExtension>>& extensions,
      char* uri,
      size_t size);

  static const scoped_refptr<RTCRtpExtension>
  FindHeaderExtensionByUriAndEncryption(
      const Vector<scoped_refptr<RTCRtpExtension>>& extensions,
      char* uri,
      size_t size,
      bool encrypt);

  static const Vector<scoped_refptr<RTCRtpExtension>>
  DeduplicateHeaderExtensions(
      const Vector<scoped_refptr<RTCRtpExtension>>& extensions,
      RTCFilter filter);

  static char GetkEncryptHeaderExtensionsUri();
  static char GetkAudioLevelUri();
  static char GetkTimestampOffsetUri();
  static char GetkAbsSendTimeUri();
  static char GetkAbsoluteCaptureTimeUri();
  static char GetkVideoRotationUri();
  static char GetkVideoContentTypeUri();
  static char GetkVideoTimingUri();
  static char GetkGenericFrameDescriptorUri00();
  static char GetkDependencyDescriptorUri();
  static char GetkVideoLayersAllocationUri();
  static char GetkTransportSequenceNumberUri();
  static char GetkTransportSequenceNumberV2Uri();
  static char GetkPlayoutDelayUri();
  static char GetkColorSpaceUri();
  static char GetkMidUri();
  static char GetkRidUri();
  static char GetkRepairedRidUri();
  static char GetkVideoFrameTrackingIdUri();
  static char GetkCsrcAudioLevelsUri();

  static int GetkMinId();
  static int GetkMaxId();
  static int GetkMaxValueSize();
  static int GetkOneByteHeaderExtensionMaxId();
  static int GetkOneByteHeaderExtensionMaxValueSize();

  virtual void ToString(OnString on) const = 0;
  virtual bool operator==(scoped_refptr<RTCRtpExtension> o) const = 0;

  virtual void GetUri(OnString on) = 0;
  virtual void SetUri(char* uri, size_t size) = 0;

  virtual int GetId() = 0;
  virtual void SetId(int value) = 0;

  virtual bool GetEncrypt() = 0;
  virtual void SetEncrypt(bool value) = 0;
};

class RtpFecParameters : public RefCountInterface {
  virtual uint32_t GetSsrc() = 0;
  virtual void SetSsrc(uint32_t value) = 0;

  virtual RTCFecMechanism GetMechanism() = 0;
  virtual void SetMechanism(RTCFecMechanism value) = 0;

  virtual bool operator==(const RtpFecParameters& o) const = 0;
  virtual bool operator!=(const RtpFecParameters& o) const = 0;
};

class RTCRtpRtxParameters : public RefCountInterface {
  virtual uint32_t GetSsrc() = 0;
  virtual void SetSsrc(uint32_t value) = 0;

  virtual bool operator==(scoped_refptr<RTCRtpRtxParameters> o) const = 0;

  virtual bool operator!=(scoped_refptr<RTCRtpRtxParameters> o) const = 0;
};

class RTCRtpCodecParameters : public RefCountInterface {
 public:
  virtual void GetMimeType(OnString on) const = 0;

  virtual void GetName(OnString on) = 0;
  virtual void SetName(char* p, size_t size) = 0;

  virtual RTCMediaType GetKind() = 0;
  virtual void SetKind(RTCMediaType value) = 0;

  virtual int GetPayloadType() = 0;
  virtual void SetPayloadType(int value) = 0;

  virtual int GetClockRate() = 0;
  virtual void SetClockRate(int value) = 0;

  virtual int GetNumChannels() = 0;
  virtual void SetNumChannels(int value) = 0;

  virtual int GetMaxPtime() = 0;
  virtual void GetMaxPtime(int value) = 0;

  virtual int GetPtime() = 0;
  virtual void SetPtime(int value) = 0;

  virtual Vector<scoped_refptr<RTCRtcpFeedback>> GetRtcpFeedback() = 0;
  virtual void SetRtcpFeedback(
      Vector<scoped_refptr<RTCRtcpFeedback>> value) = 0;

  virtual void GetParameters(OnStringAndString on) = 0;
  virtual void SetParameters(OnMapStringAndString on) = 0;

  virtual bool operator==(scoped_refptr<RTCRtpCodecParameters> o) = 0;
  virtual bool operator!=(scoped_refptr<RTCRtpCodecParameters> o) = 0;
};

class RTCRtpCapabilities : public RefCountInterface {
  virtual Vector<scoped_refptr<RTCRtpCodecCapability>> GetCodecs() = 0;
  virtual void SetCodecs(
      Vector<scoped_refptr<RTCRtpCodecCapability>> value) = 0;

  virtual Vector<scoped_refptr<RTCRtpHeaderExtensionCapability>>
  GetHeaderExtensions() = 0;
  virtual void SetHeaderExtensions(
      Vector<scoped_refptr<RTCRtpHeaderExtensionCapability>> value) = 0;

  virtual Vector<RTCFecMechanism> GetFec() = 0;
  virtual void SetFec(Vector<RTCFecMechanism>) = 0;

  virtual bool operator==(scoped_refptr<RTCRtpCapabilities> o) = 0;
  virtual bool operator!=(scoped_refptr<RTCRtpCapabilities> o) = 0;
};

class RTCRtcpParameters : public RefCountInterface {
 public:
  virtual uint32_t GetSsrc() = 0;
  virtual void SetSsrc(uint32_t value) = 0;

  virtual void GetCname(OnString on) = 0;
  virtual void SetCname(char* p, size_t size) = 0;

  virtual bool GetReducedSize() = 0;
  virtual void SetReducedSize(bool value) = 0;

  virtual bool GetMux() = 0;
  virtual void SetMux(bool value) = 0;

  virtual bool operator==(scoped_refptr<RTCRtcpParameters> o) const = 0;
  virtual bool operator!=(scoped_refptr<RTCRtcpParameters> o) const = 0;
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

  virtual void GetScalabilityMode(OnString on) = 0;
  virtual void SetScalabilityMode(char* p,size_t size) = 0;

  virtual bool GetActive() = 0;
  virtual void SetActive(bool value) = 0;

  virtual void GetRid(OnString on) = 0;
  virtual void SetRid(char* p, size_t size) = 0;

  virtual bool GetAdaptivePtime() = 0;
  virtual void SetAdaptivePtime(bool value) = 0;

  virtual bool operator==(scoped_refptr<RTCRtpEncodingParameters> o) const = 0;
  virtual bool operator!=(scoped_refptr<RTCRtpEncodingParameters> o) const = 0;
};

struct RTCRtpParameters : public RefCountInterface {
 public:
  // static scoped_refptr<RTCRtpParameters> Create();
  virtual void GetTransactionId(OnString on) = 0;
  virtual void SetTransactionId(char* p, size_t size) = 0;

  virtual void GetMid(OnString on) = 0;
  virtual void SetMid(char* p, size_t size) = 0;

  virtual Vector<scoped_refptr<RTCRtpCodecParameters>> GetCodecs() = 0;
  virtual void SetCodecs(
      Vector<scoped_refptr<RTCRtpCodecParameters>> value) = 0;

  virtual Vector<scoped_refptr<RTCRtpExtension>> GetHeaderExtensions() = 0;
  virtual void SetHeaderExtensions(
      Vector<scoped_refptr<RTCRtpExtension>> value) = 0;

  virtual Vector<scoped_refptr<RTCRtpEncodingParameters>> GetEncodings() = 0;
  virtual void SetEncodings(
      Vector<scoped_refptr<RTCRtpEncodingParameters>> value) = 0;

  virtual scoped_refptr<RTCRtcpParameters> GetRtcp() = 0;
  virtual void SetRtcp(scoped_refptr<RTCRtcpParameters> value) = 0;

  // virtual DegradationPreference GetDegradationPreference() = 0;
  // virtual void SetDegradationPreference(DegradationPreference value) = 0;

  virtual bool operator==(scoped_refptr<RTCRtpParameters> o) const = 0;
  virtual bool operator!=(scoped_refptr<RTCRtpParameters> o) const = 0;
};

}  // namespace libwebrtc
#endif  // LIB_WBBRTC_RTC_RTP_PARAMETERS_HXX