#include "interop_api.h"
#include "rtc_peerconnection_factory.h"
#include "libwebrtc.h"

using namespace libwebrtc;

rtcBool32 LIB_WEBRTC_CALL LibWebRTC_Initialize() noexcept
{
    return LibWebRTC::Initialize()
        ? rtcBool32::kTrue
        : rtcBool32::kFalse;
}

rtcPeerConnectionFactoryHandle LIB_WEBRTC_CALL LibWebRTC_CreateRTCPeerConnectionFactory() noexcept
{
    scoped_refptr<RTCPeerConnectionFactory> p = LibWebRTC::CreateRTCPeerConnectionFactory();
    p->AddRef();
    return static_cast<rtcPeerConnectionFactoryHandle>(p.get());
}

void LIB_WEBRTC_CALL LibWebRTC_Terminate() noexcept
{
    LibWebRTC::Terminate();
}
