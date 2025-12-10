#ifndef LIB_WEBRTC_RTC_PEERCONNECTION_FACTORY_HXX_CAPI
#define LIB_WEBRTC_RTC_PEERCONNECTION_FACTORY_HXX_CAPI

#include "rtc_types_capi.h"

extern "C" {

typedef rtcRefCountedObjectHandle rtcPeerConnectionFactoryHandle;

LIB_WEBRTC_CAPI rtcBoolean
RTCPeerConnectionFactory_Initialize(rtcPeerConnectionFactoryHandle factory);

LIB_WEBRTC_CAPI rtcBoolean
RTCPeerConnectionFactory_Terminate(rtcPeerConnectionFactoryHandle factory);

LIB_WEBRTC_CAPI rtcResult
RTCPeerConnectionFactory_Create(rtcPeerConnectionFactoryHandle factory,
                                const void* configuration);
}

#endif  // LIB_WEBRTC_RTC_PEERCONNECTION_FACTORY_HXX_CAPI