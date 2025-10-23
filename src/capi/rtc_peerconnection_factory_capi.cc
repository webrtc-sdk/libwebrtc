#include "include/rtc_peerconnection_factory_capi.h"

#include "include/rtc_peerconnection_factory.h"

using namespace libwebrtc;

rtcBoolean RTCPeerConnectionFactory_Initialize(
    rtcPeerConnectionFactoryHandle factory) {
  CHECK_POINTER_EX(factory, rtcBoolean::kFalse);

  scoped_refptr<RTCPeerConnectionFactory> pFactory =
      static_cast<RTCPeerConnectionFactory*>(factory);
  return pFactory->Initialize() ? rtcBoolean::kTrue : rtcBoolean::kFalse;
}

rtcBoolean RTCPeerConnectionFactory_Terminate(
    rtcPeerConnectionFactoryHandle factory) {
  CHECK_POINTER_EX(factory, rtcBoolean::kFalse);

  scoped_refptr<RTCPeerConnectionFactory> pFactory =
      static_cast<RTCPeerConnectionFactory*>(factory);
  return pFactory->Terminate() ? rtcBoolean::kTrue : rtcBoolean::kFalse;
}

LIB_WEBRTC_CAPI rtcResult RTCPeerConnectionFactory_Create(
    rtcPeerConnectionFactoryHandle factory, const void* configuration);