#ifndef LIB_WBBRTC_RTC_RTP_PARAMETERS_IMPL_HXX
#define LIB_WBBRTC_RTC_RTP_PARAMETERS_IMPL_HXX

#include "api/rtp_parameters.h"
#include "rtc_rtp_parameters.h"

namespace libwebrtc {

class RTCRtcpFeedbackImpl : public RTCRtcpFeedback {
 public:
  RTCRtcpFeedbackImpl(webrtc::RtcpFeedback rtcp_feedback);

  virtual RTCRtcpFeedbackType type() override;
  virtual void set_type(RTCRtcpFeedbackType value) override;
  virtual RTCRtcpFeedbackMessageType message_type() override;
  virtual void set_message_type(RTCRtcpFeedbackMessageType value) override;
  virtual bool operator==(scoped_refptr<RTCRtcpFeedback> o) override;
  virtual bool operator!=(scoped_refptr<RTCRtcpFeedback> o) override;

  webrtc::RtcpFeedback rtcp_feedback();

 private:
  webrtc::RtcpFeedback rtcp_feedback_;
};

class RTCRtpCodecParametersImpl : public RTCRtpCodecParameters {
 public:
  RTCRtpCodecParametersImpl(webrtc::RtpCodecParameters rtp_codec_parameters);
  virtual const string mime_type() const override;
  virtual const string name() override;
  virtual void set_name(const string name) override;
  virtual RTCMediaType kind() override;
  virtual void set_kind(RTCMediaType value) override;
  virtual int payload_type() override;
  virtual void set_payload_type(int value) override;
  virtual int clock_rate() override;
  virtual void set_clock_rate(int value) override;
  virtual int num_channels() override;
  virtual void set_num_channels(int value) override;
  virtual int max_ptime() override;
  virtual void set_max_ptime(int value) override;
  virtual int ptime() override;
  virtual void set_ptime(int value) override;
  virtual const vector<scoped_refptr<RTCRtcpFeedback>> rtcp_feedback() override;
  virtual void set_rtcp_feedback(
      vector<scoped_refptr<RTCRtcpFeedback>> rtcp_feedbacks) override;
  virtual const vector<std::pair<string, string>> parameters() override;
  virtual void set_parameters(map<string, string> parameters) override;
  virtual bool operator==(scoped_refptr<RTCRtpCodecParameters> o) override;
  virtual bool operator!=(scoped_refptr<RTCRtpCodecParameters> o) override;

  webrtc::RtpCodecParameters rtp_codec_parameters();

 private:
  webrtc::RtpCodecParameters rtp_codec_parameters_;
};

class RTCRtpExtensionImpl : public RTCRtpExtension {
 public:
  RTCRtpExtensionImpl(webrtc::RtpExtension rtp_extension);
  virtual const string ToString() const override;
  virtual bool operator==(scoped_refptr<RTCRtpExtension> o) const override;
  virtual const string uri() override;
  virtual void set_uri(const string uri) override;
  virtual int id() override;
  virtual void set_id(int value) override;
  virtual bool encrypt() override;
  virtual void set_encrypt(bool value) override;

  webrtc::RtpExtension rtp_extension();

 private:
  webrtc::RtpExtension rtp_extension_;
};

class RTCRtcpParametersImpl : public RTCRtcpParameters {
 public:
  RTCRtcpParametersImpl(webrtc::RtcpParameters rtcp_parameters);

  virtual uint32_t ssrc() override;
  virtual void set_ssrc(uint32_t value) override;
  virtual const string cname() override;
  virtual void set_cname(const string cname) override;
  virtual bool reduced_size() override;
  virtual void set_reduced_size(bool value) override;
  virtual bool mux() override;
  virtual void set_mux(bool value) override;
  virtual bool operator==(scoped_refptr<RTCRtcpParameters> o) const override;
  virtual bool operator!=(scoped_refptr<RTCRtcpParameters> o) const override;

  webrtc::RtcpParameters rtcp_parameters();

 private:
  webrtc::RtcpParameters rtcp_parameters_;
};

class RTCRtpParametersImpl : public RTCRtpParameters {
 public:
  RTCRtpParametersImpl(webrtc::RtpParameters rtp_parameters);

  virtual const string transaction_id() override;
  virtual void set_transaction_id(const string id) override;
  virtual const string mid() override;
  virtual void set_mid(const string mid) override;
  virtual const vector<scoped_refptr<RTCRtpCodecParameters>> codecs() override;
  virtual void set_codecs(
      const vector<scoped_refptr<RTCRtpCodecParameters>> codecs) override;
  virtual const vector<scoped_refptr<RTCRtpExtension>> header_extensions()
      override;
  virtual void set_header_extensions(
      const vector<scoped_refptr<RTCRtpExtension>> header_extensions) override;
  virtual const vector<scoped_refptr<RTCRtpEncodingParameters>> encodings()
      override;
  virtual void set_encodings(
      const vector<scoped_refptr<RTCRtpEncodingParameters>> encodings) override;

  virtual scoped_refptr<RTCRtcpParameters> rtcp_parameters() override;
  virtual void set_rtcp_parameters(
      scoped_refptr<RTCRtcpParameters> rtcp_parameters) override;
  virtual bool operator==(scoped_refptr<RTCRtpParameters> o) const override;
  virtual bool operator!=(scoped_refptr<RTCRtpParameters> o) const override;

  virtual RTCDegradationPreference GetDegradationPreference() override;
  virtual void SetDegradationPreference(
      RTCDegradationPreference value) override;

  webrtc::RtpParameters rtp_parameters();

 private:
  webrtc::RtpParameters rtp_parameters_;
};

class RTCRtpEncodingParametersImpl : public RTCRtpEncodingParameters {
 public:
  RTCRtpEncodingParametersImpl();

  RTCRtpEncodingParametersImpl(
      webrtc::RtpEncodingParameters& rtp_encoding_parameters);

  virtual uint32_t ssrc() override;
  virtual void set_ssrc(uint32_t value) override;

  virtual double bitrate_priority() override;
  virtual void set_bitrate_priority(double value) override;

  virtual RTCPriority network_priority() override;
  virtual void set_network_priority(RTCPriority value) override;

  virtual int max_bitrate_bps() override;
  virtual void set_max_bitrate_bps(int value) override;

  virtual int min_bitrate_bps() override;
  virtual void set_min_bitrate_bps(int value) override;

  virtual double max_framerate() override;
  virtual void set_max_framerate(double value) override;

  virtual int num_temporal_layers() override;
  virtual void set_num_temporal_layers(int value) override;

  virtual double scale_resolution_down_by() override;
  virtual void set_scale_resolution_down_by(double value) override;

  virtual const string scalability_mode() override;
  virtual void set_scalability_mode(const string mode) override;

  virtual bool active() override;
  virtual void set_active(bool value) override;

  virtual const string rid() override;
  virtual void set_rid(const string rid) override;

  virtual bool adaptive_ptime() override;
  virtual void set_adaptive_ptime(bool value) override;

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