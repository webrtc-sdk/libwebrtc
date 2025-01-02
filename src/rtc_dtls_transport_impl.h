
#ifndef LIB_WEBRTC_DTLS_TRANSPORT_INTERFACE_IMPL_H_
#define LIB_WEBRTC_DTLS_TRANSPORT_INTERFACE_IMPL_H_

#include "api/dtls_transport_interface.h"
#include "api/scoped_refptr.h"
#include "rtc_dtls_transport.h"

namespace libwebrtc {

class RTCDtlsTransportInformationImpl : public RTCDtlsTransportInformation {
 public:
  RTCDtlsTransportInformationImpl(
      webrtc::DtlsTransportInformation dtls_transport_information);

  virtual RTCDtlsTransportInformation& operator=(
      scoped_refptr<RTCDtlsTransportInformation> c) override;

  virtual RTCDtlsTransportState state() const override;

  virtual int ssl_cipher_suite() const override;

  virtual int srtp_cipher_suite() const override;

  webrtc::DtlsTransportInformation& dtls_transport_information();

 private:
  webrtc::DtlsTransportInformation dtls_transport_information_;
};

/**
 * class RTCDtlsTransportObserverImpl
 */
class RTCDtlsTransportObserverImpl : public RTCDtlsTransportObserver
{
 public:
   RTCDtlsTransportObserverImpl(void* callbacks /* rtcDtlsTransportObserverCallbacks* */);
   ~RTCDtlsTransportObserverImpl();

   void OnStateChange(scoped_refptr<RTCDtlsTransportInformation> info) override;
   void OnError(const int type, const char* message) override;

 private:
   void* callbacks_ /* rtcDtlsTransportObserverCallbacks* */;
}; // end class RTCDtlsTransportObserverImpl

class RTCDtlsTransportImpl : public RTCDtlsTransport,
                             public webrtc::DtlsTransportObserverInterface {
 public:
  RTCDtlsTransportImpl(
      rtc::scoped_refptr<webrtc::DtlsTransportInterface> dtls_transport);

  virtual scoped_refptr<RTCDtlsTransportInformation> GetInformation() override;

  virtual void RegisterObserver(RTCDtlsTransportObserver* observer) override;

  virtual void UnregisterObserver() override;

  virtual RTCDtlsTransportObserver* GetObserver() const override { return observer_; }

 public:
  virtual void OnStateChange(webrtc::DtlsTransportInformation info) override;

  virtual void OnError(webrtc::RTCError error) override;

  rtc::scoped_refptr<webrtc::DtlsTransportInterface> dtls_transport();

 private:
  rtc::scoped_refptr<webrtc::DtlsTransportInterface> dtls_transport_;
  RTCDtlsTransportObserver* observer_;
};

}  // namespace libwebrtc

#endif  // API_DTLS_TRANSPORT_INTERFACE_H_
