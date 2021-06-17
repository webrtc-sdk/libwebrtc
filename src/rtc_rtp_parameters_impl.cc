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
  rtp_encoding_parameters_.network_priority = static_cast<webrtc::Priority>(value);
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

std::string RTCRtpEncodingParametersImpl::GetScalabilityMode() {
  return rtp_encoding_parameters_.scalability_mode.value();
}

void RTCRtpEncodingParametersImpl::SetScalabilityMode(std::string value) {
  rtp_encoding_parameters_.scalability_mode = value;
}

bool RTCRtpEncodingParametersImpl::GetActive() {
  return rtp_encoding_parameters_.active;
}

void RTCRtpEncodingParametersImpl::SetActive(bool value) {
  rtp_encoding_parameters_.active = value;
}

std::string RTCRtpEncodingParametersImpl::GetRid() {
  return rtp_encoding_parameters_.rid;
}

void RTCRtpEncodingParametersImpl::SetRid(std::string value) {
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
}  // namespace libwebrtc