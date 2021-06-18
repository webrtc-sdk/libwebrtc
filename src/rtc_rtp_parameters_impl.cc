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

String RTCRtpEncodingParametersImpl::GetScalabilityMode() {
  return rtp_encoding_parameters_.scalability_mode.value();
}

void RTCRtpEncodingParametersImpl::SetScalabilityMode(String value) {
  rtp_encoding_parameters_.scalability_mode = value;
}

bool RTCRtpEncodingParametersImpl::GetActive() {
  return rtp_encoding_parameters_.active;
}

void RTCRtpEncodingParametersImpl::SetActive(bool value) {
  rtp_encoding_parameters_.active = value;
}

String RTCRtpEncodingParametersImpl::GetRid() {
  return rtp_encoding_parameters_.rid;
}

void RTCRtpEncodingParametersImpl::SetRid(String value) {
  rtp_encoding_parameters_.rid = value;
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

String RTCRtpParametersImpl::GetTransactionId() {
  return rtp_parameters_.transaction_id;
}
void RTCRtpParametersImpl::SetTransactionId(String value) {
  rtp_parameters_.transaction_id = value;
}
String RTCRtpParametersImpl::GetMid() {
  return rtp_parameters_.mid;
}
void RTCRtpParametersImpl::SetMid(String value) {
  rtp_parameters_.mid = value;
}

Vector<scoped_refptr<RTCRtpCodecParameters>> RTCRtpParametersImpl::GetCodecs() {
  Vector<scoped_refptr<RTCRtpCodecParameters>> ret;
  for (webrtc::RtpCodecParameters item : rtp_parameters_.codecs) {
    ret.push_back(new RefCountedObject<RTCRtpCodecParametersImpl>(item));
  }
  return ret;
}

void RTCRtpParametersImpl::SetCodecs(
    Vector<scoped_refptr<RTCRtpCodecParameters>> value) {
  std::vector<webrtc::RtpCodecParameters> list;
  for (scoped_refptr<RTCRtpCodecParameters> item : value) {
    list.push_back(
        static_cast<RTCRtpCodecParametersImpl*>(item.get())->rtp_codec_parameters());
  }
  rtp_parameters_.codecs = list;
}

Vector<scoped_refptr<RTCRtpExtension>>
RTCRtpParametersImpl::GetHeaderExtensions() {
  Vector<scoped_refptr<RTCRtpExtension>> ret;
  for (webrtc::RtpExtension item : rtp_parameters_.header_extensions) {
    ret.push_back(new RefCountedObject<RTCRtpExtensionImpl>(item));
  }
  return ret;
}

void RTCRtpParametersImpl::SetHeaderExtensions(
    Vector<scoped_refptr<RTCRtpExtension>> value) {
  std::vector<webrtc::RtpExtension> list;
  for (scoped_refptr<RTCRtpExtension> item : value) {
    list.push_back(
        static_cast<RTCRtpExtensionImpl*>(item.get())->rtp_extension());
  }
  rtp_parameters_.header_extensions = list;
}

Vector<scoped_refptr<RTCRtpEncodingParameters>>
RTCRtpParametersImpl::GetEncodings() {
  Vector<scoped_refptr<RTCRtpEncodingParameters>> ret;
  for (webrtc::RtpEncodingParameters item : rtp_parameters_.encodings) {
    ret.push_back(new RefCountedObject<RTCRtpEncodingParametersImpl>(item));
  }
  return ret;
}

void RTCRtpParametersImpl::SetEncodings(
    Vector<scoped_refptr<RTCRtpEncodingParameters>> value) {
  std::vector<webrtc::RtpEncodingParameters> list;
  for (scoped_refptr<RTCRtpEncodingParameters> item : value) {
    list.push_back(static_cast<RTCRtpEncodingParametersImpl*>(item.get())
                       ->rtp_parameters());
  }
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
  return uint32_t();
}
uint32_t RTCRtcpParametersImpl::SetSsrc() {
  return uint32_t();
}
String RTCRtcpParametersImpl::GetCname() {
  return String();
}
void RTCRtcpParametersImpl::SetCname(String value) {}
bool RTCRtcpParametersImpl::GetReducedSize() {
  return false;
}
void RTCRtcpParametersImpl::SetReducedSize(bool value) {}
bool RTCRtcpParametersImpl::GetMux() {
  return false;
}
void RTCRtcpParametersImpl::SetMux(bool value) {}
bool RTCRtcpParametersImpl::operator==(
    scoped_refptr<RTCRtcpParameters> o) const {
  return false;
}
bool RTCRtcpParametersImpl::operator!=(
    scoped_refptr<RTCRtcpParameters> o) const {
  return false;
}
webrtc::RtcpParameters RTCRtcpParametersImpl::rtcp_parameters() {
  return rtcp_parameters_;
}

RTCRtpExtensionImpl::RTCRtpExtensionImpl(webrtc::RtpExtension rtp_extension)
    : rtp_extension_(rtp_extension) {}

bool RTCRtpExtensionImpl::operator==(scoped_refptr<RTCRtpExtension> o) const {
  return false;
}

String RTCRtpExtensionImpl::GetUri() {
  return String();
}

void RTCRtpExtensionImpl::SetUri(String value) {}

int RTCRtpExtensionImpl::GetId() {
  return 0;
}

void RTCRtpExtensionImpl::SetId(int value) {}

bool RTCRtpExtensionImpl::GetEncrypt() {
  return false;
}

void RTCRtpExtensionImpl::etEncrypt(bool value) {}

webrtc::RtpExtension RTCRtpExtensionImpl::rtp_extension() {
  return rtp_extension_;
}

String RTCRtpExtensionImpl::ToString() const {
  return String();
}

RTCRtpCodecParametersImpl::RTCRtpCodecParametersImpl(
    webrtc::RtpCodecParameters rtp_codec_parameters)
    : rtp_codec_parameters_(rtp_codec_parameters) {}

String RTCRtpCodecParametersImpl::GetMimeType() const {
  return String();
}

String RTCRtpCodecParametersImpl::GetName() {
  return String();
}

void RTCRtpCodecParametersImpl::SetName(String value) {}

RTCMediaType RTCRtpCodecParametersImpl::GetKind() {
  return RTCMediaType();
}

void RTCRtpCodecParametersImpl::SetKind(RTCMediaType value) {}

int RTCRtpCodecParametersImpl::GetPayloadType() {
  return 0;
}

void RTCRtpCodecParametersImpl::SetPayloadType(int value) {}

int RTCRtpCodecParametersImpl::GetClockRate() {
  return 0;
}

void RTCRtpCodecParametersImpl::SetClockRate(int value) {}

int RTCRtpCodecParametersImpl::GetNumChannels() {
  return 0;
}

void RTCRtpCodecParametersImpl::SetNumChannels(int value) {}

int RTCRtpCodecParametersImpl::GetMaxPtime() {
  return 0;
}

void RTCRtpCodecParametersImpl::GetMaxPtime(int value) {}

int RTCRtpCodecParametersImpl::Getptime() {
  return 0;
}

void RTCRtpCodecParametersImpl::Setptime(int value) {}

Vector<scoped_refptr<RTCRtcpFeedback>>
RTCRtpCodecParametersImpl::GetRtcpFeedback() {
  return Vector<scoped_refptr<RTCRtcpFeedback>>();
}

void RTCRtpCodecParametersImpl::SetRtcpFeedback(Vector<scoped_refptr<RTCRtcpFeedback>> value) {
}

Map<String, String> RTCRtpCodecParametersImpl::GetParameters() {
  return Map<String, String>();
}

void RTCRtpCodecParametersImpl::SetParameters(Map<String, String> value) {}

bool RTCRtpCodecParametersImpl::operator==(
    scoped_refptr<RTCRtpCodecParameters> o) {
  return false;
}

bool RTCRtpCodecParametersImpl::operator!=(
    scoped_refptr<RTCRtpCodecParameters> o) {
  return false;
}

webrtc::RtpCodecParameters RTCRtpCodecParametersImpl::rtp_codec_parameters() {
  return rtp_codec_parameters_;
}

}  // namespace libwebrtc