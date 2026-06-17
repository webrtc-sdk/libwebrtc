#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_audio_device.h"
#include "rtc_peerconnection_factory.h"
#include "libwebrtc.h"

namespace libwebrtc {

// Fixture that owns an initialized factory and the audio device obtained from
// it, exercising the C++ RTCAudioDevice API.
class RTCAudioDeviceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(LibWebRTC::Initialize());
    factory_ = LibWebRTC::CreateRTCPeerConnectionFactory();
    ASSERT_TRUE(factory_.get() != nullptr);
    ASSERT_TRUE(factory_->Initialize());
    device_ = factory_->GetAudioDevice();
    ASSERT_TRUE(device_.get() != nullptr);
  }

  void TearDown() override {
    device_ = nullptr;
    if (factory_) {
      factory_->Terminate();
      factory_ = nullptr;
    }
    LibWebRTC::Terminate();
  }

  scoped_refptr<RTCPeerConnectionFactory> factory_;
  scoped_refptr<RTCAudioDevice> device_;
};

TEST_F(RTCAudioDeviceTest, PlayoutDevicesReturnsCount) {
  // Device count, or a negative sentinel on a headless host. Both valid.
  EXPECT_GE(device_->PlayoutDevices(), -1);
}

TEST_F(RTCAudioDeviceTest, RecordingDevicesReturnsCount) {
  EXPECT_GE(device_->RecordingDevices(), -1);
}

TEST_F(RTCAudioDeviceTest, PlayoutDeviceName) {
  int count = device_->PlayoutDevices();
  if (count <= 0) {
    GTEST_SKIP() << "No playout devices available";
  }
  char name[RTCAudioDevice::kAdmMaxDeviceNameSize] = {0};
  char guid[RTCAudioDevice::kAdmMaxGuidSize] = {0};
  EXPECT_EQ(device_->PlayoutDeviceName(0, name, guid), 0);
}

TEST_F(RTCAudioDeviceTest, RecordingDeviceName) {
  int count = device_->RecordingDevices();
  if (count <= 0) {
    GTEST_SKIP() << "No recording devices available";
  }
  char name[RTCAudioDevice::kAdmMaxDeviceNameSize] = {0};
  char guid[RTCAudioDevice::kAdmMaxGuidSize] = {0};
  EXPECT_EQ(device_->RecordingDeviceName(0, name, guid), 0);
}

TEST_F(RTCAudioDeviceTest, SetAndGetMicrophoneVolume) {
  // Volume control depends on real hardware; skip when unavailable.
  if (device_->SetMicrophoneVolume(100) != 0) {
    GTEST_SKIP() << "Microphone volume control unavailable";
  }
  uint32_t volume = 0;
  EXPECT_EQ(device_->MicrophoneVolume(volume), 0);
}

TEST_F(RTCAudioDeviceTest, SetAndGetSpeakerVolume) {
  if (device_->SetSpeakerVolume(100) != 0) {
    GTEST_SKIP() << "Speaker volume control unavailable";
  }
  uint32_t volume = 0;
  EXPECT_EQ(device_->SpeakerVolume(volume), 0);
}

}  // namespace libwebrtc
