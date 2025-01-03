#include "rtc_dtls_transport_impl.h"

#include "base/refcountedobject.h"
#include "interop_api.h"

// #include "rtc_ice_transport_impl.h"

namespace libwebrtc {

/**
 * class RTCDtlsTransportInformationImpl
 */
RTCDtlsTransportInformationImpl::RTCDtlsTransportInformationImpl(
    webrtc::DtlsTransportInformation dtls_transport_information)
    : dtls_transport_information_(dtls_transport_information) {}

RTCDtlsTransportInformation& RTCDtlsTransportInformationImpl::operator=(
    scoped_refptr<RTCDtlsTransportInformation> c) {
  RTCDtlsTransportInformationImpl* impl =
      static_cast<RTCDtlsTransportInformationImpl*>(c.get());
  dtls_transport_information_ = impl->dtls_transport_information();
  return *this;
}

RTCDtlsTransportInformation::RTCDtlsTransportState
RTCDtlsTransportInformationImpl::state() const {
  return static_cast<RTCDtlsTransportState>(
      dtls_transport_information_.state());
}

int RTCDtlsTransportInformationImpl::ssl_cipher_suite() const {
  return dtls_transport_information_.ssl_cipher_suite().value();
}

int RTCDtlsTransportInformationImpl::srtp_cipher_suite() const {
  return dtls_transport_information_.srtp_cipher_suite().value();
}

webrtc::DtlsTransportInformation&
RTCDtlsTransportInformationImpl::dtls_transport_information() {
  return dtls_transport_information_;
}

/**
 * class RTCDtlsTransportObserverImpl 
 */

RTCDtlsTransportObserverImpl::RTCDtlsTransportObserverImpl(
  void* callbacks /* rtcDtlsTransportObserverCallbacks* */)
  : callbacks_(nullptr)
{
  if (callbacks) {
    size_t nSize = sizeof(rtcDtlsTransportObserverCallbacks);
    callbacks_ = malloc(nSize);
    memcpy(callbacks_, (const void*)callbacks, nSize);
  }
}

RTCDtlsTransportObserverImpl::~RTCDtlsTransportObserverImpl()
{
  if (callbacks_) {
    free(callbacks_);
  }
  callbacks_ = nullptr;
}

void RTCDtlsTransportObserverImpl::OnStateChange(scoped_refptr<RTCDtlsTransportInformation> info)
{
  if (callbacks_) {
    rtcDtlsTransportObserverCallbacks* pCallbacks = reinterpret_cast<rtcDtlsTransportObserverCallbacks*>(callbacks_);
    pCallbacks->StateChanged(
      pCallbacks->UserData,
      static_cast<rtcDtlsTransportInformationHandle>(info.release())
    );
  }
}

void RTCDtlsTransportObserverImpl::OnError(const int type, const char* message)
{
  if (callbacks_) {
    rtcDtlsTransportObserverCallbacks* pCallbacks = reinterpret_cast<rtcDtlsTransportObserverCallbacks*>(callbacks_);
    pCallbacks->Error(
      pCallbacks->UserData,
      type,
      message
    );
  }
}

/**
 * class RTCDtlsTransportImpl
 */
RTCDtlsTransportImpl::RTCDtlsTransportImpl(
    rtc::scoped_refptr<webrtc::DtlsTransportInterface> dtls_transport)
    : dtls_transport_(dtls_transport), observer_(nullptr) {}

scoped_refptr<RTCDtlsTransportInformation>
RTCDtlsTransportImpl::GetInformation() {
  return new RefCountedObject<RTCDtlsTransportInformationImpl>(
      dtls_transport_->Information());
}

void RTCDtlsTransportImpl::RegisterObserver(
    RTCDtlsTransportObserver* observer) {
  observer_ = observer;
  dtls_transport_->RegisterObserver(this);
}

void RTCDtlsTransportImpl::UnregisterObserver() {
  dtls_transport_->UnregisterObserver();
  observer_ = nullptr;
}

void RTCDtlsTransportImpl::OnStateChange(
    webrtc::DtlsTransportInformation info)
{
  if (observer_) {
    scoped_refptr<RTCDtlsTransportInformation> pInfo = 
      new RefCountedObject<RTCDtlsTransportInformationImpl>(info);
    observer_->OnStateChange(pInfo);
  }
}

void RTCDtlsTransportImpl::OnError(webrtc::RTCError error) {
  if (observer_) {
    observer_->OnError(static_cast<int>(error.type()), error.message());
  }
}

rtc::scoped_refptr<webrtc::DtlsTransportInterface>
RTCDtlsTransportImpl::dtls_transport() {
  return dtls_transport_;
}

}  // namespace libwebrtc