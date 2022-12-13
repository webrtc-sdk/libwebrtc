#include "rtc_rtp_parameters_impl.h"

#include "base/refcountedobject.h"
namespace libwebrtc {

template <typename T>
T get_optional_value(absl::optional<T> opt, T default_value) {
  if (opt.has_value())
    return opt.value();
 
 return default_value;
}

LIB_WEBRTC_API scoped_refptr<RTCRtpEncodingParameters>
RTCRtpEncodingParameters::Create() {
  return new RefCountedObject<RTCRtpEncodingParametersImpl>();
}

RTCRtpEncodingParametersImpl::RTCRtpEncodingParametersImpl() {}

RTCRtpEncodingParametersImpl::RTCRtpEncodingParametersImpl(
    webrtc::RtpEncodingParameters& rtp_encoding_parameters)
    : rtp_encoding_parameters_(rtp_encoding_parameters) {}

uint32_t RTCRtpEncodingParametersImpl::ssrc() {
  return rtp_encoding_parameters_.ssrc.value();
}

void RTCRtpEncodingParametersImpl::set_ssrc(uint32_t value) {
  rtp_encoding_parameters_.ssrc = value;
}

double RTCRtpEncodingParametersImpl::bitrate_priority() {
  return rtp_encoding_parameters_.bitrate_priority;
}

void RTCRtpEncodingParametersImpl::set_bitrate_priority(double value) {
  rtp_encoding_parameters_.bitrate_priority = value;
}

RTCPriority RTCRtpEncodingParametersImpl::network_priority() {
  return static_cast<RTCPriority>(rtp_encoding_parameters_.network_priority);
}

void RTCRtpEncodingParametersImpl::set_network_priority(RTCPriority value) {
  rtp_encoding_parameters_.network_priority =
      static_cast<webrtc::Priority>(value);
}

int RTCRtpEncodingParametersImpl::max_bitrate_bps() {
  return get_optional_value<int>(rtp_encoding_parameters_.max_bitrate_bps, 0);
}

void RTCRtpEncodingParametersImpl::set_max_bitrate_bps(int value) {
  rtp_encoding_parameters_.max_bitrate_bps = value;
}

int RTCRtpEncodingParametersImpl::min_bitrate_bps() {
  return get_optional_value<int>(rtp_encoding_parameters_.min_bitrate_bps, 0);
}

void RTCRtpEncodingParametersImpl::set_min_bitrate_bps(int value) {
  rtp_encoding_parameters_.min_bitrate_bps = value;
}

double RTCRtpEncodingParametersImpl::max_framerate() {
  return get_optional_value<double>(rtp_encoding_parameters_.max_framerate,
                                    0.0);
}

void RTCRtpEncodingParametersImpl::set_max_framerate(double value) {
  rtp_encoding_parameters_.max_framerate = value;
}

int RTCRtpEncodingParametersImpl::num_temporal_layers() {
  return get_optional_value<int>(rtp_encoding_parameters_.num_temporal_layers,
                                 1);
}

void RTCRtpEncodingParametersImpl::set_num_temporal_layers(int value) {
  rtp_encoding_parameters_.num_temporal_layers = value;
}

double RTCRtpEncodingParametersImpl::scale_resolution_down_by() {
  return get_optional_value<double>(
      rtp_encoding_parameters_.scale_resolution_down_by,
                                 1.0);
}

void RTCRtpEncodingParametersImpl::set_scale_resolution_down_by(double value) {
  rtp_encoding_parameters_.scale_resolution_down_by = value;
}

const string RTCRtpEncodingParametersImpl::scalability_mode() {
  auto temp = rtp_encoding_parameters_.scalability_mode.value();
  return temp;
}

void RTCRtpEncodingParametersImpl::set_scalability_mode(const string mode) {
  rtp_encoding_parameters_.scalability_mode = to_std_string(mode);
}

bool RTCRtpEncodingParametersImpl::active() {
  return rtp_encoding_parameters_.active;
}

void RTCRtpEncodingParametersImpl::set_active(bool value) {
  rtp_encoding_parameters_.active = value;
}

const string RTCRtpEncodingParametersImpl::rid() {
  auto temp = rtp_encoding_parameters_.rid;
  return temp;
}

void RTCRtpEncodingParametersImpl::set_rid(const string rid) {
  rtp_encoding_parameters_.rid = to_std_string(rid);
}

bool RTCRtpEncodingParametersImpl::adaptive_ptime() {
  return rtp_encoding_parameters_.adaptive_ptime;
}

void RTCRtpEncodingParametersImpl::set_adaptive_ptime(bool value) {
  rtp_encoding_parameters_.adaptive_ptime = value;
}

bool RTCRtpEncodingParametersImpl::operator==(
    scoped_refptr<RTCRtpEncodingParameters> o) const {
  return rtp_encoding_parameters_ ==
         static_cast<RTCRtpEncodingParametersImpl*>(o.get())->rtp_parameters();
}

bool RTCRtpEncodingParametersImpl::operator!=(
    scoped_refptr<RTCRtpEncodingParameters> o) const {
  return rtp_encoding_parameters_ !=
         static_cast<RTCRtpEncodingParametersImpl*>(o.get())->rtp_parameters();
}

webrtc::RtpEncodingParameters RTCRtpEncodingParametersImpl::rtp_parameters() {
  return rtp_encoding_parameters_;
}
RTCRtpParametersImpl::RTCRtpParametersImpl(webrtc::RtpParameters rtp_parameters)
    : rtp_parameters_(rtp_parameters) {}
webrtc::RtpParameters RTCRtpParametersImpl::rtp_parameters() {
  return rtp_parameters_;
}

const string RTCRtpParametersImpl::transaction_id() {
  return rtp_parameters_.transaction_id;
}

void RTCRtpParametersImpl::set_transaction_id(const string id) {
  rtp_parameters_.transaction_id = to_std_string(id);
}

const string RTCRtpParametersImpl::mid() {
  return rtp_parameters_.mid;
}
void RTCRtpParametersImpl::set_mid(const string mid) {
  rtp_parameters_.mid = to_std_string(mid);
}

const vector<scoped_refptr<RTCRtpCodecParameters>>
RTCRtpParametersImpl::codecs() {
  std::vector<scoped_refptr<RTCRtpCodecParameters>> vec;
  for (auto item : rtp_parameters_.codecs) {
    vec.push_back(new RefCountedObject<RTCRtpCodecParametersImpl>(item));
  }
  return vec;
}

void RTCRtpParametersImpl::set_codecs(
    const vector<scoped_refptr<RTCRtpCodecParameters>> codecs) {
  std::vector<webrtc::RtpCodecParameters> list;
  for (auto item : codecs.std_vector()) {
    auto impl = static_cast<RTCRtpCodecParametersImpl*>(item.get());
    list.push_back(impl->rtp_codec_parameters());
  }
  rtp_parameters_.codecs = list;
}

const vector<scoped_refptr<RTCRtpExtension>>
RTCRtpParametersImpl::header_extensions() {
  std::vector<scoped_refptr<RTCRtpExtension>> vec;
  for (auto item : rtp_parameters_.header_extensions) {
    vec.push_back(new RefCountedObject<RTCRtpExtensionImpl>(item));
  }
  return vec;
}

void RTCRtpParametersImpl::set_header_extensions(
    vector<scoped_refptr<RTCRtpExtension>> header_extensions) {
  std::vector<webrtc::RtpExtension> list;
  for (auto item : header_extensions.std_vector()) {
    auto impl = static_cast<RTCRtpExtensionImpl*>(item.get());
    list.push_back(impl->rtp_extension());
  }
  rtp_parameters_.header_extensions = list;
}

const vector<scoped_refptr<RTCRtpEncodingParameters>>
RTCRtpParametersImpl::encodings() {
  std::vector<scoped_refptr<RTCRtpEncodingParameters>> vec;
  for (auto item : rtp_parameters_.encodings) {
    vec.push_back(new RefCountedObject<RTCRtpEncodingParametersImpl>(item));
  }
  return vec;
}

void RTCRtpParametersImpl::set_encodings(
    vector<scoped_refptr<RTCRtpEncodingParameters>> encodings) {
  std::vector<webrtc::RtpEncodingParameters> list;
  for (auto item : encodings.std_vector()) {
    auto impl = static_cast<RTCRtpEncodingParametersImpl*>(item.get());
    list.push_back(impl->rtp_parameters());
  }
  rtp_parameters_.encodings = list;
}

scoped_refptr<RTCRtcpParameters> RTCRtpParametersImpl::rtcp_parameters() {
  return new RefCountedObject<RTCRtcpParametersImpl>(rtp_parameters_.rtcp);
}

void RTCRtpParametersImpl::set_rtcp_parameters(
    scoped_refptr<RTCRtcpParameters> value) {
  rtp_parameters_.rtcp =
      static_cast<RTCRtcpParametersImpl*>(value.get())->rtcp_parameters();
}

bool RTCRtpParametersImpl::operator==(scoped_refptr<RTCRtpParameters> o) const {
  return rtp_parameters_ ==
         static_cast<RTCRtpParametersImpl*>(o.get())->rtp_parameters();
}
bool RTCRtpParametersImpl::operator!=(scoped_refptr<RTCRtpParameters> o) const {
  return rtp_parameters_ !=
         static_cast<RTCRtpParametersImpl*>(o.get())->rtp_parameters();
}

RTCRtcpParametersImpl::RTCRtcpParametersImpl(
    webrtc::RtcpParameters rtcp_parameters)
    : rtcp_parameters_(rtcp_parameters) {}

uint32_t RTCRtcpParametersImpl::ssrc() {
  return get_optional_value<uint32_t>(rtcp_parameters_.ssrc,
                                 0);
}

void RTCRtcpParametersImpl::set_ssrc(uint32_t value) {
  rtcp_parameters_.ssrc = value;
}

const string RTCRtcpParametersImpl::cname() {
  return rtcp_parameters_.cname;
}
void RTCRtcpParametersImpl::set_cname(const string cname) {
  rtcp_parameters_.cname = to_std_string(cname);
}

bool RTCRtcpParametersImpl::reduced_size() {
  return rtcp_parameters_.reduced_size;
}

void RTCRtcpParametersImpl::set_reduced_size(bool value) {
  rtcp_parameters_.reduced_size = value;
}

bool RTCRtcpParametersImpl::mux() {
  return rtcp_parameters_.mux;
}
void RTCRtcpParametersImpl::set_mux(bool value) {}
bool RTCRtcpParametersImpl::operator==(
    scoped_refptr<RTCRtcpParameters> o) const {
  return rtcp_parameters_ ==
         static_cast<RTCRtcpParametersImpl*>(o.get())->rtcp_parameters();
}
bool RTCRtcpParametersImpl::operator!=(
    scoped_refptr<RTCRtcpParameters> o) const {
  return rtcp_parameters_ !=
         static_cast<RTCRtcpParametersImpl*>(o.get())->rtcp_parameters();
}
webrtc::RtcpParameters RTCRtcpParametersImpl::rtcp_parameters() {
  return rtcp_parameters_;
}

RTCRtpExtensionImpl::RTCRtpExtensionImpl(webrtc::RtpExtension rtp_extension)
    : rtp_extension_(rtp_extension) {}

bool RTCRtpExtensionImpl::operator==(scoped_refptr<RTCRtpExtension> o) const {
  return rtp_extension_ ==
         static_cast<RTCRtpExtensionImpl*>(o.get())->rtp_extension();
}

const string RTCRtpExtensionImpl::uri() {
  return rtp_extension_.uri;
}

void RTCRtpExtensionImpl::set_uri(const string uri) {
  rtp_extension_.uri = to_std_string(uri);
}

int RTCRtpExtensionImpl::id() {
  return rtp_extension_.id;
}

void RTCRtpExtensionImpl::set_id(int value) {
  rtp_extension_.id = value;
}

bool RTCRtpExtensionImpl::encrypt() {
  return rtp_extension_.encrypt;
}

void RTCRtpExtensionImpl::set_encrypt(bool value) {
  rtp_extension_.encrypt = value;
}

webrtc::RtpExtension RTCRtpExtensionImpl::rtp_extension() {
  return rtp_extension_;
}

const string RTCRtpExtensionImpl::ToString() const {
  return rtp_extension_.ToString();
}

RTCRtpCodecParametersImpl::RTCRtpCodecParametersImpl(
    webrtc::RtpCodecParameters rtp_codec_parameters)
    : rtp_codec_parameters_(rtp_codec_parameters) {}

const string RTCRtpCodecParametersImpl::mime_type() const {
  return rtp_codec_parameters_.mime_type();
}

const string RTCRtpCodecParametersImpl::name() {
  return rtp_codec_parameters_.name;
}

void RTCRtpCodecParametersImpl::set_name(const string name) {
  rtp_codec_parameters_.name = to_std_string(name);
}

RTCMediaType RTCRtpCodecParametersImpl::kind() {
  return static_cast<RTCMediaType>(rtp_codec_parameters_.kind);
}

void RTCRtpCodecParametersImpl::set_kind(RTCMediaType value) {
  rtp_codec_parameters_.kind = static_cast<cricket::MediaType>(value);
}

int RTCRtpCodecParametersImpl::payload_type() {
  return rtp_codec_parameters_.payload_type;
}

void RTCRtpCodecParametersImpl::set_payload_type(int value) {
  rtp_codec_parameters_.payload_type = value;
}

int RTCRtpCodecParametersImpl::clock_rate() {
  return get_optional_value<int>(rtp_codec_parameters_.clock_rate, 0);
}

void RTCRtpCodecParametersImpl::set_clock_rate(int value) {
  rtp_codec_parameters_.clock_rate = value;
}

int RTCRtpCodecParametersImpl::num_channels() {
  return get_optional_value<int>(rtp_codec_parameters_.num_channels, 1);
}

void RTCRtpCodecParametersImpl::set_num_channels(int value) {
  rtp_codec_parameters_.num_channels = value;
}

int RTCRtpCodecParametersImpl::max_ptime() {
  return get_optional_value<int>(rtp_codec_parameters_.max_ptime, 0);
}

void RTCRtpCodecParametersImpl::set_max_ptime(int value) {
  rtp_codec_parameters_.max_ptime = value;
}

int RTCRtpCodecParametersImpl::ptime() {
  return get_optional_value<int>(rtp_codec_parameters_.ptime, 0);
}

void RTCRtpCodecParametersImpl::set_ptime(int value) {
  rtp_codec_parameters_.ptime = value;
}

const vector<scoped_refptr<RTCRtcpFeedback>>
RTCRtpCodecParametersImpl::rtcp_feedback() {
  std::vector<scoped_refptr<RTCRtcpFeedback>> vec;
  for (auto item : rtp_codec_parameters_.rtcp_feedback) {
    vec.push_back(new RefCountedObject<RTCRtcpFeedbackImpl>(item));
  }
  return vec;
}

void RTCRtpCodecParametersImpl::set_rtcp_feedback(
    const vector<scoped_refptr<RTCRtcpFeedback>> rtcp_feedbacks) {
  std::vector<webrtc::RtcpFeedback> rtcp_feedback;
  for (scoped_refptr<RTCRtcpFeedback> item : rtcp_feedbacks.std_vector()) {
    auto impl = static_cast<RTCRtcpFeedbackImpl*>(item.get());
    rtcp_feedback.push_back(impl->rtcp_feedback());
  }
  rtp_codec_parameters_.rtcp_feedback = rtcp_feedback;
}

const vector<std::pair<string, string>>
RTCRtpCodecParametersImpl::parameters() {
  std::vector<std::pair<string, string>> els{};
  for (auto item : rtp_codec_parameters_.parameters) {
    els.push_back(
        std::pair<string, string>{item.first.c_str(), item.second.c_str()});
  }
  return els;
}

void RTCRtpCodecParametersImpl::set_parameters(
    const map<string, string> parameters_in) {
  std::map<std::string, std::string> parameters;
  for (auto item : parameters_in) {
    parameters[to_std_string(item.first)] = to_std_string(item.second);
  }
  rtp_codec_parameters_.parameters = parameters;
}

bool RTCRtpCodecParametersImpl::operator==(
    scoped_refptr<RTCRtpCodecParameters> o) {
  RTCRtpCodecParametersImpl* impl =
      static_cast<RTCRtpCodecParametersImpl*>(o.get());
  return rtp_codec_parameters_ == impl->rtp_codec_parameters();
}

bool RTCRtpCodecParametersImpl::operator!=(
    scoped_refptr<RTCRtpCodecParameters> o) {
  RTCRtpCodecParametersImpl* impl =
      static_cast<RTCRtpCodecParametersImpl*>(o.get());
  return rtp_codec_parameters_ != impl->rtp_codec_parameters();
}

webrtc::RtpCodecParameters RTCRtpCodecParametersImpl::rtp_codec_parameters() {
  return rtp_codec_parameters_;
}

RTCRtcpFeedbackImpl::RTCRtcpFeedbackImpl(webrtc::RtcpFeedback rtcp_feedback)
    : rtcp_feedback_(rtcp_feedback) {}

webrtc::RtcpFeedback RTCRtcpFeedbackImpl::rtcp_feedback() {
  return rtcp_feedback_;
}

RTCRtcpFeedbackType RTCRtcpFeedbackImpl::type() {
  return static_cast<RTCRtcpFeedbackType>(rtcp_feedback_.type);
}

void RTCRtcpFeedbackImpl::set_type(RTCRtcpFeedbackType value) {
  rtcp_feedback_.type = static_cast<webrtc::RtcpFeedbackType>(value);
}

RTCRtcpFeedbackMessageType RTCRtcpFeedbackImpl::message_type() {
  return static_cast<RTCRtcpFeedbackMessageType>(
      rtcp_feedback_.message_type.value());
}

void RTCRtcpFeedbackImpl::set_message_type(RTCRtcpFeedbackMessageType value) {
  rtcp_feedback_.message_type =
      static_cast<webrtc::RtcpFeedbackMessageType>(value);
}

bool RTCRtcpFeedbackImpl::operator==(scoped_refptr<RTCRtcpFeedback> o) {
  return rtcp_feedback_ ==
         static_cast<RTCRtcpFeedbackImpl*>(o.get())->rtcp_feedback();
}

bool RTCRtcpFeedbackImpl::operator!=(scoped_refptr<RTCRtcpFeedback> o) {
  return rtcp_feedback_ !=
         static_cast<RTCRtcpFeedbackImpl*>(o.get())->rtcp_feedback();
}

}  // namespace libwebrtc