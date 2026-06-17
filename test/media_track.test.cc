#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_audio_source.h"
#include "rtc_audio_track.h"
#include "rtc_dummy_video_capturer.h"
#include "rtc_media_track.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_video_source.h"
#include "rtc_video_track.h"
#include "libwebrtc.h"

namespace libwebrtc {

// Fixture owning an initialized factory; helpers create real audio and video
// tracks via the C++ factory API.
class RTCMediaTrackTest : public ::testing::Test {
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

  scoped_refptr<RTCAudioTrack> CreateAudioTrack(const char* track_id) {
    scoped_refptr<RTCAudioSource> source =
        factory_->CreateAudioSource("test-audio-source");
    EXPECT_TRUE(source.get() != nullptr);
    if (!source.get()) return nullptr;
    return factory_->CreateAudioTrack(source, track_id);
  }

  scoped_refptr<RTCVideoTrack> CreateVideoTrack(const char* track_id) {
    scoped_refptr<RTCDummyVideoCapturer> capturer =
        factory_->CreateDummyVideoCapturer(30, 640, 480);
    EXPECT_TRUE(capturer.get() != nullptr);
    if (!capturer.get()) return nullptr;
    scoped_refptr<RTCVideoSource> source =
        factory_->CreateDummyVideoSource(capturer, "test-video-source");
    EXPECT_TRUE(source.get() != nullptr);
    if (!source.get()) return nullptr;
    return factory_->CreateVideoTrack(source, track_id);
  }

  scoped_refptr<RTCPeerConnectionFactory> factory_;
};

// --- Audio track ---

TEST_F(RTCMediaTrackTest, AudioTrackKindIsAudio) {
  scoped_refptr<RTCAudioTrack> track = CreateAudioTrack("audio-1");
  ASSERT_TRUE(track.get() != nullptr);
  EXPECT_STREQ(track->kind().c_string(), "audio");
}

TEST_F(RTCMediaTrackTest, AudioTrackIdMatches) {
  scoped_refptr<RTCAudioTrack> track = CreateAudioTrack("audio-id-xyz");
  ASSERT_TRUE(track.get() != nullptr);
  EXPECT_STREQ(track->id().c_string(), "audio-id-xyz");
}

TEST_F(RTCMediaTrackTest, AudioTrackStateIsLive) {
  scoped_refptr<RTCAudioTrack> track = CreateAudioTrack("audio-2");
  ASSERT_TRUE(track.get() != nullptr);
  EXPECT_EQ(track->state(), RTCMediaTrack::kLive);
}

TEST_F(RTCMediaTrackTest, AudioTrackEnabledGetSet) {
  scoped_refptr<RTCAudioTrack> track = CreateAudioTrack("audio-3");
  ASSERT_TRUE(track.get() != nullptr);

  EXPECT_TRUE(track->enabled());

  track->set_enabled(false);
  EXPECT_FALSE(track->enabled());

  track->set_enabled(true);
  EXPECT_TRUE(track->enabled());
}

TEST_F(RTCMediaTrackTest, AudioTrackSetVolumeDoesNotCrash) {
  scoped_refptr<RTCAudioTrack> track = CreateAudioTrack("audio-4");
  ASSERT_TRUE(track.get() != nullptr);
  track->SetVolume(5.0);
}

// --- Video track ---

TEST_F(RTCMediaTrackTest, VideoTrackKindIsVideo) {
  scoped_refptr<RTCVideoTrack> track = CreateVideoTrack("video-1");
  ASSERT_TRUE(track.get() != nullptr);
  EXPECT_STREQ(track->kind().c_string(), "video");
}

TEST_F(RTCMediaTrackTest, VideoTrackIdMatches) {
  scoped_refptr<RTCVideoTrack> track = CreateVideoTrack("video-id-1");
  ASSERT_TRUE(track.get() != nullptr);
  EXPECT_STREQ(track->id().c_string(), "video-id-1");
}

TEST_F(RTCMediaTrackTest, VideoTrackStateIsLive) {
  scoped_refptr<RTCVideoTrack> track = CreateVideoTrack("video-2");
  ASSERT_TRUE(track.get() != nullptr);
  EXPECT_EQ(track->state(), RTCMediaTrack::kLive);
}

TEST_F(RTCMediaTrackTest, VideoTrackEnabledGetSet) {
  scoped_refptr<RTCVideoTrack> track = CreateVideoTrack("video-3");
  ASSERT_TRUE(track.get() != nullptr);

  EXPECT_TRUE(track->enabled());

  track->set_enabled(false);
  EXPECT_FALSE(track->enabled());

  track->set_enabled(true);
  EXPECT_TRUE(track->enabled());
}

}  // namespace libwebrtc
