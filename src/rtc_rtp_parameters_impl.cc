#include "rtc_rtp_parameters_impl.h"

#include "base/refcountedobject.h"
namespace libwebrtc {
LIB_WEBRTC_API scoped_refptr<RTCRtpEncodingParameters>
RTCRtpEncodingParameters::Create() {
  return new RefCountedObject<RTCRtpEncodingParametersImpl>();
}

RTCRtpEncodingParametersImpl::RTCRtpEncodingParametersImpl() {}

RTCRtpEncodingParametersImpl::RTCRtpEncodingParametersImpl(
    webrtc::RtpEncodingParameters& rtp_encoding_parameters)
    : rtp_encoding_parameters_(rtp_encoding_parameters) {}

uint32_t RTCRtpEncodingParametersImpl::GetSsrc() {
  return rtp_encoding_parameters_.ssrc.value();
}

void RTCRtpEncodingParametersImpl::SetSsrc(uint32_t value) {
  rtp_encoding_parameters_.ssrc = value;
}

double RTCRtpEncodingParametersImpl::GetBitratePriority() {
  return rtp_encoding_parameters_.bitrate_priority;
}

void RTCRtpEncodingParametersImpl::SetBitratePriority(double value) {
  rtp_encoding_parameters_.bitrate_priority = value;
}

RTCPriority RTCRtpEncodingParametersImpl::GetNetworkPriority() {
  return static_cast<RTCPriority>(rtp_encoding_parameters_.network_priority);
}

void RTCRtpEncodingParametersImpl::SetNetworkPriority(RTCPriority value) {
  rtp_encoding_parameters_.network_priority =
      static_cast<webrtc::Priority>(value);
}

int RTCRtpEncodingParametersImpl::GetMaxBitrateBps() {
  return rtp_encoding_parameters_.max_bitrate_bps.value();
}

void RTCRtpEncodingParametersImpl::SetMaxBitrateBps(int value) {
  rtp_encoding_parameters_.max_bitrate_bps = value;
}

int RTCRtpEncodingParametersImpl::GetMinBitrateBps() {
  return rtp_encoding_parameters_.min_bitrate_bps.value();
}

void RTCRtpEncodingParametersImpl::SetMinBitrateBps(int value) {
  rtp_encoding_parameters_.min_bitrate_bps = value;
}

double RTCRtpEncodingParametersImpl::GetMaxFramerate() {
  return rtp_encoding_parameters_.max_framerate.value();
}

void RTCRtpEncodingParametersImpl::SetMaxFramerate(double value) {
  rtp_encoding_parameters_.max_framerate = value;
}

int RTCRtpEncodingParametersImpl::GetNumTemporalLayers() {
  return rtp_encoding_parameters_.num_temporal_layers.value();
}

void RTCRtpEncodingParametersImpl::SetNumTemporalLayers(int value) {
  rtp_encoding_parameters_.num_temporal_layers = value;
}

double RTCRtpEncodingParametersImpl::GetScaleResolutionDownBy() {
  return rtp_encoding_parameters_.scale_resolution_down_by.value();
}

void RTCRtpEncodingParametersImpl::SetScaleResolutionDownBy(double value) {
  rtp_encoding_parameters_.scale_resolution_down_by = value;
}

void RTCRtpEncodingParametersImpl::GetScalabilityMode(OnString on) {
  auto temp= rtp_encoding_parameters_.scalability_mode.value();
  on((char*)temp.c_str(), temp.size());
}

void RTCRtpEncodingParametersImpl::SetScalabilityMode(char* p, size_t size) {
  rtp_encoding_parameters_.scalability_mode = std::string(p,size);
}

bool RTCRtpEncodingParametersImpl::GetActive() {
  return rtp_encoding_parameters_.active;
}

void RTCRtpEncodingParametersImpl::SetActive(bool value) {
  rtp_encoding_parameters_.active = value;
}

void RTCRtpEncodingParametersImpl::GetRid(OnString on) {
  auto temp = rtp_encoding_parameters_.rid;
  on((char*)temp.c_str(), temp.size());
}

void RTCRtpEncodingParametersImpl::SetRid(char* p, size_t size) {
  rtp_encoding_parameters_.rid = std::string(p,size);
}

bool RTCRtpEncodingParametersImpl::GetAdaptivePtime() {
  return rtp_encoding_parameters_.adaptive_ptime;
}

void RTCRtpEncodingParametersImpl::SetAdaptivePtime(bool value) {
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
RTCRtpParametersImpl::RTCRtpParametersImpl(
    webrtc::RtpParameters rtp_parameters) {}
webrtc::RtpParameters RTCRtpParametersImpl::rtp_parameters() {
  return rtp_parameters_;
}

void RTCRtpParametersImpl::GetTransactionId(OnString on) {
  auto temp= rtp_parameters_.transaction_id;
  on((char*)temp.c_str(), temp.size());
}
void RTCRtpParametersImpl::SetTransactionId(char* p, size_t size) {
  rtp_parameters_.transaction_id = std::string(p, size);
}
void RTCRtpParametersImpl::GetMid(OnString on) {
  auto temp = rtp_parameters_.mid;
  on((char*)temp.c_str(), temp.size());
}
void RTCRtpParametersImpl::SetMid(char* p, size_t size) {
  rtp_parameters_.mid = std::string(p,size);
}

void RTCRtpParametersImpl::GetCodecs(OnRTCRtpCodecParameters on) {
  for (webrtc::RtpCodecParameters item : rtp_parameters_.codecs) {
    on(new RefCountedObject<RTCRtpCodecParametersImpl>(item));
  }
}

void RTCRtpParametersImpl::SetCodecs(OnVectorRTCRtpCodecParameters on) {
  std::vector<webrtc::RtpCodecParameters> list;
  on([&](scoped_refptr<RTCRtpCodecParameters> item) {
    auto impl = static_cast<RTCRtpCodecParametersImpl*>(item.get());
    list.push_back(impl->rtp_codec_parameters());
  });
  rtp_parameters_.codecs = list;
}

void
RTCRtpParametersImpl::GetHeaderExtensions(OnRTCRtpExtension on) {
  for (webrtc::RtpExtension item : rtp_parameters_.header_extensions) {
    on(new RefCountedObject<RTCRtpExtensionImpl>(item));
  }
}

void RTCRtpParametersImpl::SetHeaderExtensions(OnVectorRTCRtpExtension on) {
  std::vector<webrtc::RtpExtension> list;
  on([&](scoped_refptr<RTCRtpExtension> item) {
    auto impl = static_cast<RTCRtpExtensionImpl*>(item.get());
    list.push_back(impl->rtp_extension());
  });

  rtp_parameters_.header_extensions = list;
}

void RTCRtpParametersImpl::GetEncodings(OnRTCRtpEncodingParameters on) {
  for (webrtc::RtpEncodingParameters item : rtp_parameters_.encodings) {
    on(new RefCountedObject<RTCRtpEncodingParametersImpl>(item));
  }
}

void RTCRtpParametersImpl::SetEncodings(OnVectorRTCRtpEncodingParameters on) {
  std::vector<webrtc::RtpEncodingParameters> list;
  on([&](scoped_refptr<RTCRtpEncodingParameters> item) {
    auto impl = static_cast<RTCRtpEncodingParametersImpl*>(item.get());
    list.push_back(impl->rtp_parameters());
  });
  rtp_parameters_.encodings = list;
}

scoped_refptr<RTCRtcpParameters> RTCRtpParametersImpl::GetRtcp() {
  return new RefCountedObject<RTCRtcpParametersImpl>(rtp_parameters_.rtcp);
}
void RTCRtpParametersImpl::SetRtcp(scoped_refptr<RTCRtcpParameters> value) {
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

uint32_t RTCRtcpParametersImpl::GetSsrc() {
  return rtcp_parameters_.ssrc.value();
}
void RTCRtcpParametersImpl::SetSsrc(uint32_t value) {
   rtcp_parameters_.ssrc = value;
}
void RTCRtcpParametersImpl::GetCname(OnString on) {
  auto temp= rtcp_parameters_.cname;
  on((char*)temp.c_str(), temp.size());
}
void RTCRtcpParametersImpl::SetCname(char* p, size_t size) {
  rtcp_parameters_.cname = std::string(p,size);
}
bool RTCRtcpParametersImpl::GetReducedSize() {
  return rtcp_parameters_.reduced_size;
}
void RTCRtcpParametersImpl::SetReducedSize(bool value) {}

bool RTCRtcpParametersImpl::GetMux() {
  return rtcp_parameters_.mux;
}
void RTCRtcpParametersImpl::SetMux(bool value) {}
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
    : rtp_extension_(rtp_extension) {
}

bool RTCRtpExtensionImpl::operator==(scoped_refptr<RTCRtpExtension> o) const {
  return rtp_extension_ == static_cast<RTCRtpExtensionImpl*>(o.get())->rtp_extension();
}

void RTCRtpExtensionImpl::GetUri(OnString on) {
  auto temp= rtp_extension_.uri;
  on((char*)temp.c_str(), temp.size());
}

void RTCRtpExtensionImpl::SetUri(char* uri, size_t size) {
  rtp_extension_.uri = std::string(uri,size);
}

int RTCRtpExtensionImpl::GetId() {
  return rtp_extension_.id;
}

void RTCRtpExtensionImpl::SetId(int value) {
  rtp_extension_.id = value;
}

bool RTCRtpExtensionImpl::GetEncrypt() {
  return rtp_extension_.encrypt;
}

void RTCRtpExtensionImpl::SetEncrypt(bool value) {
  rtp_extension_.encrypt = value;
}

webrtc::RtpExtension RTCRtpExtensionImpl::rtp_extension() {
  return rtp_extension_;
}

void RTCRtpExtensionImpl::ToString(OnString on) const {
  auto temp =  rtp_extension_.ToString();
  on((char*)temp.c_str(), temp.size());
}

RTCRtpCodecParametersImpl::RTCRtpCodecParametersImpl(
    webrtc::RtpCodecParameters rtp_codec_parameters)
    : rtp_codec_parameters_(rtp_codec_parameters) {}


void RTCRtpCodecParametersImpl::GetMimeType(OnString on) const {
  auto val = rtp_codec_parameters_.mime_type();
  on((char*)val.c_str(), val.size());
}

void RTCRtpCodecParametersImpl::GetName(OnString on) {
  auto val = rtp_codec_parameters_.name;
  on((char*)val.c_str(), val.size());
}

void RTCRtpCodecParametersImpl::SetName(char* p,size_t size) {
  std::string value(p, size);
  rtp_codec_parameters_.name = value;
}

RTCMediaType RTCRtpCodecParametersImpl::GetKind() {
  return static_cast<RTCMediaType>(rtp_codec_parameters_.kind);
}

void RTCRtpCodecParametersImpl::SetKind(RTCMediaType value) {
  rtp_codec_parameters_.kind = static_cast<cricket::MediaType>(value);
}

int RTCRtpCodecParametersImpl::GetPayloadType() {
  return rtp_codec_parameters_.payload_type;
}

void RTCRtpCodecParametersImpl::SetPayloadType(int value) {
  rtp_codec_parameters_.payload_type = value;
}

int RTCRtpCodecParametersImpl::GetClockRate() {
  return rtp_codec_parameters_.clock_rate.value();
}

void RTCRtpCodecParametersImpl::SetClockRate(int value) {
  rtp_codec_parameters_.clock_rate = value;
}

int RTCRtpCodecParametersImpl::GetNumChannels() {
  return rtp_codec_parameters_.num_channels.value();
}

void RTCRtpCodecParametersImpl::SetNumChannels(int value) {
  rtp_codec_parameters_.num_channels = value;
}

int RTCRtpCodecParametersImpl::GetMaxPtime() {
  return rtp_codec_parameters_.max_ptime.value();
}

void RTCRtpCodecParametersImpl::GetMaxPtime(int value) {
  rtp_codec_parameters_.max_ptime = value;
}

int RTCRtpCodecParametersImpl::GetPtime() {
 return rtp_codec_parameters_.ptime.value();
}

void RTCRtpCodecParametersImpl::SetPtime(int value) {
  rtp_codec_parameters_.ptime = value;
}
void RTCRtpCodecParametersImpl::GetRtcpFeedback(OnRTCRtcpFeedback on) {
  for (auto item : rtp_codec_parameters_.rtcp_feedback) {
   on(new RefCountedObject<RTCRtcpFeedbackImpl>(item));
  }
}

void RTCRtpCodecParametersImpl::SetRtcpFeedback(OnVectorRTCRtcpFeedback on) {
  std::vector<webrtc::RtcpFeedback> rtcp_feedback;
  on([&](scoped_refptr<RTCRtcpFeedback> item) {
    auto impl = static_cast<RTCRtcpFeedbackImpl*>(item.get());
    rtcp_feedback.push_back(impl->rtcp_feedback());
  });

  rtp_codec_parameters_.rtcp_feedback = rtcp_feedback;
}

void RTCRtpCodecParametersImpl::GetParameters(OnStringAndString on) {
  for (auto item : rtp_codec_parameters_.parameters) {
    on((char*)item.first.c_str(), item.first.size(), (char*)item.second.c_str(),
       item.second.size());
  }
}

void RTCRtpCodecParametersImpl::SetParameters(OnMapStringAndString on) {
  std::map<std::string, std::string> parameters;
  on([&](char* key, size_t key_size, char* val, size_t val_size) {
    parameters[std::string(key, key_size)] = std::string(val, val_size);
  });
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

RTCRtcpFeedbackType RTCRtcpFeedbackImpl::GetType() {
  return static_cast<RTCRtcpFeedbackType>(rtcp_feedback_.type);
}

void RTCRtcpFeedbackImpl::GetType(RTCRtcpFeedbackType value) {
  rtcp_feedback_.type = static_cast<webrtc::RtcpFeedbackType>(value);
}

RTCRtcpFeedbackMessageType RTCRtcpFeedbackImpl::GetMessageType() {
  return static_cast<RTCRtcpFeedbackMessageType>(rtcp_feedback_.message_type.value());
}

void RTCRtcpFeedbackImpl::GetMessageType(RTCRtcpFeedbackMessageType value) {
  rtcp_feedback_.message_type =  static_cast<webrtc::RtcpFeedbackMessageType>(value);
}

bool RTCRtcpFeedbackImpl::operator==(scoped_refptr<RTCRtcpFeedback> o) {
  return rtcp_feedback_ ==
         static_cast<RTCRtcpFeedbackImpl*>(o.get())->rtcp_feedback();
}

bool RTCRtcpFeedbackImpl::operator!=(scoped_refptr<RTCRtcpFeedback> o) {
  return rtcp_feedback_ != static_cast<RTCRtcpFeedbackImpl*>(o.get())->rtcp_feedback();
}

}  // namespace libwebrtc