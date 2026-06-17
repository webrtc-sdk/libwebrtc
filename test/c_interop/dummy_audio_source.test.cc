#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// Negative-path tests that don't need a dummy audio source.

TEST(RTCDummyAudioSourceNegative, GetBitsPerSampleWithNullHandleFails) {
    int value = 0;
    EXPECT_EQ(RTCDummyAudioSource_GetBitsPerSample(nullptr, &value),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDummyAudioSourceNegative, GetBitsPerSampleWithNullOutFails) {
    EXPECT_EQ(RTCDummyAudioSource_GetBitsPerSample(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCDummyAudioSourceNegative, RegisterObserverWithNullHandleFails) {
    rtcDummyAudioSourceObserverCallbacks callbacks{};
    EXPECT_EQ(
        RTCDummyAudioSource_RegisterAudioDataObserver(nullptr, &callbacks),
        rtcResultU4::kInvalidNativeHandle);
}

// Fixture that owns a factory initialized with dummy audio enabled and a
// dummy audio source obtained from it.
class RTCDummyAudioSourceTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_EQ(LibWebRTC_Initialize(), rtcBool32::kTrue);
        factory_ = LibWebRTC_CreateRTCPeerConnectionFactory();
        ASSERT_NE(factory_, nullptr);
        // Enable dummy audio so dummy audio sources can be created.
        ASSERT_EQ(
            RTCPeerConnectionFactory_Initialize(factory_, rtcBool32::kTrue),
            rtcBool32::kTrue);
        ASSERT_EQ(RTCPeerConnectionFactory_CreateDummyAudioSource(
                      factory_, "dummy-audio", 48000, 2, &source_),
                  rtcResultU4::kSuccess);
        ASSERT_NE(source_, nullptr);
    }

    void TearDown() override {
        if (source_) {
            RefCountedObject_Release(source_);
            source_ = nullptr;
        }
        if (factory_) {
            EXPECT_EQ(RTCPeerConnectionFactory_Terminate(factory_),
                      rtcBool32::kTrue);
            RefCountedObject_Release(factory_);
            factory_ = nullptr;
        }
        LibWebRTC_Terminate();
    }

    rtcPeerConnectionFactoryHandle factory_ = nullptr;
    // CreateDummyAudioSource yields a handle usable as a dummy audio source.
    rtcAudioSourceHandle source_ = nullptr;
};

TEST_F(RTCDummyAudioSourceTest, GetBitsPerSample) {
    int value = 0;
    EXPECT_EQ(RTCDummyAudioSource_GetBitsPerSample(source_, &value),
              rtcResultU4::kSuccess);
}

TEST_F(RTCDummyAudioSourceTest, GetSampleRateHz) {
    int value = 0;
    EXPECT_EQ(RTCDummyAudioSource_GetSampleRateHz(source_, &value),
              rtcResultU4::kSuccess);
}

TEST_F(RTCDummyAudioSourceTest, GetNumChannels) {
    int value = 0;
    EXPECT_EQ(RTCDummyAudioSource_GetNumChannels(source_, &value),
              rtcResultU4::kSuccess);
}

TEST_F(RTCDummyAudioSourceTest, GetSamplesPerChannel) {
    int value = 0;
    EXPECT_EQ(RTCDummyAudioSource_GetSamplesPerChannel(source_, &value),
              rtcResultU4::kSuccess);
}

TEST_F(RTCDummyAudioSourceTest, StartReturnsCaptureState) {
    rtcCaptureState state{};
    EXPECT_EQ(RTCDummyAudioSource_Start(source_, &state),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCDummyAudioSource_Stop(source_), rtcResultU4::kSuccess);
}

TEST_F(RTCDummyAudioSourceTest, GetCaptureState) {
    rtcCaptureState state{};
    EXPECT_EQ(RTCDummyAudioSource_GetCaptureState(source_, &state),
              rtcResultU4::kSuccess);
}

TEST_F(RTCDummyAudioSourceTest, IsRunning) {
    rtcBool32 running = rtcBool32::kTrue;
    EXPECT_EQ(RTCDummyAudioSource_IsRunning(source_, &running),
              rtcResultU4::kSuccess);
}

TEST_F(RTCDummyAudioSourceTest, RegisterAndDeregisterObserver) {
    rtcDummyAudioSourceObserverCallbacks callbacks{};
    EXPECT_EQ(
        RTCDummyAudioSource_RegisterAudioDataObserver(source_, &callbacks),
        rtcResultU4::kSuccess);
    EXPECT_EQ(RTCDummyAudioSource_DeRegisterAudioDataObserver(source_),
              rtcResultU4::kSuccess);
}

TEST_F(RTCDummyAudioSourceTest, RegisterObserverWithNullCallbacksFails) {
    EXPECT_EQ(
        RTCDummyAudioSource_RegisterAudioDataObserver(source_, nullptr),
        rtcResultU4::kInvalidParameter);
}

}  // namespace libwebrtc
