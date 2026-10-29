#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {
namespace {

// No-op custom processing callback used to build a valid custom handle.
void LIB_WEBRTC_CALL OnProcess(rtcObjectHandle /*user_data*/,
                               int /*num_bands*/, int /*num_frames*/,
                               int /*buffer_size*/, float* /*buffer*/) {}

}  // namespace

// --- Negative-path tests that don't need a created instance ---

TEST(RTCAudioProcessingNegative, CreateWithNullOutPointerFails) {
    EXPECT_EQ(RTCAudioProcessing_Create(nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCAudioProcessingNegative, SetCapturePostProcessingWithNullHandleFails) {
    EXPECT_EQ(RTCAudioProcessing_SetCapturePostProcessing(nullptr, nullptr),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCAudioProcessingNegative, SetRenderPreProcessingWithNullHandleFails) {
    EXPECT_EQ(RTCAudioProcessing_SetRenderPreProcessing(nullptr, nullptr),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCAudioProcessingNegative, CreateCustomWithNullOutPointerFails) {
    rtcAudioProcessingCustomCallbacks callbacks{};
    callbacks.Process = &OnProcess;
    EXPECT_EQ(RTCAudioProcessing_CreateCustom(&callbacks, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCAudioProcessingNegative, CreateCustomWithNullCallbacksFails) {
    rtcAudioProcessingCustomHandle custom = nullptr;
    EXPECT_EQ(RTCAudioProcessing_CreateCustom(nullptr, &custom),
              rtcResultU4::kInvalidPointer);
    EXPECT_EQ(custom, nullptr);
}

TEST(RTCAudioProcessingNegative, CreateCustomWithoutProcessCallbackFails) {
    rtcAudioProcessingCustomCallbacks callbacks{};  // Process is null
    rtcAudioProcessingCustomHandle custom = nullptr;
    EXPECT_EQ(RTCAudioProcessing_CreateCustom(&callbacks, &custom),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(custom, nullptr);
}

TEST(RTCAudioProcessingNegative, ReleaseCustomWithNullHandleFails) {
    EXPECT_EQ(RTCAudioProcessing_ReleaseCustom(nullptr),
              rtcResultU4::kInvalidPointer);
}

// --- Fixture owning a created audio processing instance ---

class RTCAudioProcessingTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_EQ(RTCAudioProcessing_Create(&processing_),
                  rtcResultU4::kSuccess);
        ASSERT_NE(processing_, nullptr);
    }

    void TearDown() override {
        if (processing_) {
            RefCountedObject_Release(processing_);
            processing_ = nullptr;
        }
    }

    rtcAudioProcessingHandle processing_ = nullptr;
};

TEST_F(RTCAudioProcessingTest, SetCapturePostProcessingWithNullCustomFails) {
    EXPECT_EQ(RTCAudioProcessing_SetCapturePostProcessing(processing_, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST_F(RTCAudioProcessingTest, SetRenderPreProcessingWithNullCustomFails) {
    EXPECT_EQ(RTCAudioProcessing_SetRenderPreProcessing(processing_, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST_F(RTCAudioProcessingTest, CreateCustomAndAttachCapturePostProcessing) {
    rtcAudioProcessingCustomCallbacks callbacks{};
    callbacks.Process = &OnProcess;

    rtcAudioProcessingCustomHandle custom = nullptr;
    ASSERT_EQ(RTCAudioProcessing_CreateCustom(&callbacks, &custom),
              rtcResultU4::kSuccess);
    ASSERT_NE(custom, nullptr);

    EXPECT_EQ(RTCAudioProcessing_SetCapturePostProcessing(processing_, custom),
              rtcResultU4::kSuccess);

    // Ownership of the custom handle passes to the processing instance, so it
    // is not released here.
}

TEST_F(RTCAudioProcessingTest, CreateCustomAndAttachRenderPreProcessing) {
    rtcAudioProcessingCustomCallbacks callbacks{};
    callbacks.Process = &OnProcess;

    rtcAudioProcessingCustomHandle custom = nullptr;
    ASSERT_EQ(RTCAudioProcessing_CreateCustom(&callbacks, &custom),
              rtcResultU4::kSuccess);
    ASSERT_NE(custom, nullptr);

    EXPECT_EQ(RTCAudioProcessing_SetRenderPreProcessing(processing_, custom),
              rtcResultU4::kSuccess);
}

TEST_F(RTCAudioProcessingTest, CreateCustomThenReleaseCustomSucceeds) {
    rtcAudioProcessingCustomCallbacks callbacks{};
    callbacks.Process = &OnProcess;

    rtcAudioProcessingCustomHandle custom = nullptr;
    ASSERT_EQ(RTCAudioProcessing_CreateCustom(&callbacks, &custom),
              rtcResultU4::kSuccess);
    ASSERT_NE(custom, nullptr);

    EXPECT_EQ(RTCAudioProcessing_ReleaseCustom(custom),
              rtcResultU4::kSuccess);
}

}  // namespace libwebrtc
