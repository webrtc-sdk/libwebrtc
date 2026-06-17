#include <cstring>
#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

TEST(RTCSdpParseError, CreateReturnsHandle) {
    rtcSdpParseErrorHandle error = nullptr;
    EXPECT_EQ(RTCSdpParseError_Create(&error), rtcResultU4::kSuccess);
    EXPECT_NE(error, nullptr);
    if (error) RefCountedObject_Release(error);
}

TEST(RTCSdpParseError, CreateNullOutPointerFails) {
    EXPECT_EQ(RTCSdpParseError_Create(nullptr), rtcResultU4::kInvalidPointer);
}

TEST(RTCSdpParseError, SetAndGetLine) {
    rtcSdpParseErrorHandle error = nullptr;
    ASSERT_EQ(RTCSdpParseError_Create(&error), rtcResultU4::kSuccess);
    ASSERT_NE(error, nullptr);

    EXPECT_EQ(RTCSdpParseError_SetLine(error, "m=audio 9 UDP/TLS/RTP/SAVPF 111"),
              rtcResultU4::kSuccess);

    char line[64] = {0};
    EXPECT_EQ(RTCSdpParseError_GetLine(error, line, sizeof(line)),
              rtcResultU4::kSuccess);
    EXPECT_STREQ(line, "m=audio 9 UDP/TLS/RTP/SAVPF 111");

    RefCountedObject_Release(error);
}

TEST(RTCSdpParseError, SetAndGetDescription) {
    rtcSdpParseErrorHandle error = nullptr;
    ASSERT_EQ(RTCSdpParseError_Create(&error), rtcResultU4::kSuccess);
    ASSERT_NE(error, nullptr);

    EXPECT_EQ(RTCSdpParseError_SetDescription(error, "Invalid media line"),
              rtcResultU4::kSuccess);

    char desc[64] = {0};
    EXPECT_EQ(RTCSdpParseError_GetDescription(error, desc, sizeof(desc)),
              rtcResultU4::kSuccess);
    EXPECT_STREQ(desc, "Invalid media line");

    RefCountedObject_Release(error);
}

TEST(RTCSdpParseError, GetLineReportsBufferTooSmall) {
    rtcSdpParseErrorHandle error = nullptr;
    ASSERT_EQ(RTCSdpParseError_Create(&error), rtcResultU4::kSuccess);
    ASSERT_NE(error, nullptr);

    ASSERT_EQ(RTCSdpParseError_SetLine(error, "a-very-long-error-line-value"),
              rtcResultU4::kSuccess);

    char tiny[4] = {0};
    EXPECT_EQ(RTCSdpParseError_GetLine(error, tiny, sizeof(tiny)),
              rtcResultU4::kBufferTooSmall);

    RefCountedObject_Release(error);
}

// --- Negative paths ---

TEST(RTCSdpParseError, GetLineNullHandleFails) {
    char value[16] = {0};
    EXPECT_EQ(RTCSdpParseError_GetLine(nullptr, value, sizeof(value)),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCSdpParseError, GetDescriptionNullHandleFails) {
    char value[16] = {0};
    EXPECT_EQ(RTCSdpParseError_GetDescription(nullptr, value, sizeof(value)),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCSdpParseError, SetLineNullHandleFails) {
    EXPECT_EQ(RTCSdpParseError_SetLine(nullptr, "value"),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCSdpParseError, SetDescriptionNullHandleFails) {
    EXPECT_EQ(RTCSdpParseError_SetDescription(nullptr, "value"),
              rtcResultU4::kInvalidNativeHandle);
}

}  // namespace libwebrtc
