#ifndef LIB_WBBRTC_RTC_RTP_PARAMETERS_HXX
#define LIB_WBBRTC_RTC_RTP_PARAMETERS_HXX

#include "base/refcount.h"
#include "base/scoped_ref_ptr.h"
#include "rtc_types.h"
#include "rtc_base_list.h"

namespace libwebrtc {

enum class RTCRtpTransceiverDirection {
  kSendRecv,
  kSendOnly,
  kRecvOnly,
  kInactive,
  kStopped,
};

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
 public:
  virtual RTCRtcpFeedbackType type() = 0;
  virtual void set_type(RTCRtcpFeedbackType value) = 0;

  virtual RTCRtcpFeedbackMessageType message_type() = 0;
  virtual void set_message_type(RTCRtcpFeedbackMessageType value) = 0;

  virtual bool operator==(scoped_refptr<RTCRtcpFeedback> o) = 0;
  virtual bool operator!=(scoped_refptr<RTCRtcpFeedback> o) = 0;
};

/**
 * class RTCRtcpFeedbackList
 */
class RTCRtcpFeedbackList : public RTCBaseList<scoped_refptr<RTCRtcpFeedback>> {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCRtcpFeedbackList> Create(
    const vector<scoped_refptr<RTCRtcpFeedback>>& source);

 protected:
  RTCRtcpFeedbackList(const vector<scoped_refptr<RTCRtcpFeedback>>& source)
    : RTCBaseList<scoped_refptr<RTCRtcpFeedback>>(source)
  {}

  ~RTCRtcpFeedbackList() {}
};

class RTCRtpExtension : public RefCountInterface {
 public:
  enum RTCFilter {
    kDiscardEncryptedExtension,
    kPreferEncryptedExtension,
    kRequireEncryptedExtension,
  };

  virtual const string ToString() const = 0;
  virtual bool operator==(scoped_refptr<RTCRtpExtension> o) const = 0;

  virtual const string uri() = 0;
  virtual void set_uri(const string uri) = 0;

  virtual int id() = 0;
  virtual void set_id(int value) = 0;

  virtual bool encrypt() = 0;
  virtual void set_encrypt(bool value) = 0;
};

/**
 * class RTCRtpExtensionList
 */
class RTCRtpExtensionList : public RTCBaseList<scoped_refptr<RTCRtpExtension>> {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCRtpExtensionList> Create(
    const vector<scoped_refptr<RTCRtpExtension>>& source);

 protected:
  RTCRtpExtensionList(const vector<scoped_refptr<RTCRtpExtension>>& source)
    : RTCBaseList<scoped_refptr<RTCRtpExtension>>(source)
  {}

  ~RTCRtpExtensionList() {}
};

class RtpFecParameters : public RefCountInterface {
  virtual uint32_t ssrc() = 0;
  virtual void set_ssrc(uint32_t value) = 0;

  virtual RTCFecMechanism mechanism() = 0;
  virtual void set_mechanism(RTCFecMechanism value) = 0;

  virtual bool operator==(const RtpFecParameters& o) const = 0;
  virtual bool operator!=(const RtpFecParameters& o) const = 0;
};

class RTCRtpRtxParameters : public RefCountInterface {
  virtual uint32_t ssrc() = 0;
  virtual void set_ssrc(uint32_t value) = 0;

  virtual bool operator==(scoped_refptr<RTCRtpRtxParameters> o) const = 0;

  virtual bool operator!=(scoped_refptr<RTCRtpRtxParameters> o) const = 0;
};

/**
 * class RTCRtpCodecParametersPair
 */
class RTCRtpCodecParametersPair : public RTCBasePair<string, string> {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCRtpCodecParametersPair> Create(
    const std::pair<string, string>& source);

  LIB_WEBRTC_API static scoped_refptr<RTCRtpCodecParametersPair> Create();

 protected:
  RTCRtpCodecParametersPair()
   : RTCBasePair<string, string>()
  {}

  RTCRtpCodecParametersPair(const std::pair<string, string>& source)
   : RTCBasePair<string, string>(source)
   {}

  ~RTCRtpCodecParametersPair() {}
};

/**
 * class RTCRtpCodecParametersMap
 */
class RTCRtpCodecParametersMap : public RTCBaseList<scoped_refptr<RTCRtpCodecParametersPair>> {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCRtpCodecParametersMap> Create(
    const vector<scoped_refptr<RTCRtpCodecParametersPair>>& source);
  
  LIB_WEBRTC_API static scoped_refptr<RTCRtpCodecParametersMap> Create();

 protected:
  RTCRtpCodecParametersMap()
    : RTCBaseList<scoped_refptr<RTCRtpCodecParametersPair>>()
  {}

  RTCRtpCodecParametersMap(const vector<scoped_refptr<RTCRtpCodecParametersPair>>& source)
    : RTCBaseList<scoped_refptr<RTCRtpCodecParametersPair>>(source)
  {}

  ~RTCRtpCodecParametersMap() {}

 public:
  virtual vector<std::pair<string, string>> to_parameters() = 0;
};

class RTCRtpCodecParameters : public RefCountInterface {
 public:
  virtual const string mime_type() const = 0;

  virtual const string name() = 0;
  virtual void set_name(const string name) = 0;

  virtual RTCMediaType kind() = 0;
  virtual void set_kind(RTCMediaType value) = 0;

  virtual int payload_type() = 0;
  virtual void set_payload_type(int value) = 0;

  virtual int clock_rate() = 0;
  virtual void set_clock_rate(int value) = 0;

  virtual int num_channels() = 0;
  virtual void set_num_channels(int value) = 0;

  virtual int max_ptime() = 0;
  virtual void set_max_ptime(int value) = 0;

  virtual int ptime() = 0;
  virtual void set_ptime(int value) = 0;

  virtual const vector<scoped_refptr<RTCRtcpFeedback>> rtcp_feedback() = 0;
  virtual void set_rtcp_feedback(
      const vector<scoped_refptr<RTCRtcpFeedback>> feecbacks) = 0;

  virtual const vector<std::pair<string, string>> parameters() = 0;
  virtual void set_parameters(const map<string, string> parameters) = 0;
  virtual void set_parameters(const vector<std::pair<string, string>> parameters) = 0;

  virtual bool operator==(scoped_refptr<RTCRtpCodecParameters> o) = 0;
  virtual bool operator!=(scoped_refptr<RTCRtpCodecParameters> o) = 0;

 protected:
  virtual ~RTCRtpCodecParameters() {}
};

/**
 * class RTCRtpCodecParametersList
 */
class RTCRtpCodecParametersList : public RTCBaseList<scoped_refptr<RTCRtpCodecParameters>> {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCRtpCodecParametersList> Create(
    const vector<scoped_refptr<RTCRtpCodecParameters>>& source);

 protected:
  RTCRtpCodecParametersList(const vector<scoped_refptr<RTCRtpCodecParameters>>& source)
    : RTCBaseList<scoped_refptr<RTCRtpCodecParameters>>(source)
  {}

  ~RTCRtpCodecParametersList() {}
};

class RTCRtcpParameters : public RefCountInterface {
 public:
  virtual uint32_t ssrc() = 0;
  virtual void set_ssrc(uint32_t value) = 0;

  virtual const string cname() = 0;
  virtual void set_cname(const string) = 0;

  virtual bool reduced_size() = 0;
  virtual void set_reduced_size(bool value) = 0;

  virtual bool mux() = 0;
  virtual void set_mux(bool value) = 0;

  virtual bool operator==(scoped_refptr<RTCRtcpParameters> o) const = 0;
  virtual bool operator!=(scoped_refptr<RTCRtcpParameters> o) const = 0;
};

/**
 * class RTCRtcpParametersList
 */
class RTCRtcpParametersList : public RTCBaseList<scoped_refptr<RTCRtcpParameters>> {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCRtcpParametersList> Create(
    const vector<scoped_refptr<RTCRtcpParameters>>& source);

 protected:
  RTCRtcpParametersList(const vector<scoped_refptr<RTCRtcpParameters>>& source)
    : RTCBaseList<scoped_refptr<RTCRtcpParameters>>(source)
  {}

  ~RTCRtcpParametersList() {}
};

enum class RTCPriority {
  kVeryLow,
  kLow,
  kMedium,
  kHigh,
};

class RTCRtpEncodingParameters : public RefCountInterface {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCRtpEncodingParameters> Create();

  virtual uint32_t ssrc() = 0;
  virtual void set_ssrc(uint32_t value) = 0;

  virtual double bitrate_priority() = 0;
  virtual void set_bitrate_priority(double value) = 0;

  virtual RTCPriority network_priority() = 0;
  virtual void set_network_priority(RTCPriority value) = 0;

  virtual int max_bitrate_bps() = 0;
  virtual void set_max_bitrate_bps(int value) = 0;

  virtual int min_bitrate_bps() = 0;
  virtual void set_min_bitrate_bps(int value) = 0;

  virtual double max_framerate() = 0;
  virtual void set_max_framerate(double value) = 0;

  virtual int num_temporal_layers() = 0;
  virtual void set_num_temporal_layers(int value) = 0;

  virtual double scale_resolution_down_by() = 0;
  virtual void set_scale_resolution_down_by(double value) = 0;

  virtual const string scalability_mode() = 0;
  virtual void set_scalability_mode(const string mode) = 0;

  virtual bool active() = 0;
  virtual void set_active(bool value) = 0;

  virtual const string rid() = 0;
  virtual void set_rid(const string rid) = 0;

  virtual bool adaptive_ptime() = 0;
  virtual void set_adaptive_ptime(bool value) = 0;

  virtual bool operator==(scoped_refptr<RTCRtpEncodingParameters> o) const = 0;
  virtual bool operator!=(scoped_refptr<RTCRtpEncodingParameters> o) const = 0;
};

/**
 * class RTCRtpEncodingParametersList
 */
class RTCRtpEncodingParametersList : public RTCBaseList<scoped_refptr<RTCRtpEncodingParameters>> {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCRtpEncodingParametersList> Create(
    const vector<scoped_refptr<RTCRtpEncodingParameters>>& source);

 protected:
  RTCRtpEncodingParametersList(const vector<scoped_refptr<RTCRtpEncodingParameters>>& source)
    : RTCBaseList<scoped_refptr<RTCRtpEncodingParameters>>(source)
  {}

  ~RTCRtpEncodingParametersList() {}
};

struct RTCRtpParameters : public RefCountInterface {
 public:
  // static scoped_refptr<RTCRtpParameters> Create();
  virtual const string transaction_id() = 0;
  virtual void set_transaction_id(const string id) = 0;

  virtual const string mid() = 0;
  virtual void set_mid(const string mid) = 0;

  virtual const vector<scoped_refptr<RTCRtpCodecParameters>> codecs() = 0;
  virtual void set_codecs(
      const vector<scoped_refptr<RTCRtpCodecParameters>> codecs) = 0;

  virtual const vector<scoped_refptr<RTCRtpExtension>> header_extensions() = 0;
  virtual void set_header_extensions(
      const vector<scoped_refptr<RTCRtpExtension>> header_extensions) = 0;

  virtual const vector<scoped_refptr<RTCRtpEncodingParameters>> encodings() = 0;
  virtual void set_encodings(
      const vector<scoped_refptr<RTCRtpEncodingParameters>> encodings) = 0;

  virtual scoped_refptr<RTCRtcpParameters> rtcp_parameters() = 0;
  virtual void set_rtcp_parameters(
      scoped_refptr<RTCRtcpParameters> rtcp_parameters) = 0;

  virtual RTCDegradationPreference GetDegradationPreference() = 0;
  virtual void SetDegradationPreference(RTCDegradationPreference value) = 0;

  virtual bool operator==(scoped_refptr<RTCRtpParameters> o) const = 0;
  virtual bool operator!=(scoped_refptr<RTCRtpParameters> o) const = 0;
};

}  // namespace libwebrtc
#endif  // LIB_WBBRTC_RTC_RTP_PARAMETERS_HXX