#include <cstring>
#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// Negative-path tests that don't need a media track.

TEST(RTCMediaTrackNegative, GetStateWithNullHandleReturnsUnknown) {
    EXPECT_EQ(RTCMediaTrack_GetState(nullptr), rtcTrackState::kUnknown);
}

TEST(RTCMediaTrackNegative, GetEnabledWithNullHandleReturnsFalse) {
    EXPECT_EQ(RTCMediaTrack_GetEnabled(nullptr), rtcBool32::kFalse);
}

TEST(RTCMediaTrackNegative, GetKindWithNullHandleFails) {
    char kind[32] = {0};
    EXPECT_EQ(RTCMediaTrack_GetKind(nullptr, kind, sizeof(kind)),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCAudioTrackNegative, SetVolumeWithNullHandleFails) {
    EXPECT_EQ(RTCAudioTrack_SetVolume(nullptr, 1.0),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCVideoTrackNegative, AddRendererWithNullHandleFails) {
    EXPECT_EQ(RTCVideoTrack_AddRenderer(nullptr, nullptr),
              rtcResultU4::kInvalidNativeHandle);
}

// Fixture owning an initialized factory; helpers create real tracks.
class RTCMediaTrackTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_EQ(LibWebRTC_Initialize(), rtcBool32::kTrue);
        factory_ = LibWebRTC_CreateRTCPeerConnectionFactory();
        ASSERT_NE(factory_, nullptr);
        ASSERT_EQ(RTCPeerConnectionFactory_Initialize(factory_),
                  rtcBool32::kTrue);
    }

    void TearDown() override {
        if (factory_) {
            EXPECT_EQ(RTCPeerConnectionFactory_Terminate(factory_),
                      rtcBool32::kTrue);
            RefCountedObject_Release(factory_);
            factory_ = nullptr;
        }
        LibWebRTC_Terminate();
    }

    rtcAudioTrackHandle CreateAudioTrack(const char* track_id) {
        rtcAudioSourceHandle source = nullptr;
        EXPECT_EQ(RTCPeerConnectionFactory_CreateAudioSource(
                      factory_, "test-audio-source", &source),
                  rtcResultU4::kSuccess);
        if (!source) return nullptr;

        rtcAudioTrackHandle track = nullptr;
        EXPECT_EQ(RTCPeerConnectionFactory_CreateAudioTrack(
                      factory_, source, track_id, &track),
                  rtcResultU4::kSuccess);
        RefCountedObject_Release(source);
        return track;
    }

    // Returns the video track; outputs the capturer the caller must release.
    rtcVideoTrackHandle CreateVideoTrack(const char* track_id,
                                         rtcDummyVideoCapturerHandle* capturer) {
        *capturer = nullptr;
        EXPECT_EQ(RTCPeerConnectionFactory_CreateDummyVideoCapturer(
                      factory_, 30, 640, 480, capturer),
                  rtcResultU4::kSuccess);
        if (!*capturer) return nullptr;

        rtcVideoSourceHandle source = nullptr;
        EXPECT_EQ(RTCPeerConnectionFactory_CreateDummyVideoSource(
                      factory_, *capturer, "test-video-source", &source),
                  rtcResultU4::kSuccess);
        if (!source) return nullptr;

        rtcVideoTrackHandle track = nullptr;
        EXPECT_EQ(RTCPeerConnectionFactory_CreateVideoTrack(
                      factory_, source, track_id, &track),
                  rtcResultU4::kSuccess);
        RefCountedObject_Release(source);
        return track;
    }

    rtcPeerConnectionFactoryHandle factory_ = nullptr;
};

// --- Audio track ---

TEST_F(RTCMediaTrackTest, AudioTrackKindIsAudio) {
    rtcAudioTrackHandle track = CreateAudioTrack("audio-1");
    ASSERT_NE(track, nullptr);

    char kind[32] = {0};
    EXPECT_EQ(RTCMediaTrack_GetKind(track, kind, sizeof(kind)),
              rtcResultU4::kSuccess);
    EXPECT_STREQ(kind, "audio");

    RefCountedObject_Release(track);
}

TEST_F(RTCMediaTrackTest, AudioTrackIdMatches) {
    rtcAudioTrackHandle track = CreateAudioTrack("audio-id-xyz");
    ASSERT_NE(track, nullptr);

    char id[256] = {0};
    EXPECT_EQ(RTCMediaTrack_GetId(track, id, sizeof(id)),
              rtcResultU4::kSuccess);
    EXPECT_STREQ(id, "audio-id-xyz");

    RefCountedObject_Release(track);
}

TEST_F(RTCMediaTrackTest, AudioTrackStateIsLive) {
    rtcAudioTrackHandle track = CreateAudioTrack("audio-2");
    ASSERT_NE(track, nullptr);

    EXPECT_EQ(RTCMediaTrack_GetState(track), rtcTrackState::kLive);

    RefCountedObject_Release(track);
}

TEST_F(RTCMediaTrackTest, AudioTrackEnabledGetSet) {
    rtcAudioTrackHandle track = CreateAudioTrack("audio-3");
    ASSERT_NE(track, nullptr);

    EXPECT_EQ(RTCMediaTrack_GetEnabled(track), rtcBool32::kTrue);

    RTCMediaTrack_SetEnabled(track, rtcBool32::kFalse);
    EXPECT_EQ(RTCMediaTrack_GetEnabled(track), rtcBool32::kFalse);

    RTCMediaTrack_SetEnabled(track, rtcBool32::kTrue);
    EXPECT_EQ(RTCMediaTrack_GetEnabled(track), rtcBool32::kTrue);

    RefCountedObject_Release(track);
}

TEST_F(RTCMediaTrackTest, AudioTrackGetKindBufferTooSmall) {
    rtcAudioTrackHandle track = CreateAudioTrack("audio-4");
    ASSERT_NE(track, nullptr);

    char kind[3] = {0};
    EXPECT_EQ(RTCMediaTrack_GetKind(track, kind, sizeof(kind)),
              rtcResultU4::kBufferTooSmall);

    RefCountedObject_Release(track);
}

TEST_F(RTCMediaTrackTest, AudioTrackSetVolume) {
    rtcAudioTrackHandle track = CreateAudioTrack("audio-5");
    ASSERT_NE(track, nullptr);

    EXPECT_EQ(RTCAudioTrack_SetVolume(track, 5.0), rtcResultU4::kSuccess);

    RefCountedObject_Release(track);
}

// --- Video track ---

TEST_F(RTCMediaTrackTest, VideoTrackKindIsVideo) {
    rtcDummyVideoCapturerHandle capturer = nullptr;
    rtcVideoTrackHandle track = CreateVideoTrack("video-1", &capturer);
    ASSERT_NE(track, nullptr);

    char kind[32] = {0};
    EXPECT_EQ(RTCMediaTrack_GetKind(track, kind, sizeof(kind)),
              rtcResultU4::kSuccess);
    EXPECT_STREQ(kind, "video");

    RefCountedObject_Release(track);
    if (capturer) RefCountedObject_Release(capturer);
}

TEST_F(RTCMediaTrackTest, VideoTrackIdAndEnabled) {
    rtcDummyVideoCapturerHandle capturer = nullptr;
    rtcVideoTrackHandle track = CreateVideoTrack("video-id-1", &capturer);
    ASSERT_NE(track, nullptr);

    char id[256] = {0};
    EXPECT_EQ(RTCMediaTrack_GetId(track, id, sizeof(id)),
              rtcResultU4::kSuccess);
    EXPECT_STREQ(id, "video-id-1");

    EXPECT_EQ(RTCMediaTrack_GetEnabled(track), rtcBool32::kTrue);
    RTCMediaTrack_SetEnabled(track, rtcBool32::kFalse);
    EXPECT_EQ(RTCMediaTrack_GetEnabled(track), rtcBool32::kFalse);

    RefCountedObject_Release(track);
    if (capturer) RefCountedObject_Release(capturer);
}

TEST_F(RTCMediaTrackTest, VideoTrackAddRendererWithNullRendererFails) {
    rtcDummyVideoCapturerHandle capturer = nullptr;
    rtcVideoTrackHandle track = CreateVideoTrack("video-2", &capturer);
    ASSERT_NE(track, nullptr);

    EXPECT_EQ(RTCVideoTrack_AddRenderer(track, nullptr),
              rtcResultU4::kInvalidNativeHandle);

    RefCountedObject_Release(track);
    if (capturer) RefCountedObject_Release(capturer);
}

}  // namespace libwebrtc
