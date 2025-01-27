#include "rtc_dummy_audio_source_impl.h"
#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

/**
 * DummyAudioSourceObserverImpl
 */

DummyAudioSourceObserverImpl::DummyAudioSourceObserverImpl(
    void* callbacks /* rtcDummyAudioSourceObserverCallbacks* */)
    : callbacks_(nullptr) {
  if (callbacks) {
    size_t nSize = sizeof(rtcDummyAudioSourceObserverCallbacks);
    callbacks_ = malloc(nSize);
    memcpy(callbacks_, (const void*)callbacks, nSize);
  }
}

DummyAudioSourceObserverImpl::~DummyAudioSourceObserverImpl() {
  if (callbacks_) {
    free(callbacks_);
  }
  callbacks_ = nullptr;
}

void DummyAudioSourceObserverImpl::OnStart() {
  if (callbacks_) {
    rtcDummyAudioSourceObserverCallbacks* pCallbacks =
        reinterpret_cast<rtcDummyAudioSourceObserverCallbacks*>(callbacks_);
    pCallbacks->Started(pCallbacks->UserData);
  }
}

void DummyAudioSourceObserverImpl::OnPaused() {
  if (callbacks_) {
    rtcDummyAudioSourceObserverCallbacks* pCallbacks =
        reinterpret_cast<rtcDummyAudioSourceObserverCallbacks*>(callbacks_);
    pCallbacks->Paused(pCallbacks->UserData);
  }
}

void DummyAudioSourceObserverImpl::OnStop() {
  if (callbacks_) {
    rtcDummyAudioSourceObserverCallbacks* pCallbacks =
        reinterpret_cast<rtcDummyAudioSourceObserverCallbacks*>(callbacks_);
    pCallbacks->Stopped(pCallbacks->UserData);
  }
}

void DummyAudioSourceObserverImpl::OnError() {
  if (callbacks_) {
    rtcDummyAudioSourceObserverCallbacks* pCallbacks =
        reinterpret_cast<rtcDummyAudioSourceObserverCallbacks*>(callbacks_);
    pCallbacks->Failed(pCallbacks->UserData);
  }
}

void DummyAudioSourceObserverImpl::OnFillBuffer(
    scoped_refptr<RTCAudioData> audio_data) {
  if (callbacks_) {
    rtcDummyAudioSourceObserverCallbacks* pCallbacks =
        reinterpret_cast<rtcDummyAudioSourceObserverCallbacks*>(callbacks_);
    rtcAudioDataHandle hData =
        static_cast<rtcVideoFrameHandle>(audio_data.release());
    pCallbacks->FillBuffer(pCallbacks->UserData, hData);
  }
}

/**
 * class RTCDummyAudioSourceImpl
 */

RTCDummyAudioSourceImpl::RTCDummyAudioSourceImpl(
    rtc::scoped_refptr<webrtc::internal::DummyAudioCapturer> rtc_audio_source
) : rtc_audio_source_(rtc_audio_source)
{
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor " << (void*)this;
}

RTCDummyAudioSourceImpl::~RTCDummyAudioSourceImpl() {
  Stop();
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}

void RTCDummyAudioSourceImpl::RegisterAudioDataObserver(DummyAudioSourceObserver* observer) {
  rtc_audio_source_->RegisterAudioDataObserver(observer);
}

void RTCDummyAudioSourceImpl::DeRegisterAudioDataObserver() {
  rtc_audio_source_->DeRegisterAudioDataObserver();
}

int RTCDummyAudioSourceImpl::bits_per_sample() const {
  return rtc_audio_source_->bits_per_sample();
}

int RTCDummyAudioSourceImpl::sample_rate_hz() const {
  return rtc_audio_source_->sample_rate_hz();
}

size_t RTCDummyAudioSourceImpl::number_of_channels() {
  return rtc_audio_source_->number_of_channels();
}

size_t RTCDummyAudioSourceImpl::number_of_frames() {
  return rtc_audio_source_->number_of_frames();
}

RTCCaptureState RTCDummyAudioSourceImpl::Start() {
  return rtc_audio_source_->Start();
}

void RTCDummyAudioSourceImpl::Stop() {
  rtc_audio_source_->Stop();
}

RTCCaptureState RTCDummyAudioSourceImpl::CaptureState() {
  return rtc_audio_source_->CaptureState();
}

bool RTCDummyAudioSourceImpl::IsRunning() const {
  return rtc_audio_source_->IsRunning();
}

DummyAudioSourceObserver* RTCDummyAudioSourceImpl::GetObserver() {
  return rtc_audio_source_->GetObserver();
}

}  // namespace libwebrtc
