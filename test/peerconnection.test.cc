#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"
#include "system_wrappers/include/sleep.h"

#include "libwebrtc.h"
#include "libwebrtc_capi.h"
#include "ref_counted_object_capi.h"
#include "rtc_peerconnection_factory_capi.h"

namespace libwebrtc {

/*
TEST(LibWebRTC, ConstructDestruct) {
    EXPECT_EQ(LibWebRTC::Initialize(), true);
    auto peer_connection_factory = LibWebRTC::CreateRTCPeerConnectionFactory();
    EXPECT_NE(peer_connection_factory, nullptr);
    peer_connection_factory = nullptr;
    LibWebRTC::Terminate();
}*/

TEST(LibWebRTC_CAPI, ConstructDestruct) {
    EXPECT_EQ(LibWebRTC_Initialize(), rtcBoolean::kTrue);
    
    RTC_LOG(LS_INFO) << "LibWebRTC_Initialize() called";

    auto peer_connection_factory = LibWebRTC_CreatePeerConnectionFactory();
    
    EXPECT_NE(peer_connection_factory, nullptr);

    RTC_LOG(LS_INFO) << "LibWebRTC_CreatePeerConnectionFactory() called, peer_connection_factory " << peer_connection_factory;

    EXPECT_EQ(RTCPeerConnectionFactory_Initialize(peer_connection_factory), rtcBoolean::kTrue);

    EXPECT_EQ(RTCPeerConnectionFactory_Terminate(peer_connection_factory), rtcBoolean::kTrue);

    RTC_LOG(LS_INFO) << "RTCRefCountedObject_Release() called";
    RTCRefCountedObject_Release(peer_connection_factory);

    LibWebRTC_Terminate();
    RTC_LOG(LS_INFO) << "LibWebRTC_Terminate() called";
}

}  // namespace libwebrtc
