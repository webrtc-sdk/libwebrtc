#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {
/*
TEST(LibWebRTC_CAPI, ConstructDestruct) {
    EXPECT_EQ(LibWebRTC_Initialize(), rtcBool32::kTrue);

    RTC_LOG(LS_INFO) << "LibWebRTC_Initialize() called";

    auto peer_connection_factory = LibWebRTC_CreateRTCPeerConnectionFactory();

    EXPECT_NE(peer_connection_factory, nullptr);

    RTC_LOG(LS_INFO) << "LibWebRTC_CreateRTCPeerConnectionFactory() called, peer_connection_factory " << peer_connection_factory;

    EXPECT_EQ(RTCPeerConnectionFactory_Initialize(peer_connection_factory), rtcBool32::kTrue);

    EXPECT_EQ(RTCPeerConnectionFactory_Terminate(peer_connection_factory), rtcBool32::kTrue);

    RTC_LOG(LS_INFO) << "RefCountedObject_Release() called";
    RefCountedObject_Release(peer_connection_factory);

    LibWebRTC_Terminate();
    RTC_LOG(LS_INFO) << "LibWebRTC_Terminate() called";
}
*/
// Negative-path tests that don't need an initialized factory.

TEST(RTCPeerConnectionFactoryNegative, InitializeWithNullFactoryReturnsFalse) {
    EXPECT_EQ(RTCPeerConnectionFactory_Initialize(nullptr), rtcBool32::kFalse);
}

TEST(RTCPeerConnectionFactoryNegative, TerminateWithNullFactoryReturnsFalse) {
    EXPECT_EQ(RTCPeerConnectionFactory_Terminate(nullptr), rtcBool32::kFalse);
}

TEST(RTCPeerConnectionFactoryNegative, CreatePeerConnectionWithNullFactoryFails) {
    rtcPeerConnectionConfiguration config{};
    rtcPeerConnectionHandle pc = nullptr;
    EXPECT_NE(RTCPeerConnectionFactory_CreatePeerConnection(
                  nullptr, &config, nullptr, &pc),
              rtcResultU4::kSuccess);
    EXPECT_EQ(pc, nullptr);
}

TEST(RTCPeerConnectionFactoryNegative, CreatePeerConnectionWithNullOutPointerFails) {
    EXPECT_EQ(LibWebRTC_Initialize(), rtcBool32::kTrue);
    auto factory = LibWebRTC_CreateRTCPeerConnectionFactory();
    ASSERT_NE(factory, nullptr);
    ASSERT_EQ(RTCPeerConnectionFactory_Initialize(factory), rtcBool32::kTrue);

    rtcPeerConnectionConfiguration config{};
    EXPECT_NE(RTCPeerConnectionFactory_CreatePeerConnection(
                  factory, &config, nullptr, nullptr),
              rtcResultU4::kSuccess);

    EXPECT_EQ(RTCPeerConnectionFactory_Terminate(factory), rtcBool32::kTrue);
    RefCountedObject_Release(factory);
    LibWebRTC_Terminate();
}

// Fixture that owns an initialized factory for the duration of each test.
class RTCPeerConnectionFactoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_EQ(LibWebRTC_Initialize(), rtcBool32::kTrue);
        factory_ = LibWebRTC_CreateRTCPeerConnectionFactory();
        ASSERT_NE(factory_, nullptr);
        ASSERT_EQ(RTCPeerConnectionFactory_Initialize(factory_), rtcBool32::kTrue);
    }

    void TearDown() override {
        if (factory_) {
            EXPECT_EQ(RTCPeerConnectionFactory_Terminate(factory_), rtcBool32::kTrue);
            RefCountedObject_Release(factory_);
            factory_ = nullptr;
        }
        LibWebRTC_Terminate();
    }

    rtcPeerConnectionFactoryHandle factory_ = nullptr;
};

TEST_F(RTCPeerConnectionFactoryTest, CreatePeerConnectionWithDefaultConfig) {
    rtcPeerConnectionConfiguration config{};
    rtcPeerConnectionHandle pc = nullptr;

    EXPECT_EQ(RTCPeerConnectionFactory_CreatePeerConnection(
                  factory_, &config, nullptr, &pc),
              rtcResultU4::kSuccess);
    ASSERT_NE(pc, nullptr);

    EXPECT_EQ(RTCPeerConnectionFactory_DeletePeerConnection(factory_, pc),
              rtcResultU4::kSuccess);
    RefCountedObject_Release(pc);
}

TEST_F(RTCPeerConnectionFactoryTest, CreatePeerConnectionWithMediaConstraints) {
    rtcPeerConnectionConfiguration config{};
    rtcMediaConstraintsHandle constraints = MediaConstraints_Create();
    ASSERT_NE(constraints, nullptr);

    rtcPeerConnectionHandle pc = nullptr;
    EXPECT_EQ(RTCPeerConnectionFactory_CreatePeerConnection(
                  factory_, &config, constraints, &pc),
              rtcResultU4::kSuccess);
    ASSERT_NE(pc, nullptr);

    EXPECT_EQ(RTCPeerConnectionFactory_DeletePeerConnection(factory_, pc),
              rtcResultU4::kSuccess);
    RefCountedObject_Release(pc);
    // CreatePeerConnection takes ownership of the constraints handle (see impl).
}

TEST_F(RTCPeerConnectionFactoryTest, DeletePeerConnectionWithNullHandleFails) {
    EXPECT_NE(RTCPeerConnectionFactory_DeletePeerConnection(factory_, nullptr),
              rtcResultU4::kSuccess);
}

TEST_F(RTCPeerConnectionFactoryTest, GetAudioDeviceReturnsHandle) {
    rtcAudioDeviceHandle device = nullptr;
    EXPECT_EQ(RTCPeerConnectionFactory_GetAudioDevice(factory_, &device),
              rtcResultU4::kSuccess);
    EXPECT_NE(device, nullptr);
    if (device) RefCountedObject_Release(device);
}

TEST_F(RTCPeerConnectionFactoryTest, GetVideoDeviceReturnsHandle) {
    rtcVideoDeviceHandle device = nullptr;
    EXPECT_EQ(RTCPeerConnectionFactory_GetVideoDevice(factory_, &device),
              rtcResultU4::kSuccess);
    EXPECT_NE(device, nullptr);
    if (device) RefCountedObject_Release(device);
}

TEST_F(RTCPeerConnectionFactoryTest, CreateAudioSourceAndAudioTrack) {
    rtcAudioSourceHandle source = nullptr;
    EXPECT_EQ(RTCPeerConnectionFactory_CreateAudioSource(
                  factory_, "test-audio-source", &source),
              rtcResultU4::kSuccess);
    ASSERT_NE(source, nullptr);

    rtcAudioTrackHandle track = nullptr;
    EXPECT_EQ(RTCPeerConnectionFactory_CreateAudioTrack(
                  factory_, source, "test-audio-track", &track),
              rtcResultU4::kSuccess);
    EXPECT_NE(track, nullptr);

    if (track) RefCountedObject_Release(track);
    RefCountedObject_Release(source);
}

TEST_F(RTCPeerConnectionFactoryTest, CreateAudioTrackWithNullSourceFails) {
    rtcAudioTrackHandle track = nullptr;
    EXPECT_NE(RTCPeerConnectionFactory_CreateAudioTrack(
                  factory_, nullptr, "test-audio-track", &track),
              rtcResultU4::kSuccess);
    EXPECT_EQ(track, nullptr);
}

TEST_F(RTCPeerConnectionFactoryTest, CreateDummyVideoCapturerRejectsZeroParams) {
    rtcDummyVideoCapturerHandle capturer = nullptr;
    EXPECT_EQ(RTCPeerConnectionFactory_CreateDummyVideoCapturer(
                  factory_, 0, 640, 480, &capturer),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(RTCPeerConnectionFactory_CreateDummyVideoCapturer(
                  factory_, 30, 0, 480, &capturer),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(RTCPeerConnectionFactory_CreateDummyVideoCapturer(
                  factory_, 30, 640, 0, &capturer),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(capturer, nullptr);
}

TEST_F(RTCPeerConnectionFactoryTest, CreateDummyVideoSourceAndVideoTrack) {
    rtcDummyVideoCapturerHandle capturer = nullptr;
    EXPECT_EQ(RTCPeerConnectionFactory_CreateDummyVideoCapturer(
                  factory_, 30, 640, 480, &capturer),
              rtcResultU4::kSuccess);
    ASSERT_NE(capturer, nullptr);

    rtcVideoSourceHandle source = nullptr;
    EXPECT_EQ(RTCPeerConnectionFactory_CreateDummyVideoSource(
                  factory_, capturer, "test-video-source", &source),
              rtcResultU4::kSuccess);
    ASSERT_NE(source, nullptr);

    rtcVideoTrackHandle track = nullptr;
    EXPECT_EQ(RTCPeerConnectionFactory_CreateVideoTrack(
                  factory_, source, "test-video-track", &track),
              rtcResultU4::kSuccess);
    EXPECT_NE(track, nullptr);

    if (track) RefCountedObject_Release(track);
    RefCountedObject_Release(source);
    RefCountedObject_Release(capturer);
}

TEST_F(RTCPeerConnectionFactoryTest, CreateVideoTrackWithNullSourceFails) {
    rtcVideoTrackHandle track = nullptr;
    EXPECT_NE(RTCPeerConnectionFactory_CreateVideoTrack(
                  factory_, nullptr, "test-video-track", &track),
              rtcResultU4::kSuccess);
    EXPECT_EQ(track, nullptr);
}

TEST_F(RTCPeerConnectionFactoryTest, CreateStreamReturnsHandle) {
    rtcMediaStreamHandle stream = nullptr;
    EXPECT_EQ(RTCPeerConnectionFactory_CreateStream(
                  factory_, "test-stream-id", &stream),
              rtcResultU4::kSuccess);
    EXPECT_NE(stream, nullptr);
    if (stream) RefCountedObject_Release(stream);
}

TEST_F(RTCPeerConnectionFactoryTest, GetRtpSenderCapabilitiesAudioAndVideo) {
    rtcRtpCapabilitiesHandle audio_caps = nullptr;
    EXPECT_EQ(RTCPeerConnectionFactory_GetRtpSenderCapabilities(
                  factory_, RTCMediaType::AUDIO, &audio_caps),
              rtcResultU4::kSuccess);
    EXPECT_NE(audio_caps, nullptr);
    if (audio_caps) RefCountedObject_Release(audio_caps);

    rtcRtpCapabilitiesHandle video_caps = nullptr;
    EXPECT_EQ(RTCPeerConnectionFactory_GetRtpSenderCapabilities(
                  factory_, RTCMediaType::VIDEO, &video_caps),
              rtcResultU4::kSuccess);
    EXPECT_NE(video_caps, nullptr);
    if (video_caps) RefCountedObject_Release(video_caps);
}

TEST_F(RTCPeerConnectionFactoryTest, GetRtpReceiverCapabilitiesAudioAndVideo) {
    rtcRtpCapabilitiesHandle audio_caps = nullptr;
    EXPECT_EQ(RTCPeerConnectionFactory_GetRtpReceiverCapabilities(
                  factory_, RTCMediaType::AUDIO, &audio_caps),
              rtcResultU4::kSuccess);
    EXPECT_NE(audio_caps, nullptr);
    if (audio_caps) RefCountedObject_Release(audio_caps);

    rtcRtpCapabilitiesHandle video_caps = nullptr;
    EXPECT_EQ(RTCPeerConnectionFactory_GetRtpReceiverCapabilities(
                  factory_, RTCMediaType::VIDEO, &video_caps),
              rtcResultU4::kSuccess);
    EXPECT_NE(video_caps, nullptr);
    if (video_caps) RefCountedObject_Release(video_caps);
}

TEST_F(RTCPeerConnectionFactoryTest, GetUseDummyAudioReportsFalseByDefault) {
    rtcBool32 use_dummy = rtcBool32::kTrue;
    EXPECT_EQ(RTCPeerConnectionFactory_GetUseDummyAudio(factory_, &use_dummy),
              rtcResultU4::kSuccess);
    EXPECT_EQ(use_dummy, rtcBool32::kFalse);
}

TEST_F(RTCPeerConnectionFactoryTest, DummyAudioSourceRejectedWhenNotEnabled) {
    rtcAudioSourceHandle source = nullptr;
    EXPECT_EQ(RTCPeerConnectionFactory_CreateDummyAudioSource(
                  factory_, "dummy", 48000, 2, &source),
              rtcResultU4::kDummyAudioSourceNotSupported);
    EXPECT_EQ(source, nullptr);
}

}  // namespace libwebrtc
