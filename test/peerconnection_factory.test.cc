#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_peerconnection_factory.h"
#include "rtc_peerconnection.h"
#include "rtc_audio_device.h"
#include "rtc_audio_processing.h"
#include "rtc_audio_source.h"
#include "rtc_audio_track.h"
#include "rtc_video_device.h"
#include "rtc_video_source.h"
#include "rtc_video_track.h"
#include "rtc_dummy_video_capturer.h"
#include "rtc_media_stream.h"
#include "rtc_mediaconstraints.h"
#include "libwebrtc.h"

namespace libwebrtc {

// Fixture that owns an initialized factory for the duration of each test.
// Mirrors test/c_interop/peerconnection_factory.test.cc but exercises the
// C++ class API directly.
class RTCPeerConnectionFactoryTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(LibWebRTC::Initialize());
    factory_ = LibWebRTC::CreateRTCPeerConnectionFactory();
    ASSERT_TRUE(factory_.get() != nullptr);
    ASSERT_TRUE(factory_->Initialize());
  }

  void TearDown() override {
    if (factory_) {
      EXPECT_TRUE(factory_->Terminate());
      factory_ = nullptr;
    }
    LibWebRTC::Terminate();
  }

  scoped_refptr<RTCPeerConnectionFactory> factory_;
};

TEST_F(RTCPeerConnectionFactoryTest, CreatePeerConnectionWithDefaultConfig) {
  RTCConfiguration config;
  scoped_refptr<RTCMediaConstraints> constraints = RTCMediaConstraints::Create();
  ASSERT_TRUE(constraints.get() != nullptr);

  scoped_refptr<RTCPeerConnection> pc = factory_->Create(config, constraints);
  ASSERT_TRUE(pc.get() != nullptr);
  EXPECT_TRUE(pc->IsInitialized());

  pc->Close();
  factory_->Delete(pc);
}

TEST_F(RTCPeerConnectionFactoryTest, CreatePeerConnectionWithMediaConstraints) {
  RTCConfiguration config;
  scoped_refptr<RTCMediaConstraints> constraints = RTCMediaConstraints::Create();
  ASSERT_TRUE(constraints.get() != nullptr);
  constraints->AddMandatoryConstraint(RTCMediaConstraints::kOfferToReceiveAudio,
                                      RTCMediaConstraints::kValueTrue);

  scoped_refptr<RTCPeerConnection> pc = factory_->Create(config, constraints);
  ASSERT_TRUE(pc.get() != nullptr);

  pc->Close();
  factory_->Delete(pc);
}

TEST_F(RTCPeerConnectionFactoryTest, GetAudioDeviceReturnsObject) {
  scoped_refptr<RTCAudioDevice> device = factory_->GetAudioDevice();
  EXPECT_NE(device.get(), nullptr);
}

TEST_F(RTCPeerConnectionFactoryTest, GetVideoDeviceReturnsObject) {
  scoped_refptr<RTCVideoDevice> device = factory_->GetVideoDevice();
  EXPECT_NE(device.get(), nullptr);
}

TEST_F(RTCPeerConnectionFactoryTest, GetAudioProcessingReturnsObject) {
  scoped_refptr<RTCAudioProcessing> ap = factory_->GetAudioProcessing();
  EXPECT_NE(ap.get(), nullptr);
}

TEST_F(RTCPeerConnectionFactoryTest, CreateAudioSourceAndAudioTrack) {
  scoped_refptr<RTCAudioSource> source =
      factory_->CreateAudioSource("test-audio-source");
  ASSERT_TRUE(source.get() != nullptr);

  scoped_refptr<RTCAudioTrack> track =
      factory_->CreateAudioTrack(source, "test-audio-track");
  ASSERT_TRUE(track.get() != nullptr);
  EXPECT_EQ(track->id().std_string(), "test-audio-track");
}

TEST_F(RTCPeerConnectionFactoryTest, CreateDummyVideoSourceAndVideoTrack) {
  scoped_refptr<RTCDummyVideoCapturer> capturer =
      factory_->CreateDummyVideoCapturer(30, 640, 480);
  ASSERT_TRUE(capturer.get() != nullptr);

  scoped_refptr<RTCVideoSource> source =
      factory_->CreateDummyVideoSource(capturer, "test-video-source");
  ASSERT_TRUE(source.get() != nullptr);

  scoped_refptr<RTCVideoTrack> track =
      factory_->CreateVideoTrack(source, "test-video-track");
  ASSERT_TRUE(track.get() != nullptr);
  EXPECT_EQ(track->id().std_string(), "test-video-track");
}

TEST_F(RTCPeerConnectionFactoryTest, CreateStreamReturnsObject) {
  scoped_refptr<RTCMediaStream> stream = factory_->CreateStream("test-stream-id");
  ASSERT_TRUE(stream.get() != nullptr);
  EXPECT_EQ(stream->id().std_string(), "test-stream-id");
}

TEST_F(RTCPeerConnectionFactoryTest, GetRtpSenderCapabilitiesAudioAndVideo) {
  scoped_refptr<RTCRtpCapabilities> audio_caps =
      factory_->GetRtpSenderCapabilities(RTCMediaType::AUDIO);
  EXPECT_NE(audio_caps.get(), nullptr);

  scoped_refptr<RTCRtpCapabilities> video_caps =
      factory_->GetRtpSenderCapabilities(RTCMediaType::VIDEO);
  EXPECT_NE(video_caps.get(), nullptr);
}

TEST_F(RTCPeerConnectionFactoryTest, GetRtpReceiverCapabilitiesAudioAndVideo) {
  scoped_refptr<RTCRtpCapabilities> audio_caps =
      factory_->GetRtpReceiverCapabilities(RTCMediaType::AUDIO);
  EXPECT_NE(audio_caps.get(), nullptr);

  scoped_refptr<RTCRtpCapabilities> video_caps =
      factory_->GetRtpReceiverCapabilities(RTCMediaType::VIDEO);
  EXPECT_NE(video_caps.get(), nullptr);
}

TEST_F(RTCPeerConnectionFactoryTest, GetUseDummyAudioReportsFalseByDefault) {
  EXPECT_FALSE(factory_->GetUseDummyAudio());
}

}  // namespace libwebrtc
