#include <memory>
#include <string>
#include <vector>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// Negative-path tests that don't require a created key provider.

TEST(KeyProviderNegative, CreateWithNullOptionsFails) {
    rtcKeyProviderHandle provider = nullptr;
    EXPECT_EQ(KeyProvider_Create(nullptr, &provider),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(provider, nullptr);
}

TEST(KeyProviderNegative, CreateWithNullOutPointerFails) {
    rtcKeyProviderOptions options{};
    EXPECT_EQ(KeyProvider_Create(&options, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(KeyProviderNegative, SetSharedKeyWithNullHandleReturnsFalse) {
    const unsigned char key[16] = {0};
    EXPECT_EQ(KeyProvider_SetSharedKey(nullptr, 0, key, 16),
              rtcBool32::kFalse);
}

TEST(KeyProviderNegative, RatchetSharedKeyWithNullHandleFails) {
    int len = 0;
    EXPECT_EQ(KeyProvider_RatchetSharedKey(nullptr, 0, nullptr, &len),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(KeyProviderNegative, ExportSharedKeyWithNullHandleFails) {
    int len = 0;
    EXPECT_EQ(KeyProvider_ExportSharedKey(nullptr, 0, nullptr, &len),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(KeyProviderNegative, SetKeyWithNullHandleReturnsFalse) {
    const unsigned char key[16] = {0};
    EXPECT_EQ(KeyProvider_SetKey(nullptr, "p0", 0, key, 16),
              rtcBool32::kFalse);
}

TEST(KeyProviderNegative, RatchetKeyWithNullHandleFails) {
    int len = 0;
    EXPECT_EQ(KeyProvider_RatchetKey(nullptr, "p0", 0, nullptr, &len),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(KeyProviderNegative, ExportKeyWithNullHandleFails) {
    int len = 0;
    EXPECT_EQ(KeyProvider_ExportKey(nullptr, "p0", 0, nullptr, &len),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(KeyProviderNegative, SetSifTrailerWithNullHandleFails) {
    const unsigned char trailer[4] = {0};
    EXPECT_EQ(KeyProvider_SetSifTrailer(nullptr, trailer, 4),
              rtcResultU4::kInvalidNativeHandle);
}

// Fixture that owns a created key provider for the duration of each test.
class KeyProviderTest : public ::testing::Test {
protected:
    void SetUp() override {
        rtcKeyProviderOptions options{};
        options.shared_key = rtcBool32::kTrue;
        ratchet_salt_ = {1, 2, 3, 4, 5, 6, 7, 8};
        options.ratchet_salt = ratchet_salt_.data();
        options.ratchet_salt_len = static_cast<int>(ratchet_salt_.size());
        options.ratchet_window_size = 16;
        options.failure_tolerance = -1;
        options.key_ring_size = rtcKeyRingSize::kDefault;

        ASSERT_EQ(KeyProvider_Create(&options, &provider_),
                  rtcResultU4::kSuccess);
        ASSERT_NE(provider_, nullptr);
    }

    void TearDown() override {
        if (provider_) {
            RefCountedObject_Release(provider_);
            provider_ = nullptr;
        }
    }

    std::vector<unsigned char> ratchet_salt_;
    rtcKeyProviderHandle provider_ = nullptr;
};

TEST_F(KeyProviderTest, SetSharedKeySucceeds) {
    const unsigned char key[16] = {0x11, 0x22, 0x33, 0x44};
    EXPECT_EQ(KeyProvider_SetSharedKey(provider_, 0, key, 16),
              rtcBool32::kTrue);
}

TEST_F(KeyProviderTest, ExportSharedKeyAfterSet) {
    const unsigned char key[16] = {0x11, 0x22, 0x33, 0x44};
    ASSERT_EQ(KeyProvider_SetSharedKey(provider_, 0, key, 16),
              rtcBool32::kTrue);

    // Query the required buffer length first.
    int len = 0;
    EXPECT_EQ(KeyProvider_ExportSharedKey(provider_, 0, nullptr, &len),
              rtcResultU4::kSuccess);
    EXPECT_GE(len, 0);
}

TEST_F(KeyProviderTest, ExportSharedKeyWithNullOutLenFails) {
    EXPECT_EQ(KeyProvider_ExportSharedKey(provider_, 0, nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST_F(KeyProviderTest, RatchetSharedKeyWithNullOutLenFails) {
    EXPECT_EQ(KeyProvider_RatchetSharedKey(provider_, 0, nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST_F(KeyProviderTest, SetKeyWithNullParticipantReturnsFalse) {
    const unsigned char key[16] = {0};
    EXPECT_EQ(KeyProvider_SetKey(provider_, nullptr, 0, key, 16),
              rtcBool32::kFalse);
}

TEST_F(KeyProviderTest, SetKeyForParticipantSucceeds) {
    const unsigned char key[16] = {0x55, 0x66, 0x77, 0x88};
    EXPECT_EQ(KeyProvider_SetKey(provider_, "participant-1", 0, key, 16),
              rtcBool32::kTrue);
}

TEST_F(KeyProviderTest, RatchetKeyWithNullParticipantFails) {
    int len = 0;
    EXPECT_EQ(KeyProvider_RatchetKey(provider_, nullptr, 0, nullptr, &len),
              rtcResultU4::kInvalidParameter);
}

TEST_F(KeyProviderTest, RatchetKeyWithNullOutLenFails) {
    EXPECT_EQ(KeyProvider_RatchetKey(provider_, "participant-1", 0, nullptr,
                                     nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST_F(KeyProviderTest, ExportKeyWithNullParticipantFails) {
    int len = 0;
    EXPECT_EQ(KeyProvider_ExportKey(provider_, nullptr, 0, nullptr, &len),
              rtcResultU4::kInvalidParameter);
}

TEST_F(KeyProviderTest, ExportKeyWithNullOutLenFails) {
    EXPECT_EQ(KeyProvider_ExportKey(provider_, "participant-1", 0, nullptr,
                                    nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST_F(KeyProviderTest, ExportKeyAfterSetKey) {
    const unsigned char key[16] = {0x55, 0x66, 0x77, 0x88};
    ASSERT_EQ(KeyProvider_SetKey(provider_, "participant-1", 0, key, 16),
              rtcBool32::kTrue);

    int len = 0;
    EXPECT_EQ(KeyProvider_ExportKey(provider_, "participant-1", 0, nullptr,
                                    &len),
              rtcResultU4::kSuccess);
    EXPECT_GE(len, 0);
}

TEST_F(KeyProviderTest, SetSifTrailerSucceeds) {
    const unsigned char trailer[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    EXPECT_EQ(KeyProvider_SetSifTrailer(provider_, trailer, 4),
              rtcResultU4::kSuccess);
}

}  // namespace libwebrtc
