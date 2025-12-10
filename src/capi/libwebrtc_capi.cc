#include "include/libwebrtc_capi.h"

#include "include/libwebrtc.h"

using namespace libwebrtc;

rtcBoolean LibWebRTC_Initialize() {
  return LibWebRTC::Initialize() ? rtcBoolean::kTrue : rtcBoolean::kFalse;
}

void LibWebRTC_Terminate() { LibWebRTC::Terminate(); }

rtcPeerConnectionFactoryHandle LibWebRTC_CreatePeerConnectionFactory() {
  scoped_refptr<RTCPeerConnectionFactory> p =
      LibWebRTC::CreateRTCPeerConnectionFactory();
  return static_cast<rtcPeerConnectionFactoryHandle>(p.release());
}
