#include "rtc_rtp_transceiver_impl.h"
#include <src/rtc_rtp_capabilities_impl.h>
#include <src/rtc_rtp_parameters_impl.h>
#include <src/rtc_rtp_receiver_impl.h>
#include <src/rtc_rtp_sender_impl.h>

namespace libwebrtc {

LIB_WEBRTC_API scoped_refptr<RTCRtpTransceiverInit>
RTCRtpTransceiverInit::Create(
    RTCRtpTransceiverDirection direction,
    const vector<string> stream_ids,
    const vector<scoped_refptr<RTCRtpEncodingParameters>> encodings) {
  auto init = new RefCountedObject<RTCRtpTransceiverInitImpl>();
  init->set_direction(direction);
  init->set_stream_ids(stream_ids);
  init->set_send_encodings(encodings);
  return init;
}

RTCRtpTransceiverInitImpl::RTCRtpTransceiverInitImpl() {}

RTCRtpTransceiverDirection RTCRtpTransceiverInitImpl::direction() {
  return static_cast<RTCRtpTransceiverDirection>(
      rtp_transceiver_init_.direction);
}

void RTCRtpTransceiverInitImpl::set_direction(
    RTCRtpTransceiverDirection value) {
  rtp_transceiver_init_.direction =
      static_cast<webrtc::RtpTransceiverDirection>(value);
}

const vector<string> RTCRtpTransceiverInitImpl::stream_ids() {
  std::vector<string> vec;
  for (std::string item : rtp_transceiver_init_.stream_ids) {
    vec.push_back(item.c_str());
  }
  return vec;
}

void RTCRtpTransceiverInitImpl::set_stream_ids(const vector<string> ids) {
  std::vector<std::string> list;
  for (auto id : ids.std_vector()) {
    list.push_back(to_std_string(id));
  }
  rtp_transceiver_init_.stream_ids = list;
}

const vector<scoped_refptr<RTCRtpEncodingParameters>>
RTCRtpTransceiverInitImpl::send_encodings() {
  std::vector<scoped_refptr<RTCRtpEncodingParameters>> vec;
  for (auto item : rtp_transceiver_init_.send_encodings) {
    vec.push_back(new RefCountedObject<RTCRtpEncodingParametersImpl>(item));
  }
  return vec;
}

void RTCRtpTransceiverInitImpl::set_send_encodings(
    const vector<scoped_refptr<RTCRtpEncodingParameters>> send_encodings) {
  std::vector<webrtc::RtpEncodingParameters> list;
  for (auto param : send_encodings.std_vector()) {
    auto impl = static_cast<RTCRtpEncodingParametersImpl*>(param.get());
    list.push_back(impl->rtp_parameters());
  }
  rtp_transceiver_init_.send_encodings = list;
}

RTCRtpTransceiverImpl::RTCRtpTransceiverImpl(
    rtc::scoped_refptr<webrtc::RtpTransceiverInterface> rtp_transceiver)
    : rtp_transceiver_(rtp_transceiver) {}

rtc::scoped_refptr<webrtc::RtpTransceiverInterface>
libwebrtc::RTCRtpTransceiverImpl::rtp_transceiver() {
  return rtp_transceiver_;
}

scoped_refptr<RTCRtpSender> RTCRtpTransceiverImpl::sender() const {
  if (nullptr == rtp_transceiver_->sender().get()) {
    return scoped_refptr<RTCRtpSender>();
  }
  return new RefCountedObject<RTCRtpSenderImpl>(rtp_transceiver_->sender());
}

scoped_refptr<RTCRtpReceiver> RTCRtpTransceiverImpl::receiver() const {
  if (nullptr == rtp_transceiver_->receiver().get()) {
    return scoped_refptr<RTCRtpReceiver>();
  }
  return new RefCountedObject<RTCRtpReceiverImpl>(rtp_transceiver_->receiver());
}

bool RTCRtpTransceiverImpl::Stopped() const {
  return rtp_transceiver_->stopped();
}

bool RTCRtpTransceiverImpl::Stopping() const {
  return rtp_transceiver_->stopping();
}

RTCRtpTransceiverDirection RTCRtpTransceiverImpl::direction() const {
  return static_cast<RTCRtpTransceiverDirection>(rtp_transceiver_->direction());
}

const string RTCRtpTransceiverImpl::SetDirectionWithError(
    RTCRtpTransceiverDirection new_direction) {
  auto error = rtp_transceiver_->SetDirectionWithError(
      static_cast<webrtc::RtpTransceiverDirection>(new_direction));
  if (error.ok()) {
    return "";
  } else {
    std::string val = error.message();
    return val;
  }
}

RTCRtpTransceiverDirection RTCRtpTransceiverImpl::current_direction() const {
  auto temp = rtp_transceiver_->current_direction();
  if (temp.has_value()) {
    return static_cast<RTCRtpTransceiverDirection>(temp.value());
  }
  return RTCRtpTransceiverDirection::kInactive;
}

RTCRtpTransceiverDirection RTCRtpTransceiverImpl::fired_direction() const {
  auto temp = rtp_transceiver_->fired_direction();
  if (temp.has_value()) {
    return static_cast<RTCRtpTransceiverDirection>(temp.value());
  }
  return RTCRtpTransceiverDirection::kInactive;
}

const string RTCRtpTransceiverImpl::StopStandard() {
  std::string val = rtp_transceiver_->StopStandard().message();
  return val;
}

void RTCRtpTransceiverImpl::StopInternal() {
  rtp_transceiver_->StopInternal();
}

const string RTCRtpTransceiverImpl::mid() const {
  auto temp = rtp_transceiver_->mid();
  if (temp.has_value()) {
    return temp.value();
  }
  return "";
}

RTCMediaType RTCRtpTransceiverImpl::media_type() const {
  return static_cast<RTCMediaType>(rtp_transceiver_->media_type());
}

void RTCRtpTransceiverImpl::SetCodecPreferences(
    vector<scoped_refptr<RTCRtpCodecCapability>> codecs) {
  std::vector<webrtc::RtpCodecCapability> list;
  for (auto codec : codecs.std_vector()) {
    auto impl = static_cast<RTCRtpCodecCapabilityImpl*>(codec.get());
    list.push_back(impl->rtp_codec_capability());
  }
  rtp_transceiver_->SetCodecPreferences(list);
}

}  // namespace libwebrtc
