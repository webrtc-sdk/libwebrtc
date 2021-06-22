#include "rtc_dtmf_sender_impl.h"

namespace libwebrtc {

RTCDtmfSenderImpl::RTCDtmfSenderImpl(
    rtc::scoped_refptr<webrtc::DtmfSenderInterface> dtmf_sender)
    : dtmf_sender_(dtmf_sender), observer_(nullptr) {}

rtc::scoped_refptr<webrtc::DtmfSenderInterface>
RTCDtmfSenderImpl::dtmf_sender() {
  return dtmf_sender_;
}


bool RTCDtmfSenderImpl::InsertDtmf(const char* tones,
                                   size_t size,
                                   int duration,
                                   int inter_tone_gap) {
  return dtmf_sender_->InsertDtmf(std::string(tones,size),duration,inter_tone_gap);
}

bool RTCDtmfSenderImpl::InsertDtmf(const char* tones,
                                   size_t size,
                                   int duration,
                                   int inter_tone_gap,
                                   int comma_delay) {
  return dtmf_sender_->InsertDtmf(std::string(tones, size), duration,
                                  inter_tone_gap,comma_delay);
}

void RTCDtmfSenderImpl::Tones(OnString on) const {
  auto temp = dtmf_sender_->tones();
  on((char*)temp.c_str(), temp.size());
}

void RTCDtmfSenderImpl::OnToneChange(const std::string& tone,
                                     const std::string& tone_buffer) {
  if (observer_) {
    observer_->OnToneChange((char*)tone.c_str(),tone.size(),(char*)tone_buffer.c_str(),tone_buffer.size());
  }
}

void RTCDtmfSenderImpl::OnToneChange(const std::string& tone) {
  if (observer_) {
    observer_->OnToneChange((char*)tone.c_str(), tone.size());
  }
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


int RTCDtmfSenderImpl::Duration() const {
  return dtmf_sender_->duration();
}


int RTCDtmfSenderImpl::InterToneGap() const {
  return dtmf_sender_->inter_tone_gap();
}


int RTCDtmfSenderImpl::CommaDelay() const {
  return dtmf_sender_->comma_delay();
}
}  // namespace libwebrtc