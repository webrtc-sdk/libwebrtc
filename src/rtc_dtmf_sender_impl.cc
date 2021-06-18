#include "rtc_dtmf_sender_impl.h"

namespace libwebrtc {

RTCDtmfSenderImpl::RTCDtmfSenderImpl(
    rtc::scoped_refptr<webrtc::DtmfSenderInterface> dtmf_sender)
    : dtmf_sender_(dtmf_sender), observer_(nullptr) {}

rtc::scoped_refptr<webrtc::DtmfSenderInterface>
RTCDtmfSenderImpl::dtmf_sender() {
  return dtmf_sender_;
}

void RTCDtmfSenderImpl::RegisterObserver(RTCDtmfSenderObserver* observer) {
  observer_ = observer;
  dtmf_sender_->RegisterObserver(this);
}

void RTCDtmfSenderImpl::UnregisterObserver() {
  dtmf_sender_->UnregisterObserver();
  observer_ = nullptr;
}

bool RTCDtmfSenderImpl::CanInsertDtmf() {
  return dtmf_sender_->CanInsertDtmf();
}

bool RTCDtmfSenderImpl::InsertDtmf(const String& tones,
                                int duration,
                                int inter_tone_gap) {
  return dtmf_sender_->InsertDtmf(tones, duration, inter_tone_gap);
}

bool RTCDtmfSenderImpl::InsertDtmf(const String& tones,
                                int duration,
                                int inter_tone_gap,
                                int comma_delay) {
  return dtmf_sender_->InsertDtmf(tones, duration, inter_tone_gap, comma_delay);
}

String RTCDtmfSenderImpl::Tones() const {
  return dtmf_sender_->tones();
}


int RTCDtmfSenderImpl::Duration() const {
  return dtmf_sender_->duration();
}


int RTCDtmfSenderImpl::InterToneGap() const {
  return dtmf_sender_->inter_tone_gap();
}


void RTCDtmfSenderImpl::OnToneChange(const String& tone,
                                  const String& tone_buffer) {
  if (observer_) {
    observer_->OnToneChange(tone, tone_buffer);
  }
}

void RTCDtmfSenderImpl::OnToneChange(const String& tone) {
  if (observer_) {
    observer_->OnToneChange(tone);
  }
}

int RTCDtmfSenderImpl::CommaDelay() const {
  return dtmf_sender_->comma_delay();
}
}  // namespace libwebrtc