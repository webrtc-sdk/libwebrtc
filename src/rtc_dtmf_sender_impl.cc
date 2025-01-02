#include "rtc_dtmf_sender_impl.h"
#include "interop_api.h"

namespace libwebrtc {

/**
 * class RTCDtmfSenderObserverImpl 
 */

RTCDtmfSenderObserverImpl::RTCDtmfSenderObserverImpl(
  void* callbacks /* rtcDtmfSenderObserverCallbacks* */)
  : callbacks_(nullptr)
{
  if (callbacks) {
    size_t nSize = sizeof(rtcDtmfSenderObserverCallbacks);
    callbacks_ = malloc(nSize);
    memcpy(callbacks_, (const void*)callbacks, nSize);
  }
}

RTCDtmfSenderObserverImpl::~RTCDtmfSenderObserverImpl()
{
  if (callbacks_) {
    free(callbacks_);
  }
  callbacks_ = nullptr;
}

void RTCDtmfSenderObserverImpl::OnToneChange(const string tone, const string tone_buffer)
{
  if (callbacks_) {
    rtcDtmfSenderObserverCallbacks* pCallbacks = reinterpret_cast<rtcDtmfSenderObserverCallbacks*>(callbacks_);
    pCallbacks->ToneChanged(
      pCallbacks->UserData,
      tone.c_string(),
      static_cast<int>(((string*)&tone)->size()),
      tone_buffer.c_string(),
      static_cast<int>(((string*)&tone_buffer)->size()));
  }
}

void RTCDtmfSenderObserverImpl::OnToneChange(const string tone)
{
  if (callbacks_) {
    rtcDtmfSenderObserverCallbacks* pCallbacks = reinterpret_cast<rtcDtmfSenderObserverCallbacks*>(callbacks_);
    pCallbacks->ToneChanged(
      pCallbacks->UserData,
      tone.c_string(),
      static_cast<int>(((string*)&tone)->size()),
      nullptr,
      0);
  }
}

/**
 * class RTCDtmfSenderImpl
 */

RTCDtmfSenderImpl::RTCDtmfSenderImpl(
    rtc::scoped_refptr<webrtc::DtmfSenderInterface> dtmf_sender)
    : dtmf_sender_(dtmf_sender), observer_(nullptr) {}

rtc::scoped_refptr<webrtc::DtmfSenderInterface>
RTCDtmfSenderImpl::dtmf_sender() {
  return dtmf_sender_;
}

bool RTCDtmfSenderImpl::InsertDtmf(const string tones, int duration,
                                   int inter_tone_gap) {
  return dtmf_sender_->InsertDtmf(to_std_string(tones), duration,
                                  inter_tone_gap);
}

bool RTCDtmfSenderImpl::InsertDtmf(const string tones, int duration,
                                   int inter_tone_gap, int comma_delay) {
  return dtmf_sender_->InsertDtmf(to_std_string(tones), duration,
                                  inter_tone_gap, comma_delay);
}

const string RTCDtmfSenderImpl::tones() const { return dtmf_sender_->tones(); }

void RTCDtmfSenderImpl::OnToneChange(const std::string& tone,
                                     const std::string& tone_buffer) {
  if (observer_) {
    observer_->OnToneChange(tone.c_str(), tone_buffer.c_str());
  }
}

void RTCDtmfSenderImpl::OnToneChange(const std::string& tone) {
  if (observer_) {
    observer_->OnToneChange(tone.c_str());
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

int RTCDtmfSenderImpl::duration() const { return dtmf_sender_->duration(); }

int RTCDtmfSenderImpl::inter_tone_gap() const {
  return dtmf_sender_->inter_tone_gap();
}

int RTCDtmfSenderImpl::comma_delay() const {
  return dtmf_sender_->comma_delay();
}
}  // namespace libwebrtc