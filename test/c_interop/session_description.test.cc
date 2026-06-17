#include <cstring>
#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {
namespace {

// A minimal but valid session description SDP.
const char kSdp[] =
    "v=0\r\n"
    "o=- 0 0 IN IP4 127.0.0.1\r\n"
    "s=-\r\n"
    "t=0 0\r\n"
    "m=application 9 DTLS/SCTP 5000\r\n"
    "c=IN IP4 0.0.0.0\r\n"
    "a=ice-ufrag:abcd\r\n"
    "a=ice-pwd:0123456789abcdef0123\r\n"
    "a=fingerprint:sha-256 "
    "00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF:00:11:22:33:44:55:66:77:88:"
    "99:AA:BB:CC:DD:EE:FF\r\n"
    "a=setup:actpass\r\n"
    "a=mid:data\r\n"
    "a=sctpmap:5000 webrtc-datachannel 1024\r\n";

}  // namespace

// SSL is required to parse/build SDP, so wrap creation tests in a fixture
// that initializes the library.
class RTCSessionDescriptionTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_EQ(LibWebRTC_Initialize(), rtcBool32::kTrue);
    }
    void TearDown() override { LibWebRTC_Terminate(); }
};

TEST_F(RTCSessionDescriptionTest, CreateOfferReturnsHandle) {
    rtcSessionDescriptionHandle desc = nullptr;
    EXPECT_EQ(RTCSessionDescription_Create("offer", kSdp, nullptr, &desc),
              rtcResultU4::kSuccess);
    ASSERT_NE(desc, nullptr);

    char type[32] = {0};
    EXPECT_EQ(RTCSessionDescription_GetType(desc, type, sizeof(type)),
              rtcResultU4::kSuccess);
    EXPECT_STREQ(type, "offer");

    rtcSdpType sdp_type{};
    EXPECT_EQ(RTCSessionDescription_GetSdpType(desc, &sdp_type),
              rtcResultU4::kSuccess);
    EXPECT_EQ(sdp_type, rtcSdpType::kOffer);

    char sdp[1024] = {0};
    EXPECT_EQ(RTCSessionDescription_GetSdp(desc, sdp, sizeof(sdp)),
              rtcResultU4::kSuccess);
    EXPECT_GT(std::strlen(sdp), 0u);

    char str[1024] = {0};
    rtcBool32 success = rtcBool32::kFalse;
    EXPECT_EQ(RTCSessionDescription_ToString(desc, str, sizeof(str), &success),
              rtcResultU4::kSuccess);

    RefCountedObject_Release(desc);
}

TEST_F(RTCSessionDescriptionTest, GetSdpReportsBufferTooSmall) {
    rtcSessionDescriptionHandle desc = nullptr;
    ASSERT_EQ(RTCSessionDescription_Create("offer", kSdp, nullptr, &desc),
              rtcResultU4::kSuccess);
    ASSERT_NE(desc, nullptr);

    char tiny[4] = {0};
    EXPECT_EQ(RTCSessionDescription_GetSdp(desc, tiny, sizeof(tiny)),
              rtcResultU4::kBufferTooSmall);

    RefCountedObject_Release(desc);
}

// --- Negative paths ---

TEST_F(RTCSessionDescriptionTest, CreateNullTypeFails) {
    rtcSessionDescriptionHandle desc = nullptr;
    EXPECT_EQ(RTCSessionDescription_Create(nullptr, kSdp, nullptr, &desc),
              rtcResultU4::kInvalidPointer);
}

TEST_F(RTCSessionDescriptionTest, CreateNullSdpFails) {
    rtcSessionDescriptionHandle desc = nullptr;
    EXPECT_EQ(RTCSessionDescription_Create("offer", nullptr, nullptr, &desc),
              rtcResultU4::kInvalidPointer);
}

TEST_F(RTCSessionDescriptionTest, CreateNullOutPointerFails) {
    EXPECT_EQ(RTCSessionDescription_Create("offer", kSdp, nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST_F(RTCSessionDescriptionTest, GetSdpNullHandleFails) {
    char value[16] = {0};
    EXPECT_EQ(RTCSessionDescription_GetSdp(nullptr, value, sizeof(value)),
              rtcResultU4::kInvalidNativeHandle);
}

TEST_F(RTCSessionDescriptionTest, GetTypeNullHandleFails) {
    char value[16] = {0};
    EXPECT_EQ(RTCSessionDescription_GetType(nullptr, value, sizeof(value)),
              rtcResultU4::kInvalidNativeHandle);
}

TEST_F(RTCSessionDescriptionTest, GetSdpTypeNullHandleFails) {
    rtcSdpType sdp_type{};
    EXPECT_EQ(RTCSessionDescription_GetSdpType(nullptr, &sdp_type),
              rtcResultU4::kInvalidNativeHandle);
}

TEST_F(RTCSessionDescriptionTest, ToStringNullHandleFails) {
    char value[16] = {0};
    rtcBool32 success = rtcBool32::kTrue;
    EXPECT_EQ(RTCSessionDescription_ToString(nullptr, value, sizeof(value),
                                             &success),
              rtcResultU4::kInvalidNativeHandle);
}

}  // namespace libwebrtc
