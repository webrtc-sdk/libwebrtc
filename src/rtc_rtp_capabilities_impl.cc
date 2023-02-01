#include "rtc_rtp_capabilities_impl.h"

namespace libwebrtc {

scoped_refptr<RTCRtpCodecCapability> RTCRtpCodecCapability::Create() {
  return new RefCountedObject<RTCRtpCodecCapabilityImpl>();
}

inline std::vector<std::string> split(std::string s, std::string delimiter) {
  std::vector<std::string> tokens;
  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    token = s.substr(0, pos);
    tokens.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  if (int(s.length()) > 0) {
    tokens.push_back(s);
  }
  return tokens;
}

template <class T>
std::string join(T& val, std::string delim) {
  std::string str;
  typename T::iterator it;
  const typename T::iterator itlast = val.end() - 1;
  for (it = val.begin(); it != val.end(); it++) {
    str += *it;
    if (it != itlast) {
      str += delim;
    }
  }
  return str;
}

RTCRtpCapabilitiesImpl::RTCRtpCapabilitiesImpl(
    webrtc::RtpCapabilities rtp_capabilities)
    : rtp_capabilities_(rtp_capabilities) {}

const vector<scoped_refptr<RTCRtpCodecCapability>>
RTCRtpCapabilitiesImpl::codecs() {
  std::vector<scoped_refptr<RTCRtpCodecCapability>> codecs;
  for (auto& codec : rtp_capabilities_.codecs) {
    codecs.push_back(new RefCountedObject<RTCRtpCodecCapabilityImpl>(codec));
  }
  return codecs;
}

void RTCRtpCapabilitiesImpl::set_codecs(
    vector<scoped_refptr<RTCRtpCodecCapability>> codecs) {
  rtp_capabilities_.codecs.clear();
  for (auto& codec : codecs.std_vector()) {
    auto impl = static_cast<RTCRtpCodecCapabilityImpl*>(codec.get());
    rtp_capabilities_.codecs.push_back(impl->rtp_codec_capability());
  }
}

const vector<scoped_refptr<RTCRtpHeaderExtensionCapability>>
RTCRtpCapabilitiesImpl::header_extensions() {
  std::vector<scoped_refptr<RTCRtpHeaderExtensionCapability>> header_extensions;
  for (auto& header_extension : rtp_capabilities_.header_extensions) {
    header_extensions.push_back(
        new RefCountedObject<RTCRtpHeaderExtensionCapabilityImpl>(
            header_extension));
  }
  return header_extensions;
}

void RTCRtpCapabilitiesImpl::set_header_extensions(
    vector<scoped_refptr<RTCRtpHeaderExtensionCapability>> header_extensions) {
  rtp_capabilities_.header_extensions.clear();
  for (auto& header_extension : header_extensions.std_vector()) {
    auto impl = static_cast<RTCRtpHeaderExtensionCapabilityImpl*>(
        header_extension.get());
    rtp_capabilities_.header_extensions.push_back(
        impl->rtp_header_extension_capability());
  }
}

webrtc::RtpCapabilities RTCRtpCapabilitiesImpl::rtp_capabilities() {
  return rtp_capabilities_;
}

RTCRtpCodecCapabilityImpl::RTCRtpCodecCapabilityImpl(
    webrtc::RtpCodecCapability rtp_codec_capability)
    : rtp_codec_capability_(rtp_codec_capability) {}

string RTCRtpCodecCapabilityImpl::mime_type() const {
  return rtp_codec_capability_.mime_type();
}

void RTCRtpCodecCapabilityImpl::set_mime_type(const string& mime_type) {
  std::vector<std::string> mime_type_split = split(mime_type.std_string(), "/");
  rtp_codec_capability_.name = mime_type_split[1];
  cricket::MediaType kind = cricket::MEDIA_TYPE_AUDIO;
  if (mime_type_split[0] == "audio") {
    kind = cricket::MEDIA_TYPE_AUDIO;
  } else if (mime_type_split[0] == "video") {
    kind = cricket::MEDIA_TYPE_VIDEO;
  } else {
    kind = cricket::MEDIA_TYPE_DATA;
  }
  rtp_codec_capability_.kind = kind;
}

int RTCRtpCodecCapabilityImpl::clock_rate() const {
  return rtp_codec_capability_.clock_rate.value_or(-1);
}

void RTCRtpCodecCapabilityImpl::set_clock_rate(int clock_rate) {
  rtp_codec_capability_.clock_rate = clock_rate;
}

int RTCRtpCodecCapabilityImpl::channels() const {
  return rtp_codec_capability_.num_channels.value_or(1);
}

void RTCRtpCodecCapabilityImpl::set_channels(int channels) {
  rtp_codec_capability_.num_channels = channels;
}

string RTCRtpCodecCapabilityImpl::sdp_fmtp_line() const {
  std::vector<std::string> strarr;
  for (auto parameter : rtp_codec_capability_.parameters) {
    if (parameter.first == "") {
      strarr.push_back(parameter.second);
    } else {
      strarr.push_back(parameter.first + "=" + parameter.second);
    }
  }
  return join(strarr, ";");
}

void RTCRtpCodecCapabilityImpl::set_sdp_fmtp_line(const string& sdp_fmtp_line) {
  std::vector<std::string> parameters = split(sdp_fmtp_line.std_string(), ";");
  for (auto parameter : parameters) {
    if (parameter.find("=") != std::string::npos) {
      std::vector<std::string> parameter_split = split(parameter, "=");
      rtp_codec_capability_.parameters[parameter_split[0]] = parameter_split[1];
    } else {
      rtp_codec_capability_.parameters[""] = parameter;
    }
  }

  rtp_codec_capability_.parameters = {};
}

webrtc::RtpCodecCapability RTCRtpCodecCapabilityImpl::rtp_codec_capability() {
  return rtp_codec_capability_;
}

RTCRtpHeaderExtensionCapabilityImpl::RTCRtpHeaderExtensionCapabilityImpl(
    webrtc::RtpHeaderExtensionCapability rtp_header_extension_capability)
    : rtp_header_extension_capability_(rtp_header_extension_capability) {}

const string RTCRtpHeaderExtensionCapabilityImpl::uri() {
  return rtp_header_extension_capability_.uri;
}

void RTCRtpHeaderExtensionCapabilityImpl::set_uri(const string uri) {
  rtp_header_extension_capability_.uri = uri.c_string();
}

webrtc::RtpHeaderExtensionCapability
RTCRtpHeaderExtensionCapabilityImpl::rtp_header_extension_capability() {
  return rtp_header_extension_capability_;
}

int RTCRtpHeaderExtensionCapabilityImpl::preferred_id() {
  return rtp_header_extension_capability_.preferred_id.value_or(-1);
}

void RTCRtpHeaderExtensionCapabilityImpl::set_preferred_id(int value) {
  rtp_header_extension_capability_.preferred_id = value;
}

bool RTCRtpHeaderExtensionCapabilityImpl::preferred_encrypt() {
  return rtp_header_extension_capability_.preferred_encrypt;
}
void RTCRtpHeaderExtensionCapabilityImpl::set_preferred_encrypt(bool value) {
  rtp_header_extension_capability_.preferred_encrypt = value;
}

}  // namespace libwebrtc
