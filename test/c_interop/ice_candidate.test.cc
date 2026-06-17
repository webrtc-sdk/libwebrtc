#include <cstring>
#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {
namespace {

// A valid ICE candidate SDP attribute line.
const char kCandidate[] =
    "candidate:1 1 UDP 2130706431 192.168.1.1 50000 typ host";

}  // namespace

// SSL is required to parse candidate strings, so initialize the library.
class RTCIceCandidateTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_EQ(LibWebRTC_Initialize(), rtcBool32::kTrue);
    }
    void TearDown() override { LibWebRTC_Terminate(); }
};

TEST_F(RTCIceCandidateTest, CreateReturnsHandle) {
    rtcSdpParseErrorHandle parse_error = nullptr;
    rtcIceCandidateHandle candidate = nullptr;
    EXPECT_EQ(RTCIceCandidate_Create(kCandidate, "audio", 0, &parse_error,
                                     &candidate),
              rtcResultU4::kSuccess);
    ASSERT_NE(candidate, nullptr);

    char sdp_mid[32] = {0};
    EXPECT_EQ(RTCIceCandidate_GetSdpMid(candidate, sdp_mid, sizeof(sdp_mid)),
              rtcResultU4::kSuccess);
    EXPECT_STREQ(sdp_mid, "audio");

    int mline_index = -1;
    EXPECT_EQ(RTCIceCandidate_GetSdpMlineIndex(candidate, &mline_index),
              rtcResultU4::kSuccess);
    EXPECT_EQ(mline_index, 0);

    char cand[256] = {0};
    EXPECT_EQ(RTCIceCandidate_GetCandidate(candidate, cand, sizeof(cand)),
              rtcResultU4::kSuccess);
    EXPECT_GT(std::strlen(cand), 0u);

    if (parse_error) RefCountedObject_Release(parse_error);
    RefCountedObject_Release(candidate);
}

TEST_F(RTCIceCandidateTest, CreateWithNullParseErrorOut) {
    rtcIceCandidateHandle candidate = nullptr;
    EXPECT_EQ(RTCIceCandidate_Create(kCandidate, "video", 1, nullptr,
                                     &candidate),
              rtcResultU4::kSuccess);
    ASSERT_NE(candidate, nullptr);

    int mline_index = -1;
    EXPECT_EQ(RTCIceCandidate_GetSdpMlineIndex(candidate, &mline_index),
              rtcResultU4::kSuccess);
    EXPECT_EQ(mline_index, 1);

    RefCountedObject_Release(candidate);
}

TEST_F(RTCIceCandidateTest, GetCandidateReportsBufferTooSmall) {
    rtcIceCandidateHandle candidate = nullptr;
    ASSERT_EQ(RTCIceCandidate_Create(kCandidate, "audio", 0, nullptr,
                                     &candidate),
              rtcResultU4::kSuccess);
    ASSERT_NE(candidate, nullptr);

    char tiny[4] = {0};
    EXPECT_EQ(RTCIceCandidate_GetCandidate(candidate, tiny, sizeof(tiny)),
              rtcResultU4::kBufferTooSmall);

    RefCountedObject_Release(candidate);
}

// --- Negative paths ---

TEST_F(RTCIceCandidateTest, CreateNullOutPointerFails) {
    EXPECT_EQ(RTCIceCandidate_Create(kCandidate, "audio", 0, nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST_F(RTCIceCandidateTest, GetCandidateNullHandleFails) {
    char value[32] = {0};
    EXPECT_EQ(RTCIceCandidate_GetCandidate(nullptr, value, sizeof(value)),
              rtcResultU4::kInvalidNativeHandle);
}

TEST_F(RTCIceCandidateTest, GetSdpMidNullHandleFails) {
    char value[32] = {0};
    EXPECT_EQ(RTCIceCandidate_GetSdpMid(nullptr, value, sizeof(value)),
              rtcResultU4::kInvalidNativeHandle);
}

TEST_F(RTCIceCandidateTest, GetSdpMlineIndexNullOutFails) {
    rtcIceCandidateHandle candidate = nullptr;
    ASSERT_EQ(RTCIceCandidate_Create(kCandidate, "audio", 0, nullptr,
                                     &candidate),
              rtcResultU4::kSuccess);
    ASSERT_NE(candidate, nullptr);

    EXPECT_EQ(RTCIceCandidate_GetSdpMlineIndex(candidate, nullptr),
              rtcResultU4::kInvalidPointer);

    RefCountedObject_Release(candidate);
}

TEST_F(RTCIceCandidateTest, GetSdpMlineIndexNullHandleFails) {
    int mline_index = 0;
    EXPECT_EQ(RTCIceCandidate_GetSdpMlineIndex(nullptr, &mline_index),
              rtcResultU4::kInvalidNativeHandle);
}

}  // namespace libwebrtc
