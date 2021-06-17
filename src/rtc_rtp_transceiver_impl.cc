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

Vector<String> RTCRtpTransceiverInitImpl::GetStreamIds() {
  Vector<String> ret;
  for (auto item : rtp_transceiver_init_.stream_ids) {
    ret.push_back(item);
  }
  return ret;
}

void RTCRtpTransceiverInitImpl::SetStreamIds(Vector<String> value) {
  std::vector<String> list;
  for (auto item : value) {
    list.push_back(item);
  }
  rtp_transceiver_init_.stream_ids = list;
}

Vector<scoped_refptr<RTCRtpEncodingParameters>>
RTCRtpTransceiverInitImpl::GetSendEncodings() {
  Vector<scoped_refptr<RTCRtpEncodingParameters>> ret;
  for (auto item : rtp_transceiver_init_.send_encodings) {
    ret.push_back(new RefCountedObject<RTCRtpEncodingParametersImpl>(item));
  }
  return ret;
}

void RTCRtpTransceiverInitImpl::SetSendEncodings(
    Vector<scoped_refptr<RTCRtpEncodingParameters>> value) {
  std::vector<webrtc::RtpEncodingParameters> list;
  for (auto item : value) {
    list.push_back(static_cast<RTCRtpEncodingParametersImpl*>(item.get())->rtp_parameters());
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

scoped_refptr<RTCRtpSender> RTCRtpTransceiverImpl::Sender() const {
  return new RefCountedObject<RTCRtpSenderImpl>(rtp_transceiver_->sender());
}

scoped_refptr<RTCRtpReceiver> RTCRtpTransceiverImpl::Receiver() const {
  return new RefCountedObject<RTCRtpReceiverImpl>(rtp_transceiver_->receiver());
}

bool RTCRtpTransceiverImpl::Stopped() const {
  return rtp_transceiver_->stopped();
}

bool RTCRtpTransceiverImpl::Stopping() const {
  return rtp_transceiver_->stopping();
}

RTCRtpTransceiverDirection RTCRtpTransceiverImpl::Direction() const {
  return static_cast<RTCRtpTransceiverDirection>(
      rtp_transceiver_->direction());
}

String RTCRtpTransceiverImpl::SetDirectionWithError(
    RTCRtpTransceiverDirection new_direction) {
  return rtp_transceiver_
      ->SetDirectionWithError(
          static_cast<webrtc::RtpTransceiverDirection>(new_direction))
      .message();
}

RTCRtpTransceiverDirection RTCRtpTransceiverImpl::CurrentDirection() const {
  return static_cast<RTCRtpTransceiverDirection>(
      rtp_transceiver_->current_direction().value());
}

RTCRtpTransceiverDirection RTCRtpTransceiverImpl::FiredDirection() const {
  return static_cast<RTCRtpTransceiverDirection>(
      rtp_transceiver_->fired_direction().value());
}

String RTCRtpTransceiverImpl::StopStandard() {
  return rtp_transceiver_->StopStandard().message();
}

void RTCRtpTransceiverImpl::StopInternal() {
  rtp_transceiver_->StopInternal();
}


String RTCRtpTransceiverImpl::GetMid() const {
  return rtp_transceiver_->mid().value();
}


RTCMediaType RTCRtpTransceiverImpl::GetMediaType() const {
  return static_cast<RTCMediaType>(rtp_transceiver_->media_type());
}
}  // namespace libwebrtc
