#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"
#include "system_wrappers/include/sleep.h"

#include "interop_api.h"

namespace libwebrtc {

TEST(LibWebRTC_CAPI, ConstructDestruct) {
    EXPECT_EQ(LibWebRTC_Initialize(), rtcBool32::kTrue);

    RTC_LOG(LS_INFO) << "LibWebRTC_Initialize() called";

    auto peer_connection_factory = LibWebRTC_CreateRTCPeerConnectionFactory();

    EXPECT_NE(peer_connection_factory, nullptr);

    RTC_LOG(LS_INFO) << "LibWebRTC_CreateRTCPeerConnectionFactory() called, peer_connection_factory " << peer_connection_factory;

    EXPECT_EQ(RTCPeerConnectionFactory_Initialize(peer_connection_factory), rtcBool32::kTrue);

    EXPECT_EQ(RTCPeerConnectionFactory_Terminate(peer_connection_factory), rtcBool32::kTrue);

    RTC_LOG(LS_INFO) << "RefCountedObject_Release() called";
    RefCountedObject_Release(peer_connection_factory);

    LibWebRTC_Terminate();
    RTC_LOG(LS_INFO) << "LibWebRTC_Terminate() called";
}

}  // namespace libwebrtc
