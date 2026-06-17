#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_audio_source.h"
#include "rtc_dummy_audio_source.h"
#include "rtc_dummy_video_capturer.h"
#include "rtc_mediaconstraints.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_video_source.h"
#include "libwebrtc.h"

namespace libwebrtc {

// --- RTCAudioSource / RTCVideoSource (default factory) ---

class RTCSourceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(LibWebRTC::Initialize());
    factory_ = LibWebRTC::CreateRTCPeerConnectionFactory();
    ASSERT_TRUE(factory_.get() != nullptr);
    ASSERT_TRUE(factory_->Initialize());
  }

  void TearDown() override {
    if (factory_) {
      factory_->Terminate();
      factory_ = nullptr;
    }
    LibWebRTC::Terminate();
  }

  scoped_refptr<RTCPeerConnectionFactory> factory_;
};

TEST_F(RTCSourceTest, CreateMicrophoneAudioSource) {
  scoped_refptr<RTCAudioSource> source =
      factory_->CreateAudioSource("mic-audio");
  ASSERT_TRUE(source.get() != nullptr);
  EXPECT_EQ(source->GetSourceType(), RTCAudioSource::SourceType::kMicrophone);
}

TEST_F(RTCSourceTest, CreateCustomAudioSource) {
  scoped_refptr<RTCAudioSource> source = factory_->CreateAudioSource(
      "custom-audio", RTCAudioSource::SourceType::kCustom);
  ASSERT_TRUE(source.get() != nullptr);
  EXPECT_EQ(source->GetSourceType(), RTCAudioSource::SourceType::kCustom);
}

TEST_F(RTCSourceTest, CreateDummyVideoSource) {
  scoped_refptr<RTCDummyVideoCapturer> capturer =
      factory_->CreateDummyVideoCapturer(30, 640, 480);
  ASSERT_TRUE(capturer.get() != nullptr);

  scoped_refptr<RTCVideoSource> source =
      factory_->CreateDummyVideoSource(capturer, "dummy-video");
  ASSERT_TRUE(source.get() != nullptr);
}

TEST_F(RTCSourceTest, CreateCustomVideoSource) {
  scoped_refptr<RTCMediaConstraints> constraints =
      RTCMediaConstraints::Create();
  scoped_refptr<RTCVideoSource> source =
      factory_->CreateCustomVideoSource("custom-video", constraints);
  ASSERT_TRUE(source.get() != nullptr);
  EXPECT_EQ(source->GetSourceType(), RTCVideoSource::SourceType::kCustom);
}

// --- RTCDummyAudioSource (factory initialized with dummy audio) ---

class RTCDummyAudioSourceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(LibWebRTC::Initialize());
    factory_ = LibWebRTC::CreateRTCPeerConnectionFactory();
    ASSERT_TRUE(factory_.get() != nullptr);
    // Enable dummy audio so dummy audio sources can be created.
    ASSERT_TRUE(factory_->Initialize(true));
    source_ = factory_->CreateDummyAudioSource("dummy-audio", 48000, 2);
    ASSERT_TRUE(source_.get() != nullptr);
  }

  void TearDown() override {
    source_ = nullptr;
    if (factory_) {
      factory_->Terminate();
      factory_ = nullptr;
    }
    LibWebRTC::Terminate();
  }

  scoped_refptr<RTCPeerConnectionFactory> factory_;
  scoped_refptr<RTCDummyAudioSource> source_;
};

TEST_F(RTCDummyAudioSourceTest, ReportsFormat) {
  EXPECT_GT(source_->bits_per_sample(), 0);
  EXPECT_EQ(source_->sample_rate_hz(), 48000);
  EXPECT_EQ(source_->number_of_channels(), 2u);
  EXPECT_GE(source_->number_of_frames(), 0u);
}

TEST_F(RTCDummyAudioSourceTest, StartStopAndState) {
  RTCCaptureState started = source_->Start();
  EXPECT_EQ(started, RTCCaptureState::CS_RUNNING);
  EXPECT_TRUE(source_->IsRunning());
  EXPECT_EQ(source_->CaptureState(), RTCCaptureState::CS_RUNNING);

  source_->Stop();
}

}  // namespace libwebrtc
