#include "rtc_rtp_transceiver_impl.h"
#include <src/rtc_rtp_parameters_impl.h>
#include <src/rtc_rtp_sender_impl.h>
#include <src/rtc_rtp_receive_imp.h>

namespace libwebrtc {
webrtc::RtpTransceiverInit
libwebrtc::RTCRtpTransceiverInitImpl::rtp_transceiver_init() {
  return rtp_transceiver_init_;
}

LIB_WEBRTC_API scoped_refptr<RTCRtpTransceiverInit>
RTCRtpTransceiverInit::Create() {
  return new RefCountedObject<RTCRtpTransceiverInitImpl>();
}

RTCRtpTransceiverInitImpl::RTCRtpTransceiverInitImpl() {}

RTCRtpTransceiverDirection RTCRtpTransceiverInitImpl::GetDirection() {
  return static_cast<RTCRtpTransceiverDirection>(
      rtp_transceiver_init_.direction);
}

void RTCRtpTransceiverInitImpl::SetDirection(RTCRtpTransceiverDirection value) {
  rtp_transceiver_init_.direction =
      static_cast<webrtc::RtpTransceiverDirection>(value);
}

void RTCRtpTransceiverInitImpl::GetStreamIds(OnString on) {
  for (std::string item : rtp_transceiver_init_.stream_ids) {
    on((char*)item.c_str(), item.size());
  }
}

void RTCRtpTransceiverInitImpl::SetStreamIds(OnVectorString on) {
  std::vector<std::string> list;
  on([&](char* p, size_t size) {
    std::string id(p, size);
    list.push_back(id);
  });
  rtp_transceiver_init_.stream_ids = list;
}

void
RTCRtpTransceiverInitImpl::GetSendEncodings(OnRTCRtpEncodingParameters on) {
  for (auto item : rtp_transceiver_init_.send_encodings) {
    on(new RefCountedObject<RTCRtpEncodingParametersImpl>(item));
  }
}

void RTCRtpTransceiverInitImpl::SetSendEncodings(
    OnVectorRTCRtpEncodingParameters on) {

  std::vector<webrtc::RtpEncodingParameters> list;

  on([&](scoped_refptr<RTCRtpEncodingParameters> param) {
    auto impl = static_cast<RTCRtpEncodingParametersImpl*>(param.get());
    list.push_back(impl->rtp_parameters());
  });

  rtp_transceiver_init_.send_encodings = list;
}

RTCRtpTransceiverImpl::RTCRtpTransceiverImpl(
    rtc::scoped_refptr<webrtc::RtpTransceiverInterface> rtp_transceiver)
    : rtp_transceiver_(rtp_transceiver) {}

rtc::scoped_refptr<webrtc::RtpTransceiverInterface>
libwebrtc::RTCRtpTransceiverImpl::rtp_transceiver() {
  return rtp_transceiver_;
}

scoped_refptr<RTCRtpSender> RTCRtpTransceiverImpl::Sender() const {
  if (nullptr == rtp_transceiver_->sender().get()) {
    return scoped_refptr<RTCRtpSender>();
  }
  return new RefCountedObject<RTCRtpSenderImpl>(rtp_transceiver_->sender());
}

scoped_refptr<RTCRtpReceiver> RTCRtpTransceiverImpl::Receiver() const {
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

RTCRtpTransceiverDirection RTCRtpTransceiverImpl::Direction() const {
  return static_cast<RTCRtpTransceiverDirection>(rtp_transceiver_->direction());
}

void RTCRtpTransceiverImpl::SetDirectionWithError(
    RTCRtpTransceiverDirection new_direction,
    OnString on) {
  auto error = rtp_transceiver_->SetDirectionWithError(
      static_cast<webrtc::RtpTransceiverDirection>(new_direction));
  if (error.ok()) {
    on(nullptr, 0);
  } else {
    std::string val = error.message();
    on((char*)val.c_str(), val.size());
  }
}

RTCRtpTransceiverDirection RTCRtpTransceiverImpl::CurrentDirection() const {
  auto temp = rtp_transceiver_->current_direction();
  if (temp.has_value()) {
    return static_cast<RTCRtpTransceiverDirection>(temp.value());
  }
  return RTCRtpTransceiverDirection::kInactive;
}

RTCRtpTransceiverDirection RTCRtpTransceiverImpl::FiredDirection() const {
  auto temp = rtp_transceiver_->fired_direction();
  if (temp.has_value()) {
    return static_cast<RTCRtpTransceiverDirection>(temp.value());
  }
  return RTCRtpTransceiverDirection::kInactive;
}

void RTCRtpTransceiverImpl::StopStandard(OnString on) {
  std::string val = rtp_transceiver_->StopStandard().message();
  on((char*)val.c_str(), val.size());
}

void RTCRtpTransceiverImpl::StopInternal() {
  rtp_transceiver_->StopInternal();
}

void RTCRtpTransceiverImpl::GetMid(OnString on) const {
  auto temp = rtp_transceiver_->mid();
  if (temp.has_value()) {
    std::string val = temp.value();
    on((char*)val.c_str(), val.size());
  }
}

RTCMediaType RTCRtpTransceiverImpl::GetMediaType() const {
  return static_cast<RTCMediaType>(rtp_transceiver_->media_type());
}
}  // namespace libwebrtc
