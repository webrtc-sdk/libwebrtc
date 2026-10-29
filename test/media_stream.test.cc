#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_audio_source.h"
#include "rtc_audio_track.h"
#include "rtc_dummy_video_capturer.h"
#include "rtc_media_stream.h"
#include "rtc_media_track.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_video_source.h"
#include "rtc_video_track.h"
#include "libwebrtc.h"

namespace libwebrtc {

// Fixture owning an initialized factory and a media stream created from it,
// exercising the C++ RTCMediaStream API (factory->CreateStream).
class RTCMediaStreamTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(LibWebRTC::Initialize());
    factory_ = LibWebRTC::CreateRTCPeerConnectionFactory();
    ASSERT_TRUE(factory_.get() != nullptr);
    ASSERT_TRUE(factory_->Initialize());
    stream_ = factory_->CreateStream("test-stream-id");
    ASSERT_TRUE(stream_.get() != nullptr);
  }

  void TearDown() override {
    stream_ = nullptr;
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
  scoped_refptr<RTCMediaStream> stream_;
};

TEST_F(RTCMediaStreamTest, IdMatches) {
  EXPECT_STREQ(stream_->id().c_string(), "test-stream-id");
}

TEST_F(RTCMediaStreamTest, LabelIsAccessible) {
  // label() should return a non-crashing string (commonly the stream id).
  string label = stream_->label();
  EXPECT_GE(label.size(), 0u);
}

TEST_F(RTCMediaStreamTest, InitiallyEmpty) {
  EXPECT_EQ(stream_->audio_tracks().size(), 0u);
  EXPECT_EQ(stream_->video_tracks().size(), 0u);
}

TEST_F(RTCMediaStreamTest, AddAndRemoveAudioTrack) {
  scoped_refptr<RTCAudioTrack> track = CreateAudioTrack("audio-stream-1");
  ASSERT_TRUE(track.get() != nullptr);

  EXPECT_TRUE(stream_->AddTrack(track));
  EXPECT_EQ(stream_->audio_tracks().size(), 1u);

  scoped_refptr<RTCAudioTrack> found =
      stream_->FindAudioTrack("audio-stream-1");
  EXPECT_TRUE(found.get() != nullptr);

  EXPECT_TRUE(stream_->RemoveTrack(track));
  EXPECT_EQ(stream_->audio_tracks().size(), 0u);
}

TEST_F(RTCMediaStreamTest, AddAndRemoveVideoTrack) {
  scoped_refptr<RTCVideoTrack> track = CreateVideoTrack("video-stream-1");
  ASSERT_TRUE(track.get() != nullptr);

  EXPECT_TRUE(stream_->AddTrack(track));
  EXPECT_EQ(stream_->video_tracks().size(), 1u);

  scoped_refptr<RTCVideoTrack> found =
      stream_->FindVideoTrack("video-stream-1");
  EXPECT_TRUE(found.get() != nullptr);

  EXPECT_TRUE(stream_->RemoveTrack(track));
  EXPECT_EQ(stream_->video_tracks().size(), 0u);
}

TEST_F(RTCMediaStreamTest, TracksReportsBothKinds) {
  scoped_refptr<RTCAudioTrack> audio = CreateAudioTrack("a-1");
  scoped_refptr<RTCVideoTrack> video = CreateVideoTrack("v-1");
  ASSERT_TRUE(audio.get() != nullptr);
  ASSERT_TRUE(video.get() != nullptr);

  EXPECT_TRUE(stream_->AddTrack(audio));
  EXPECT_TRUE(stream_->AddTrack(video));

  EXPECT_EQ(stream_->audio_tracks().size(), 1u);
  EXPECT_EQ(stream_->video_tracks().size(), 1u);
  EXPECT_EQ(stream_->tracks().size(), 2u);
}

TEST_F(RTCMediaStreamTest, FindMissingTrackReturnsNull) {
  EXPECT_TRUE(stream_->FindAudioTrack("does-not-exist").get() == nullptr);
  EXPECT_TRUE(stream_->FindVideoTrack("does-not-exist").get() == nullptr);
}

}  // namespace libwebrtc
