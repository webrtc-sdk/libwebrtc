#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// --- Null-handle negatives ---

TEST(RTCDtmfSenderNegative, RegisterObserverWithNullHandleFails) {
    EXPECT_EQ(RTCDtmfSender_RegisterObserver(nullptr, nullptr),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDtmfSenderNegative, UnregisterObserverWithNullHandleFails) {
    EXPECT_EQ(RTCDtmfSender_UnregisterObserver(nullptr),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDtmfSenderNegative, InsertDtmfWithNullHandleFails) {
    EXPECT_EQ(RTCDtmfSender_InsertDtmf(nullptr, "1", 100, 70),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDtmfSenderNegative, CanInsertDtmfWithNullHandleFails) {
    rtcBool32 can = rtcBool32::kFalse;
    EXPECT_EQ(RTCDtmfSender_CanInsertDtmf(nullptr, &can),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDtmfSenderNegative, CanInsertDtmfWithNullOutPointerFails) {
    EXPECT_EQ(RTCDtmfSender_CanInsertDtmf(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCDtmfSenderNegative, GetTonesWithNullHandleFails) {
    char buf[64] = {};
    EXPECT_EQ(RTCDtmfSender_GetTones(nullptr, buf, sizeof(buf)),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDtmfSenderNegative, GetDurationWithNullHandleFails) {
    int duration = 0;
    EXPECT_EQ(RTCDtmfSender_GetDuration(nullptr, &duration),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDtmfSenderNegative, GetDurationWithNullOutPointerFails) {
    EXPECT_EQ(RTCDtmfSender_GetDuration(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCDtmfSenderNegative, GetInterToneGapWithNullHandleFails) {
    int gap = 0;
    EXPECT_EQ(RTCDtmfSender_GetInterToneGap(nullptr, &gap),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDtmfSenderNegative, GetCommaDelayWithNullHandleFails) {
    int delay = 0;
    EXPECT_EQ(RTCDtmfSender_GetCommaDelay(nullptr, &delay),
              rtcResultU4::kInvalidNativeHandle);
}

// Fixture that owns a fresh peer connection and obtains a DTMF sender from an
// audio RTP sender.
class RTCDtmfSenderTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_EQ(LibWebRTC_Initialize(), rtcBool32::kTrue);
        factory_ = LibWebRTC_CreateRTCPeerConnectionFactory();
        ASSERT_NE(factory_, nullptr);
        ASSERT_EQ(RTCPeerConnectionFactory_Initialize(factory_),
                  rtcBool32::kTrue);

        rtcPeerConnectionConfiguration config{};
        ASSERT_EQ(RTCPeerConnectionFactory_CreatePeerConnection(
                      factory_, &config, nullptr, &pc_),
                  rtcResultU4::kSuccess);
        ASSERT_NE(pc_, nullptr);
    }

    void TearDown() override {
        if (pc_) {
            RTCPeerConnection_Close(pc_);
            RTCPeerConnectionFactory_DeletePeerConnection(factory_, pc_);
            RefCountedObject_Release(pc_);
            pc_ = nullptr;
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
                      factory_, "test-source", &source),
                  rtcResultU4::kSuccess);
        if (!source) return nullptr;

        rtcAudioTrackHandle track = nullptr;
        EXPECT_EQ(RTCPeerConnectionFactory_CreateAudioTrack(
                      factory_, source, track_id, &track),
                  rtcResultU4::kSuccess);
        RefCountedObject_Release(source);
        return track;
    }

    // Returns an owned DTMF sender (caller releases) from a new audio sender.
    // sender_out/track_out receive owned handles the caller must release.
    rtcDtmfSenderHandle CreateDtmfSender(rtcRtpSenderHandle* sender_out,
                                         rtcAudioTrackHandle* track_out) {
        rtcAudioTrackHandle track = CreateAudioTrack("audio-track");
        if (!track) return nullptr;

        rtcRtpSenderHandle sender = nullptr;
        EXPECT_EQ(RTCPeerConnection_AddTrack(pc_, track, "stream-id", &sender),
                  rtcResultU4::kSuccess);
        if (!sender) {
            RefCountedObject_Release(track);
            return nullptr;
        }

        rtcDtmfSenderHandle dtmf = nullptr;
        EXPECT_EQ(RTCRtpSender_GetDtmfSender(sender, &dtmf),
                  rtcResultU4::kSuccess);

        *sender_out = sender;
        *track_out = track;
        return dtmf;
    }

    rtcPeerConnectionFactoryHandle factory_ = nullptr;
    rtcPeerConnectionHandle pc_ = nullptr;
};

TEST_F(RTCDtmfSenderTest, GetDtmfSenderFromAudioSenderReturnsHandle) {
    rtcRtpSenderHandle sender = nullptr;
    rtcAudioTrackHandle track = nullptr;
    rtcDtmfSenderHandle dtmf = CreateDtmfSender(&sender, &track);
    ASSERT_NE(dtmf, nullptr);

    rtcBool32 can = rtcBool32::kFalse;
    EXPECT_EQ(RTCDtmfSender_CanInsertDtmf(dtmf, &can), rtcResultU4::kSuccess);

    int duration = 0;
    EXPECT_EQ(RTCDtmfSender_GetDuration(dtmf, &duration),
              rtcResultU4::kSuccess);

    int gap = 0;
    EXPECT_EQ(RTCDtmfSender_GetInterToneGap(dtmf, &gap),
              rtcResultU4::kSuccess);

    int comma_delay = 0;
    EXPECT_EQ(RTCDtmfSender_GetCommaDelay(dtmf, &comma_delay),
              rtcResultU4::kSuccess);

    char tones[256] = {};
    EXPECT_EQ(RTCDtmfSender_GetTones(dtmf, tones, sizeof(tones)),
              rtcResultU4::kSuccess);

    if (dtmf) RefCountedObject_Release(dtmf);
    if (sender) RefCountedObject_Release(sender);
    if (track) RefCountedObject_Release(track);
}

TEST_F(RTCDtmfSenderTest, GetTonesWithNullBufferFails) {
    rtcRtpSenderHandle sender = nullptr;
    rtcAudioTrackHandle track = nullptr;
    rtcDtmfSenderHandle dtmf = CreateDtmfSender(&sender, &track);
    ASSERT_NE(dtmf, nullptr);

    // Valid handle, null output buffer -> the buffer pointer is rejected.
    EXPECT_EQ(RTCDtmfSender_GetTones(dtmf, nullptr, 64),
              rtcResultU4::kInvalidPointer);

    if (dtmf) RefCountedObject_Release(dtmf);
    if (sender) RefCountedObject_Release(sender);
    if (track) RefCountedObject_Release(track);
}

TEST_F(RTCDtmfSenderTest, InsertDtmfReturnsSuccess) {
    rtcRtpSenderHandle sender = nullptr;
    rtcAudioTrackHandle track = nullptr;
    rtcDtmfSenderHandle dtmf = CreateDtmfSender(&sender, &track);
    ASSERT_NE(dtmf, nullptr);

    EXPECT_EQ(RTCDtmfSender_InsertDtmf(dtmf, "1", 100, 70),
              rtcResultU4::kSuccess);

    int comma_delay = 50;
    EXPECT_EQ(RTCDtmfSender_InsertDtmf(dtmf, "2", 100, 70, &comma_delay),
              rtcResultU4::kSuccess);

    if (dtmf) RefCountedObject_Release(dtmf);
    if (sender) RefCountedObject_Release(sender);
    if (track) RefCountedObject_Release(track);
}

TEST_F(RTCDtmfSenderTest, RegisterAndUnregisterObserver) {
    rtcRtpSenderHandle sender = nullptr;
    rtcAudioTrackHandle track = nullptr;
    rtcDtmfSenderHandle dtmf = CreateDtmfSender(&sender, &track);
    ASSERT_NE(dtmf, nullptr);

    rtcDtmfSenderObserverCallbacks callbacks{};
    EXPECT_EQ(RTCDtmfSender_RegisterObserver(dtmf, &callbacks),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCDtmfSender_UnregisterObserver(dtmf), rtcResultU4::kSuccess);

    if (dtmf) RefCountedObject_Release(dtmf);
    if (sender) RefCountedObject_Release(sender);
    if (track) RefCountedObject_Release(track);
}

TEST_F(RTCDtmfSenderTest, RegisterObserverWithNullCallbacksFails) {
    rtcRtpSenderHandle sender = nullptr;
    rtcAudioTrackHandle track = nullptr;
    rtcDtmfSenderHandle dtmf = CreateDtmfSender(&sender, &track);
    ASSERT_NE(dtmf, nullptr);

    EXPECT_EQ(RTCDtmfSender_RegisterObserver(dtmf, nullptr),
              rtcResultU4::kInvalidParameter);

    if (dtmf) RefCountedObject_Release(dtmf);
    if (sender) RefCountedObject_Release(sender);
    if (track) RefCountedObject_Release(track);
}

}  // namespace libwebrtc
