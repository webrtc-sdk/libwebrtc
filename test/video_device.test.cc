#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_dummy_video_capturer.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_video_device.h"
#include "libwebrtc.h"

namespace libwebrtc {

// Fixture owning an initialized factory plus a video device and a dummy video
// capturer obtained from it, exercising the C++ API.
class RTCVideoDeviceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(LibWebRTC::Initialize());
    factory_ = LibWebRTC::CreateRTCPeerConnectionFactory();
    ASSERT_TRUE(factory_.get() != nullptr);
    ASSERT_TRUE(factory_->Initialize());
    device_ = factory_->GetVideoDevice();
    ASSERT_TRUE(device_.get() != nullptr);
    capturer_ = factory_->CreateDummyVideoCapturer(30, 640, 480);
    ASSERT_TRUE(capturer_.get() != nullptr);
  }

  void TearDown() override {
    if (capturer_) {
      capturer_->Stop();
      capturer_ = nullptr;
    }
    device_ = nullptr;
    if (factory_) {
      factory_->Terminate();
      factory_ = nullptr;
    }
    LibWebRTC::Terminate();
  }

  scoped_refptr<RTCPeerConnectionFactory> factory_;
  scoped_refptr<RTCVideoDevice> device_;
  scoped_refptr<RTCDummyVideoCapturer> capturer_;
};

// --- RTCVideoDevice ---

TEST_F(RTCVideoDeviceTest, NumberOfDevicesIsNonNegative) {
  // NumberOfDevices() is unsigned, so it is always >= 0; just confirm the
  // call is safe on a headless host.
  uint32_t count = device_->NumberOfDevices();
  (void)count;
  SUCCEED();
}

TEST_F(RTCVideoDeviceTest, GetDeviceName) {
  uint32_t count = device_->NumberOfDevices();
  if (count == 0) {
    GTEST_SKIP() << "No video devices available";
  }
  char name[256] = {0};
  char id[256] = {0};
  EXPECT_EQ(device_->GetDeviceName(0, name, sizeof(name), id, sizeof(id)), 0);
}

// --- RTCDummyVideoCapturer ---

TEST_F(RTCVideoDeviceTest, DummyCapturerInitialState) {
  // A freshly created capturer should expose a queryable state and not yet
  // be running.
  RTCCaptureState state = capturer_->state();
  (void)state;
  EXPECT_FALSE(capturer_->IsRunning());
}

TEST_F(RTCVideoDeviceTest, DummyCapturerStartStop) {
  RTCCaptureState started = capturer_->Start();
  EXPECT_EQ(started, RTCCaptureState::CS_RUNNING);
  EXPECT_TRUE(capturer_->IsRunning());

  capturer_->Stop();
  EXPECT_FALSE(capturer_->IsRunning());
}

TEST_F(RTCVideoDeviceTest, DummyCapturerStartWithFps) {
  RTCCaptureState started = capturer_->Start(30);
  EXPECT_EQ(started, RTCCaptureState::CS_RUNNING);
  capturer_->Stop();
}

TEST_F(RTCVideoDeviceTest, DummyCapturerStartWithResolution) {
  RTCCaptureState started = capturer_->Start(30, 640, 480);
  EXPECT_EQ(started, RTCCaptureState::CS_RUNNING);
  capturer_->Stop();
}

}  // namespace libwebrtc
