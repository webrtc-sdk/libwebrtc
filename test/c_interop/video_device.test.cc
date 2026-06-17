#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// Negative-path tests that don't need a video device.

TEST(RTCVideoDeviceNegative, NumberOfDevicesWithNullHandleReturnsZero) {
    EXPECT_EQ(RTCVideoDevice_NumberOfDevices(nullptr), 0);
}

TEST(RTCVideoDeviceNegative, GetDeviceNameWithNullHandleFails) {
    char name[128] = {0};
    char id[128] = {0};
    EXPECT_EQ(
        RTCVideoDevice_GetDeviceName(nullptr, 0, name, sizeof(name), id,
                                     sizeof(id)),
        rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCVideoCapturerNegative, StartCaptureWithNullHandleReturnsFalse) {
    EXPECT_EQ(RTCVideoCapturer_StartCapture(nullptr), rtcBool32::kFalse);
}

TEST(RTCVideoCapturerNegative, StopCaptureWithNullHandleFails) {
    EXPECT_EQ(RTCVideoCapturer_StopCapture(nullptr),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDummyVideoCapturerNegative, IsRunningWithNullHandleFails) {
    rtcBool32 running = rtcBool32::kTrue;
    EXPECT_EQ(RTCDummyVideoCapturer_IsRunning(nullptr, &running),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDummyVideoCapturerNegative, Start2WithNullOutFails) {
    EXPECT_EQ(RTCDummyVideoCapturer_Start2(nullptr, 30, nullptr),
              rtcResultU4::kInvalidPointer);
}

// Fixture owning an initialized factory plus a video device and a dummy
// video capturer obtained from it.
class RTCVideoDeviceTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_EQ(LibWebRTC_Initialize(), rtcBool32::kTrue);
        factory_ = LibWebRTC_CreateRTCPeerConnectionFactory();
        ASSERT_NE(factory_, nullptr);
        ASSERT_EQ(RTCPeerConnectionFactory_Initialize(factory_),
                  rtcBool32::kTrue);
        ASSERT_EQ(RTCPeerConnectionFactory_GetVideoDevice(factory_, &device_),
                  rtcResultU4::kSuccess);
        ASSERT_NE(device_, nullptr);
        ASSERT_EQ(RTCPeerConnectionFactory_CreateDummyVideoCapturer(
                      factory_, 30, 640, 480, &capturer_),
                  rtcResultU4::kSuccess);
        ASSERT_NE(capturer_, nullptr);
    }

    void TearDown() override {
        if (capturer_) {
            RTCDummyVideoCapturer_Stop(capturer_);
            RefCountedObject_Release(capturer_);
            capturer_ = nullptr;
        }
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
    rtcVideoDeviceHandle device_ = nullptr;
    rtcDummyVideoCapturerHandle capturer_ = nullptr;
};

// --- RTCVideoDevice ---

TEST_F(RTCVideoDeviceTest, NumberOfDevicesIsNonNegative) {
    EXPECT_GE(RTCVideoDevice_NumberOfDevices(device_), 0);
}

TEST_F(RTCVideoDeviceTest, GetDeviceNameWithBuffer) {
    int count = RTCVideoDevice_NumberOfDevices(device_);
    if (count <= 0) {
        GTEST_SKIP() << "No video devices available";
    }
    char name[256] = {0};
    char id[256] = {0};
    EXPECT_EQ(
        RTCVideoDevice_GetDeviceName(device_, 0, name, sizeof(name), id,
                                     sizeof(id)),
        rtcResultU4::kSuccess);
}

TEST_F(RTCVideoDeviceTest, CreateVideoCapturerWithNullDeviceFails) {
    rtcVideoCapturerHandle capturer = nullptr;
    EXPECT_EQ(RTCVideoDevice_CreateVideoCapturer(nullptr, "cap", 0, 640, 480,
                                                 30, &capturer),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(capturer, nullptr);
}

// --- RTCDummyVideoCapturer ---

TEST_F(RTCVideoDeviceTest, DummyCapturerGetStateAndIsRunning) {
    rtcCaptureState state{};
    EXPECT_EQ(RTCDummyVideoCapturer_GetState(capturer_, &state),
              rtcResultU4::kSuccess);

    rtcBool32 running = rtcBool32::kTrue;
    EXPECT_EQ(RTCDummyVideoCapturer_IsRunning(capturer_, &running),
              rtcResultU4::kSuccess);
}

TEST_F(RTCVideoDeviceTest, DummyCapturerStart1) {
    rtcCaptureState state{};
    EXPECT_EQ(RTCDummyVideoCapturer_Start1(capturer_, &state),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCDummyVideoCapturer_Stop(capturer_), rtcResultU4::kSuccess);
}

TEST_F(RTCVideoDeviceTest, DummyCapturerStart2) {
    rtcCaptureState state{};
    EXPECT_EQ(RTCDummyVideoCapturer_Start2(capturer_, 30, &state),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCDummyVideoCapturer_Stop(capturer_), rtcResultU4::kSuccess);
}

TEST_F(RTCVideoDeviceTest, DummyCapturerStart2RejectsZeroFps) {
    rtcCaptureState state{};
    EXPECT_EQ(RTCDummyVideoCapturer_Start2(capturer_, 0, &state),
              rtcResultU4::kInvalidParameter);
}

TEST_F(RTCVideoDeviceTest, DummyCapturerStart3) {
    rtcCaptureState state{};
    EXPECT_EQ(RTCDummyVideoCapturer_Start3(capturer_, 30, 640, 480, &state),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCDummyVideoCapturer_Stop(capturer_), rtcResultU4::kSuccess);
}

TEST_F(RTCVideoDeviceTest, DummyCapturerStart3RejectsZeroParams) {
    rtcCaptureState state{};
    EXPECT_EQ(RTCDummyVideoCapturer_Start3(capturer_, 0, 640, 480, &state),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(RTCDummyVideoCapturer_Start3(capturer_, 30, 0, 480, &state),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(RTCDummyVideoCapturer_Start3(capturer_, 30, 640, 0, &state),
              rtcResultU4::kInvalidParameter);
}

TEST_F(RTCVideoDeviceTest, DummyCapturerRegisterAndUnregisterObserver) {
    rtcDummyVideoCapturerObserverCallbacks callbacks{};
    EXPECT_EQ(RTCDummyVideoCapturer_RegisterObserver(capturer_, &callbacks),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCDummyVideoCapturer_UnregisterObserver(capturer_),
              rtcResultU4::kSuccess);
}

TEST_F(RTCVideoDeviceTest, DummyCapturerRegisterObserverWithNullCallbacksFails) {
    EXPECT_EQ(RTCDummyVideoCapturer_RegisterObserver(capturer_, nullptr),
              rtcResultU4::kInvalidParameter);
}

}  // namespace libwebrtc
