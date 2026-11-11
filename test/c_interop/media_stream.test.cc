#include <cstring>
#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// Negative-path tests that don't need a media stream.

TEST(RTCMediaStreamNegative, AddAudioTrackWithNullStreamReturnsFalse) {
    EXPECT_EQ(RTCMediaStream_AddAudioTrack(nullptr, nullptr),
              rtcBool32::kFalse);
}

TEST(RTCMediaStreamNegative, NumberOfAudioTracksWithNullStreamReturnsZero) {
    EXPECT_EQ(RTCMediaStream_NumberOfAudioTracks(nullptr), 0);
}

TEST(RTCMediaStreamNegative, GetIdWithNullStreamFails) {
    char id[128] = {0};
    EXPECT_EQ(RTCMediaStream_GetId(nullptr, id, sizeof(id)),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCMediaStreamListNegative, GetCountWithNullHandleReturnsZero) {
    EXPECT_EQ(RTCMediaStreamList_GetCount(nullptr), 0);
}

// Fixture owning an initialized factory and a media stream from it.
class RTCMediaStreamTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_EQ(LibWebRTC_Initialize(), rtcBool32::kTrue);
        factory_ = LibWebRTC_CreateRTCPeerConnectionFactory();
        ASSERT_NE(factory_, nullptr);
        ASSERT_EQ(RTCPeerConnectionFactory_Initialize(factory_),
                  rtcBool32::kTrue);
        ASSERT_EQ(RTCPeerConnectionFactory_CreateStream(
                      factory_, "test-stream-id", &stream_),
                  rtcResultU4::kSuccess);
        ASSERT_NE(stream_, nullptr);
    }

    void TearDown() override {
        if (stream_) {
            RefCountedObject_Release(stream_);
            stream_ = nullptr;
        }
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
    rtcMediaStreamHandle stream_ = nullptr;
};

TEST_F(RTCMediaStreamTest, GetId) {
    char id[256] = {0};
    EXPECT_EQ(RTCMediaStream_GetId(stream_, id, sizeof(id)),
              rtcResultU4::kSuccess);
    EXPECT_STREQ(id, "test-stream-id");
}

TEST_F(RTCMediaStreamTest, GetLabel) {
    char label[256] = {0};
    EXPECT_EQ(RTCMediaStream_GetLabel(stream_, label, sizeof(label)),
              rtcResultU4::kSuccess);
}

TEST_F(RTCMediaStreamTest, InitiallyEmpty) {
    EXPECT_EQ(RTCMediaStream_NumberOfAudioTracks(stream_), 0);
    EXPECT_EQ(RTCMediaStream_NumberOfVideoTracks(stream_), 0);
}

TEST_F(RTCMediaStreamTest, AddAndRemoveAudioTrack) {
    rtcAudioTrackHandle track = CreateAudioTrack("audio-stream-1");
    ASSERT_NE(track, nullptr);

    EXPECT_EQ(RTCMediaStream_AddAudioTrack(stream_, track), rtcBool32::kTrue);
    EXPECT_EQ(RTCMediaStream_NumberOfAudioTracks(stream_), 1);

    rtcAudioTrackHandle fetched = nullptr;
    EXPECT_EQ(RTCMediaStream_GetAudioTrack(stream_, 0, &fetched),
              rtcResultU4::kSuccess);
    EXPECT_NE(fetched, nullptr);
    if (fetched) RefCountedObject_Release(fetched);

    rtcAudioTrackHandle found = nullptr;
    EXPECT_EQ(
        RTCMediaStream_FindAudioTrack(stream_, "audio-stream-1", &found),
        rtcResultU4::kSuccess);
    EXPECT_NE(found, nullptr);
    if (found) RefCountedObject_Release(found);

    EXPECT_EQ(RTCMediaStream_RemoveAudioTrack(stream_, track),
              rtcBool32::kTrue);
    EXPECT_EQ(RTCMediaStream_NumberOfAudioTracks(stream_), 0);

    RefCountedObject_Release(track);
}

TEST_F(RTCMediaStreamTest, AddAndRemoveVideoTrack) {
    rtcDummyVideoCapturerHandle capturer = nullptr;
    rtcVideoTrackHandle track = CreateVideoTrack("video-stream-1", &capturer);
    ASSERT_NE(track, nullptr);

    EXPECT_EQ(RTCMediaStream_AddVideoTrack(stream_, track), rtcBool32::kTrue);
    EXPECT_EQ(RTCMediaStream_NumberOfVideoTracks(stream_), 1);

    rtcVideoTrackHandle fetched = nullptr;
    EXPECT_EQ(RTCMediaStream_GetVideoTrack(stream_, 0, &fetched),
              rtcResultU4::kSuccess);
    EXPECT_NE(fetched, nullptr);
    if (fetched) RefCountedObject_Release(fetched);

    EXPECT_EQ(RTCMediaStream_RemoveVideoTrack(stream_, track),
              rtcBool32::kTrue);
    EXPECT_EQ(RTCMediaStream_NumberOfVideoTracks(stream_), 0);

    RefCountedObject_Release(track);
    if (capturer) RefCountedObject_Release(capturer);
}

TEST_F(RTCMediaStreamTest, GetAudioTrackOutOfRangeFails) {
    rtcAudioTrackHandle fetched = nullptr;
    EXPECT_EQ(RTCMediaStream_GetAudioTrack(stream_, 0, &fetched),
              rtcResultU4::kOutOfRange);
    EXPECT_EQ(fetched, nullptr);
}

TEST_F(RTCMediaStreamTest, AddAudioTrackWithNullTrackReturnsFalse) {
    EXPECT_EQ(RTCMediaStream_AddAudioTrack(stream_, nullptr),
              rtcBool32::kFalse);
}

}  // namespace libwebrtc
