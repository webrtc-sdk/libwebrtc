#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {
namespace {

void LIB_WEBRTC_CALL OnAudioDeviceChange(rtcObjectHandle user_data) {
    if (user_data) {
        *static_cast<int*>(user_data) += 1;
    }
}

}  // namespace

// Negative-path tests that don't need an audio device.

TEST(RTCAudioDeviceNegative, PlayoutDevicesWithNullHandleReturnsZero) {
    EXPECT_EQ(RTCAudioDevice_PlayoutDevices(nullptr), 0);
}

TEST(RTCAudioDeviceNegative, RecordingDevicesWithNullHandleReturnsZero) {
    EXPECT_EQ(RTCAudioDevice_RecordingDevices(nullptr), 0);
}

TEST(RTCAudioDeviceNegative, PlayoutDeviceNameWithNullHandleFails) {
    char name[128] = {0};
    char guid[128] = {0};
    EXPECT_EQ(
        RTCAudioDevice_PlayoutDeviceName(nullptr, 0, name, sizeof(name), guid,
                                         sizeof(guid)),
        rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCAudioDeviceNegative, SetMicrophoneVolumeWithNullHandleFails) {
    EXPECT_EQ(RTCAudioDevice_SetMicrophoneVolume(nullptr, 100),
              rtcResultU4::kInvalidNativeHandle);
}

// Fixture that owns an initialized factory and an audio device.
class RTCAudioDeviceTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_EQ(LibWebRTC_Initialize(), rtcBool32::kTrue);
        factory_ = LibWebRTC_CreateRTCPeerConnectionFactory();
        ASSERT_NE(factory_, nullptr);
        ASSERT_EQ(RTCPeerConnectionFactory_Initialize(factory_),
                  rtcBool32::kTrue);
        ASSERT_EQ(RTCPeerConnectionFactory_GetAudioDevice(factory_, &device_),
                  rtcResultU4::kSuccess);
        ASSERT_NE(device_, nullptr);
    }

    void TearDown() override {
        if (device_) {
            RefCountedObject_Release(device_);
            device_ = nullptr;
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
    rtcAudioDeviceHandle device_ = nullptr;
};

TEST_F(RTCAudioDeviceTest, PlayoutDevicesReturnsCount) {
    // Returns the device count, or a negative sentinel (-1) when no audio
    // backend is available (e.g. a headless CI host). Both are valid.
    EXPECT_GE(RTCAudioDevice_PlayoutDevices(device_), -1);
}

TEST_F(RTCAudioDeviceTest, RecordingDevicesReturnsCount) {
    EXPECT_GE(RTCAudioDevice_RecordingDevices(device_), -1);
}

TEST_F(RTCAudioDeviceTest, PlayoutDeviceNameWithBuffer) {
    int count = RTCAudioDevice_PlayoutDevices(device_);
    if (count <= 0) {
        GTEST_SKIP() << "No playout devices available";
    }
    char name[256] = {0};
    char guid[256] = {0};
    EXPECT_EQ(
        RTCAudioDevice_PlayoutDeviceName(device_, 0, name, sizeof(name), guid,
                                         sizeof(guid)),
        rtcResultU4::kSuccess);
}

TEST_F(RTCAudioDeviceTest, RecordingDeviceNameWithBuffer) {
    int count = RTCAudioDevice_RecordingDevices(device_);
    if (count <= 0) {
        GTEST_SKIP() << "No recording devices available";
    }
    char name[256] = {0};
    char guid[256] = {0};
    EXPECT_EQ(
        RTCAudioDevice_RecordingDeviceName(device_, 0, name, sizeof(name), guid,
                                           sizeof(guid)),
        rtcResultU4::kSuccess);
}

TEST_F(RTCAudioDeviceTest, PlayoutDeviceNameWithNullNameBufferFails) {
    char guid[128] = {0};
    EXPECT_EQ(RTCAudioDevice_PlayoutDeviceName(device_, 0, nullptr, 0, guid,
                                               sizeof(guid)),
              rtcResultU4::kInvalidPointer);
}

TEST_F(RTCAudioDeviceTest, PlayoutDeviceNameWithZeroBuffersFails) {
    char name[128] = {0};
    char guid[128] = {0};
    EXPECT_EQ(RTCAudioDevice_PlayoutDeviceName(device_, 0, name, 0, guid, 0),
              rtcResultU4::kBufferTooSmall);
}

TEST_F(RTCAudioDeviceTest, SetAndGetMicrophoneVolume) {
    // Volume control depends on real audio hardware; skip when unavailable
    // (e.g. a headless CI host where the operation reports an error).
    if (RTCAudioDevice_SetMicrophoneVolume(device_, 100) !=
        rtcResultU4::kSuccess) {
        GTEST_SKIP() << "Microphone volume control unavailable";
    }
    unsigned int volume = 0;
    EXPECT_EQ(RTCAudioDevice_GetMicrophoneVolume(device_, &volume),
              rtcResultU4::kSuccess);
}

TEST_F(RTCAudioDeviceTest, SetAndGetSpeakerVolume) {
    if (RTCAudioDevice_SetSpeakerVolume(device_, 100) !=
        rtcResultU4::kSuccess) {
        GTEST_SKIP() << "Speaker volume control unavailable";
    }
    unsigned int volume = 0;
    EXPECT_EQ(RTCAudioDevice_GetSpeakerVolume(device_, &volume),
              rtcResultU4::kSuccess);
}

TEST_F(RTCAudioDeviceTest, RegisterAndUnregisterDeviceChangeCallback) {
    int counter = 0;
    EXPECT_EQ(RTCAudioDevice_RegisterDeviceChangeCallback(
                  device_, &counter, &OnAudioDeviceChange),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCAudioDevice_UnregisterDeviceChangeCallback(device_),
              rtcResultU4::kSuccess);
}

TEST_F(RTCAudioDeviceTest, RegisterDeviceChangeCallbackAcceptsNullCallback) {
    // A null callback effectively clears the registration.
    EXPECT_EQ(RTCAudioDevice_RegisterDeviceChangeCallback(device_, nullptr,
                                                          nullptr),
              rtcResultU4::kSuccess);
}

}  // namespace libwebrtc
