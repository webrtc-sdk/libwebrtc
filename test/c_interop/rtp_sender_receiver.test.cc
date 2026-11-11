#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// --- RTCRtpSender null-handle negatives ---

TEST(RTCRtpSenderNegative, GetTrackWithNullHandleFails) {
    rtcMediaTrackHandle track = nullptr;
    EXPECT_EQ(RTCRtpSender_GetTrack(nullptr, &track),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpSenderNegative, GetMediaTypeWithNullHandleFails) {
    rtcMediaType type{};
    EXPECT_EQ(RTCRtpSender_GetMediaType(nullptr, &type),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpSenderNegative, GetIdWithNullHandleFails) {
    char buf[64] = {};
    EXPECT_EQ(RTCRtpSender_GetId(nullptr, buf, sizeof(buf)),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpSenderNegative, GetSSRCWithNullHandleFails) {
    unsigned int ssrc = 0;
    EXPECT_EQ(RTCRtpSender_GetSSRC(nullptr, &ssrc),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpSenderNegative, GetDtmfSenderWithNullHandleFails) {
    rtcDtmfSenderHandle dtmf = nullptr;
    EXPECT_EQ(RTCRtpSender_GetDtmfSender(nullptr, &dtmf),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpSenderNegative, ListGetCountWithNullHandleReturnsZero) {
    EXPECT_EQ(RTCRtpSenderList_GetCount(nullptr), 0);
}

TEST(RTCRtpSenderNegative, ListGetItemWithNullHandleFails) {
    rtcRtpSenderHandle item = nullptr;
    EXPECT_EQ(RTCRtpSenderList_GetItem(nullptr, 0, &item),
              rtcResultU4::kInvalidNativeHandle);
}

// --- RTCRtpReceiver null-handle negatives ---

TEST(RTCRtpReceiverNegative, GetTrackWithNullHandleFails) {
    rtcMediaTrackHandle track = nullptr;
    EXPECT_EQ(RTCRtpReceiver_GetTrack(nullptr, &track),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpReceiverNegative, GetMediaTypeWithNullHandleFails) {
    rtcMediaType type{};
    EXPECT_EQ(RTCRtpReceiver_GetMediaType(nullptr, &type),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpReceiverNegative, GetIdWithNullHandleFails) {
    char buf[64] = {};
    EXPECT_EQ(RTCRtpReceiver_GetId(nullptr, buf, sizeof(buf)),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpReceiverNegative, RegisterObserverWithNullCallbacksFails) {
    EXPECT_NE(RTCRtpReceiver_RegisterObserver(nullptr, nullptr),
              rtcResultU4::kSuccess);
}

TEST(RTCRtpReceiverNegative, ListGetCountWithNullHandleReturnsZero) {
    EXPECT_EQ(RTCRtpReceiverList_GetCount(nullptr), 0);
}

TEST(RTCRtpReceiverNegative, ListGetItemWithNullHandleFails) {
    rtcRtpReceiverHandle item = nullptr;
    EXPECT_EQ(RTCRtpReceiverList_GetItem(nullptr, 0, &item),
              rtcResultU4::kInvalidNativeHandle);
}

// Fixture that owns a fresh peer connection for each test.
class RTCRtpSenderReceiverTest : public ::testing::Test {
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

    // Creates and owns an audio track so callers can exercise track APIs.
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

    rtcRtpTransceiverHandle AddAudioTransceiver() {
        rtcRtpTransceiverHandle transceiver = nullptr;
        EXPECT_EQ(RTCPeerConnection_AddTransceiver1(
                      pc_, RTCMediaType::AUDIO, &transceiver),
                  rtcResultU4::kSuccess);
        return transceiver;
    }

    rtcPeerConnectionFactoryHandle factory_ = nullptr;
    rtcPeerConnectionHandle pc_ = nullptr;
};

// --- Sender obtained via AddTrack ---

TEST_F(RTCRtpSenderReceiverTest, SenderGetTrackReturnsTrack) {
    rtcAudioTrackHandle track = CreateAudioTrack("audio-track");
    ASSERT_NE(track, nullptr);

    rtcRtpSenderHandle sender = nullptr;
    ASSERT_EQ(RTCPeerConnection_AddTrack(pc_, track, "stream-id", &sender),
              rtcResultU4::kSuccess);
    ASSERT_NE(sender, nullptr);

    rtcMediaTrackHandle sender_track = nullptr;
    EXPECT_EQ(RTCRtpSender_GetTrack(sender, &sender_track),
              rtcResultU4::kSuccess);
    EXPECT_NE(sender_track, nullptr);
    if (sender_track) RefCountedObject_Release(sender_track);

    RefCountedObject_Release(sender);
    RefCountedObject_Release(track);
}

TEST_F(RTCRtpSenderReceiverTest, SenderGettersReturnSuccess) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    rtcRtpSenderHandle sender = nullptr;
    ASSERT_EQ(RTCRtpTransceiver_GetSender(transceiver, &sender),
              rtcResultU4::kSuccess);
    ASSERT_NE(sender, nullptr);

    rtcMediaType type{};
    EXPECT_EQ(RTCRtpSender_GetMediaType(sender, &type), rtcResultU4::kSuccess);
    EXPECT_EQ(type, RTCMediaType::AUDIO);

    char id[256] = {};
    EXPECT_EQ(RTCRtpSender_GetId(sender, id, sizeof(id)),
              rtcResultU4::kSuccess);

    unsigned int ssrc = 0;
    EXPECT_EQ(RTCRtpSender_GetSSRC(sender, &ssrc), rtcResultU4::kSuccess);

    char stream_ids[256] = {};
    EXPECT_EQ(RTCRtpSender_GetStreamIds(sender, stream_ids, sizeof(stream_ids)),
              rtcResultU4::kSuccess);

    RefCountedObject_Release(sender);
    RefCountedObject_Release(transceiver);
}

TEST_F(RTCRtpSenderReceiverTest, SenderSetStreamIds) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    rtcRtpSenderHandle sender = nullptr;
    ASSERT_EQ(RTCRtpTransceiver_GetSender(transceiver, &sender),
              rtcResultU4::kSuccess);
    ASSERT_NE(sender, nullptr);

    EXPECT_EQ(RTCRtpSender_SetStreamIds(sender, "s1"), rtcResultU4::kSuccess);

    RefCountedObject_Release(sender);
    RefCountedObject_Release(transceiver);
}

TEST_F(RTCRtpSenderReceiverTest, SenderGetParametersReturnsHandle) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    rtcRtpSenderHandle sender = nullptr;
    ASSERT_EQ(RTCRtpTransceiver_GetSender(transceiver, &sender),
              rtcResultU4::kSuccess);
    ASSERT_NE(sender, nullptr);

    rtcRtpParametersHandle params = nullptr;
    EXPECT_EQ(RTCRtpSender_GetParameters(sender, &params),
              rtcResultU4::kSuccess);
    EXPECT_NE(params, nullptr);

    if (params) {
        EXPECT_EQ(RTCRtpSender_SetParameters(sender, params),
                  rtcResultU4::kSuccess);
        RefCountedObject_Release(params);
    }

    RefCountedObject_Release(sender);
    RefCountedObject_Release(transceiver);
}

TEST_F(RTCRtpSenderReceiverTest, SenderGetInitEncodingsReturnsHandle) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    rtcRtpSenderHandle sender = nullptr;
    ASSERT_EQ(RTCRtpTransceiver_GetSender(transceiver, &sender),
              rtcResultU4::kSuccess);
    ASSERT_NE(sender, nullptr);

    rtcRtpEncodingParametersListHandle encodings = nullptr;
    EXPECT_EQ(RTCRtpSender_GetInitEncodings(sender, &encodings),
              rtcResultU4::kSuccess);
    EXPECT_NE(encodings, nullptr);
    if (encodings) RefCountedObject_Release(encodings);

    RefCountedObject_Release(sender);
    RefCountedObject_Release(transceiver);
}

// --- Receiver obtained via transceiver ---

TEST_F(RTCRtpSenderReceiverTest, ReceiverGettersReturnSuccess) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    rtcRtpReceiverHandle receiver = nullptr;
    ASSERT_EQ(RTCRtpTransceiver_GetReceiver(transceiver, &receiver),
              rtcResultU4::kSuccess);
    ASSERT_NE(receiver, nullptr);

    rtcMediaType type{};
    EXPECT_EQ(RTCRtpReceiver_GetMediaType(receiver, &type),
              rtcResultU4::kSuccess);
    EXPECT_EQ(type, RTCMediaType::AUDIO);

    char id[256] = {};
    EXPECT_EQ(RTCRtpReceiver_GetId(receiver, id, sizeof(id)),
              rtcResultU4::kSuccess);

    rtcMediaTrackHandle track = nullptr;
    EXPECT_EQ(RTCRtpReceiver_GetTrack(receiver, &track),
              rtcResultU4::kSuccess);
    EXPECT_NE(track, nullptr);
    if (track) RefCountedObject_Release(track);

    rtcRtpParametersHandle params = nullptr;
    EXPECT_EQ(RTCRtpReceiver_GetParameters(receiver, &params),
              rtcResultU4::kSuccess);
    EXPECT_NE(params, nullptr);
    if (params) RefCountedObject_Release(params);

    rtcMediaStreamListHandle streams = nullptr;
    EXPECT_EQ(RTCRtpReceiver_GetStreams(receiver, &streams),
              rtcResultU4::kSuccess);
    EXPECT_NE(streams, nullptr);
    if (streams) RefCountedObject_Release(streams);

    RefCountedObject_Release(receiver);
    RefCountedObject_Release(transceiver);
}

TEST_F(RTCRtpSenderReceiverTest, ReceiverRegisterAndUnregisterObserver) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    rtcRtpReceiverHandle receiver = nullptr;
    ASSERT_EQ(RTCRtpTransceiver_GetReceiver(transceiver, &receiver),
              rtcResultU4::kSuccess);
    ASSERT_NE(receiver, nullptr);

    rtcRtpReceiverObserverCallbacks callbacks{};
    EXPECT_EQ(RTCRtpReceiver_RegisterObserver(receiver, &callbacks),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCRtpReceiver_UnregisterObserver(receiver),
              rtcResultU4::kSuccess);

    RefCountedObject_Release(receiver);
    RefCountedObject_Release(transceiver);
}

// --- List getters via GetSenders / GetReceivers ---

TEST_F(RTCRtpSenderReceiverTest, GetSendersListCountAndItem) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);
    RefCountedObject_Release(transceiver);

    rtcRtpSenderListHandle list = nullptr;
    EXPECT_EQ(RTCPeerConnection_GetSenders(pc_, &list), rtcResultU4::kSuccess);
    ASSERT_NE(list, nullptr);

    int count = RTCRtpSenderList_GetCount(list);
    EXPECT_GE(count, 1);

    rtcRtpSenderHandle item = nullptr;
    EXPECT_EQ(RTCRtpSenderList_GetItem(list, 0, &item), rtcResultU4::kSuccess);
    EXPECT_NE(item, nullptr);
    if (item) RefCountedObject_Release(item);

    rtcRtpSenderHandle oob = nullptr;
    EXPECT_EQ(RTCRtpSenderList_GetItem(list, count, &oob),
              rtcResultU4::kOutOfRange);
    EXPECT_EQ(oob, nullptr);

    RefCountedObject_Release(list);
}

TEST_F(RTCRtpSenderReceiverTest, GetReceiversListCountAndItem) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);
    RefCountedObject_Release(transceiver);

    rtcRtpReceiverListHandle list = nullptr;
    EXPECT_EQ(RTCPeerConnection_GetReceivers(pc_, &list),
              rtcResultU4::kSuccess);
    ASSERT_NE(list, nullptr);

    int count = RTCRtpReceiverList_GetCount(list);
    EXPECT_GE(count, 1);

    rtcRtpReceiverHandle item = nullptr;
    EXPECT_EQ(RTCRtpReceiverList_GetItem(list, 0, &item),
              rtcResultU4::kSuccess);
    EXPECT_NE(item, nullptr);
    if (item) RefCountedObject_Release(item);

    rtcRtpReceiverHandle oob = nullptr;
    EXPECT_EQ(RTCRtpReceiverList_GetItem(list, count, &oob),
              rtcResultU4::kOutOfRange);
    EXPECT_EQ(oob, nullptr);

    rtcRtpReceiverHandle neg = nullptr;
    EXPECT_EQ(RTCRtpReceiverList_GetItem(list, -1, &neg),
              rtcResultU4::kOutOfRange);

    RefCountedObject_Release(list);
}

}  // namespace libwebrtc
