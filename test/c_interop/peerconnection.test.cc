#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {
namespace {

// A simple latch usable from any thread to wait for an SDP/stats callback.
class CallbackLatch {
public:
    void Signal() {
        std::lock_guard<std::mutex> lock(mu_);
        signaled_ = true;
        cv_.notify_all();
    }
    bool WaitFor(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mu_);
        return cv_.wait_for(lock, timeout, [&] { return signaled_; });
    }

private:
    std::mutex mu_;
    std::condition_variable cv_;
    bool signaled_ = false;
};

struct OfferAnswerResult {
    CallbackLatch latch;
    std::string sdp;
    std::string type;
    std::string error;
    bool succeeded = false;
};

void LIB_WEBRTC_CALL OnGetSdpSuccess(rtcObjectHandle user_data,
                                     const char* sdp,
                                     const char* type) {
    auto* result = static_cast<OfferAnswerResult*>(user_data);
    if (sdp) result->sdp = sdp;
    if (type) result->type = type;
    result->succeeded = true;
    result->latch.Signal();
}

void LIB_WEBRTC_CALL OnSdpFailure(rtcObjectHandle user_data,
                                  const char* error) {
    auto* result = static_cast<OfferAnswerResult*>(user_data);
    if (error) result->error = error;
    result->succeeded = false;
    result->latch.Signal();
}

void LIB_WEBRTC_CALL OnSetSdpSuccess(rtcObjectHandle user_data) {
    auto* result = static_cast<OfferAnswerResult*>(user_data);
    result->succeeded = true;
    result->latch.Signal();
}

void LIB_WEBRTC_CALL OnStatsSuccess(rtcObjectHandle user_data,
                                    rtcMediaRTCStatsListHandle reports) {
    auto* latch = static_cast<CallbackLatch*>(user_data);
    if (reports) RefCountedObject_Release(reports);
    latch->Signal();
}

}  // namespace

// Negative-path tests that don't need a peer connection.

TEST(RTCPeerConnectionNegative, IsInitializedWithNullHandleFails) {
    rtcBool32 init = rtcBool32::kTrue;
    EXPECT_NE(RTCPeerConnection_IsInitialized(nullptr, &init),
              rtcResultU4::kSuccess);
}

TEST(RTCPeerConnectionNegative, AddStreamWithNullHandleFails) {
    int dummy = 0;
    EXPECT_NE(RTCPeerConnection_AddStream(nullptr, nullptr, &dummy),
              rtcResultU4::kSuccess);
}

TEST(RTCPeerConnectionNegative, GetSignalingStateWithNullHandleFails) {
    rtcSignalingState state{};
    EXPECT_NE(RTCPeerConnection_GetSignalingState(nullptr, &state),
              rtcResultU4::kSuccess);
}

// Fixture that owns a fresh peer connection for each test.
class RTCPeerConnectionTest : public ::testing::Test {
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

    rtcPeerConnectionFactoryHandle factory_ = nullptr;
    rtcPeerConnectionHandle pc_ = nullptr;
};

// --- State queries ---

TEST_F(RTCPeerConnectionTest, IsInitializedReturnsTrue) {
    rtcBool32 initialized = rtcBool32::kFalse;
    EXPECT_EQ(RTCPeerConnection_IsInitialized(pc_, &initialized),
              rtcResultU4::kSuccess);
    EXPECT_EQ(initialized, rtcBool32::kTrue);
}

TEST_F(RTCPeerConnectionTest, InitialSignalingStateIsStable) {
    rtcSignalingState state{};
    EXPECT_EQ(RTCPeerConnection_GetSignalingState(pc_, &state),
              rtcResultU4::kSuccess);
    EXPECT_EQ(state, rtcSignalingState::kStable);
}

TEST_F(RTCPeerConnectionTest, InitialIceConnectionStateIsNew) {
    rtcIceConnectionState state{};
    EXPECT_EQ(RTCPeerConnection_GetIceConnectionState(pc_, &state),
              rtcResultU4::kSuccess);
    EXPECT_EQ(state, rtcIceConnectionState::kNew);
}

TEST_F(RTCPeerConnectionTest, InitialStandardizedIceConnectionStateIsNew) {
    rtcIceConnectionState state{};
    EXPECT_EQ(
        RTCPeerConnection_GetStandardizedIceConnectionState(pc_, &state),
        rtcResultU4::kSuccess);
    EXPECT_EQ(state, rtcIceConnectionState::kNew);
}

TEST_F(RTCPeerConnectionTest, InitialPeerConnectionStateIsNew) {
    rtcPeerConnectionState state{};
    EXPECT_EQ(RTCPeerConnection_GetPeerConnectionState(pc_, &state),
              rtcResultU4::kSuccess);
    EXPECT_EQ(state, rtcPeerConnectionState::kNew);
}

TEST_F(RTCPeerConnectionTest, InitialIceGatheringStateIsNew) {
    rtcIceGatheringState state{};
    EXPECT_EQ(RTCPeerConnection_GetIceGatheringState(pc_, &state),
              rtcResultU4::kSuccess);
    EXPECT_EQ(state, rtcIceGatheringState::kNew);
}

// --- Stream / data channel ---

TEST_F(RTCPeerConnectionTest, CreateLocalMediaStreamReturnsHandle) {
    rtcMediaStreamHandle stream = nullptr;
    EXPECT_EQ(RTCPeerConnection_CreateLocalMediaStream(pc_, "stream-1", &stream),
              rtcResultU4::kSuccess);
    EXPECT_NE(stream, nullptr);
    if (stream) RefCountedObject_Release(stream);
}

TEST_F(RTCPeerConnectionTest, CreateDataChannelDefault) {
    rtcDataChannelHandle dc = nullptr;
    EXPECT_EQ(RTCPeerConnection_CreateDataChannel(pc_, "data", nullptr, &dc),
              rtcResultU4::kSuccess);
    EXPECT_NE(dc, nullptr);
    if (dc) RefCountedObject_Release(dc);
}

TEST_F(RTCPeerConnectionTest, CreateDataChannelWithInit) {
    rtcDataChannelInit init{};
    init.ordered = rtcBool32::kTrue;
    init.maxRetransmits = 3;

    rtcDataChannelHandle dc = nullptr;
    EXPECT_EQ(RTCPeerConnection_CreateDataChannel(pc_, "data", &init, &dc),
              rtcResultU4::kSuccess);
    EXPECT_NE(dc, nullptr);
    if (dc) RefCountedObject_Release(dc);
}

TEST_F(RTCPeerConnectionTest, AddStreamWithNullStreamFails) {
    int retval = 0;
    EXPECT_NE(RTCPeerConnection_AddStream(pc_, nullptr, &retval),
              rtcResultU4::kSuccess);
}

TEST_F(RTCPeerConnectionTest, RemoveStreamWithNullStreamFails) {
    int retval = 0;
    EXPECT_NE(RTCPeerConnection_RemoveStream(pc_, nullptr, &retval),
              rtcResultU4::kSuccess);
}

// --- Transceivers / tracks ---

TEST_F(RTCPeerConnectionTest, AddTransceiver1Audio) {
    rtcRtpTransceiverHandle transceiver = nullptr;
    EXPECT_EQ(RTCPeerConnection_AddTransceiver1(
                  pc_, RTCMediaType::AUDIO, &transceiver),
              rtcResultU4::kSuccess);
    EXPECT_NE(transceiver, nullptr);
    if (transceiver) RefCountedObject_Release(transceiver);
}

TEST_F(RTCPeerConnectionTest, AddTransceiver1Video) {
    rtcRtpTransceiverHandle transceiver = nullptr;
    EXPECT_EQ(RTCPeerConnection_AddTransceiver1(
                  pc_, RTCMediaType::VIDEO, &transceiver),
              rtcResultU4::kSuccess);
    EXPECT_NE(transceiver, nullptr);
    if (transceiver) RefCountedObject_Release(transceiver);
}

TEST_F(RTCPeerConnectionTest, AddTransceiver2WithNullTrackFails) {
    rtcRtpTransceiverHandle transceiver = nullptr;
    EXPECT_NE(RTCPeerConnection_AddTransceiver2(pc_, nullptr, &transceiver),
              rtcResultU4::kSuccess);
    EXPECT_EQ(transceiver, nullptr);
}

TEST_F(RTCPeerConnectionTest, AddTransceiver3WithNullParamsFails) {
    rtcRtpTransceiverHandle transceiver = nullptr;
    EXPECT_NE(
        RTCPeerConnection_AddTransceiver3(pc_, nullptr, nullptr, &transceiver),
        rtcResultU4::kSuccess);
    EXPECT_EQ(transceiver, nullptr);
}

TEST_F(RTCPeerConnectionTest, AddTrackWithNullTrackFails) {
    rtcRtpSenderHandle sender = nullptr;
    EXPECT_NE(RTCPeerConnection_AddTrack(pc_, nullptr, "", &sender),
              rtcResultU4::kSuccess);
    EXPECT_EQ(sender, nullptr);
}

TEST_F(RTCPeerConnectionTest, AddTrackAudioReturnsSender) {
    rtcAudioTrackHandle track = CreateAudioTrack("audio-track");
    ASSERT_NE(track, nullptr);

    rtcRtpSenderHandle sender = nullptr;
    EXPECT_EQ(RTCPeerConnection_AddTrack(pc_, track, "stream-id", &sender),
              rtcResultU4::kSuccess);
    EXPECT_NE(sender, nullptr);

    if (sender) RefCountedObject_Release(sender);
    RefCountedObject_Release(track);
}

TEST_F(RTCPeerConnectionTest, RemoveTrackWithNullSenderFails) {
    rtcBool32 retval = rtcBool32::kTrue;
    EXPECT_NE(RTCPeerConnection_RemoveTrack(pc_, nullptr, &retval),
              rtcResultU4::kSuccess);
    EXPECT_EQ(retval, rtcBool32::kFalse);
}

// --- List getters ---

TEST_F(RTCPeerConnectionTest, GetSendersReturnsHandle) {
    rtcRtpSenderListHandle list = nullptr;
    EXPECT_EQ(RTCPeerConnection_GetSenders(pc_, &list), rtcResultU4::kSuccess);
    EXPECT_NE(list, nullptr);
    if (list) RefCountedObject_Release(list);
}

TEST_F(RTCPeerConnectionTest, GetReceiversReturnsHandle) {
    rtcRtpReceiverListHandle list = nullptr;
    EXPECT_EQ(RTCPeerConnection_GetReceivers(pc_, &list),
              rtcResultU4::kSuccess);
    EXPECT_NE(list, nullptr);
    if (list) RefCountedObject_Release(list);
}

TEST_F(RTCPeerConnectionTest, GetTransceiversReturnsHandle) {
    rtcRtpTransceiverListHandle list = nullptr;
    EXPECT_EQ(RTCPeerConnection_GetTransceivers(pc_, &list),
              rtcResultU4::kSuccess);
    EXPECT_NE(list, nullptr);
    if (list) RefCountedObject_Release(list);
}

TEST_F(RTCPeerConnectionTest, GetLocalStreamsReturnsHandle) {
    rtcMediaStreamListHandle list = nullptr;
    EXPECT_EQ(RTCPeerConnection_GetLocalStreams(pc_, &list),
              rtcResultU4::kSuccess);
    EXPECT_NE(list, nullptr);
    if (list) RefCountedObject_Release(list);
}

TEST_F(RTCPeerConnectionTest, GetRemoteStreamsReturnsHandle) {
    rtcMediaStreamListHandle list = nullptr;
    EXPECT_EQ(RTCPeerConnection_GetRemoteStreams(pc_, &list),
              rtcResultU4::kSuccess);
    EXPECT_NE(list, nullptr);
    if (list) RefCountedObject_Release(list);
}

// --- SDP / stats negative paths ---

TEST_F(RTCPeerConnectionTest, CreateOfferRejectsNullCallbacksAndConstraints) {
    rtcMediaConstraintsHandle constraints = MediaConstraints_Create();
    ASSERT_NE(constraints, nullptr);

    EXPECT_EQ(RTCPeerConnection_CreateOffer(pc_, nullptr, nullptr,
                                            &OnSdpFailure, constraints),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(RTCPeerConnection_CreateOffer(pc_, nullptr, &OnGetSdpSuccess,
                                            nullptr, constraints),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(RTCPeerConnection_CreateOffer(pc_, nullptr, &OnGetSdpSuccess,
                                            &OnSdpFailure, nullptr),
              rtcResultU4::kInvalidParameter);

    RefCountedObject_Release(constraints);
}

TEST_F(RTCPeerConnectionTest, CreateAnswerRejectsNullCallbacksAndConstraints) {
    rtcMediaConstraintsHandle constraints = MediaConstraints_Create();
    ASSERT_NE(constraints, nullptr);

    EXPECT_EQ(RTCPeerConnection_CreateAnswer(pc_, nullptr, nullptr,
                                             &OnSdpFailure, constraints),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(RTCPeerConnection_CreateAnswer(pc_, nullptr, &OnGetSdpSuccess,
                                             nullptr, constraints),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(RTCPeerConnection_CreateAnswer(pc_, nullptr, &OnGetSdpSuccess,
                                             &OnSdpFailure, nullptr),
              rtcResultU4::kInvalidParameter);

    RefCountedObject_Release(constraints);
}

TEST_F(RTCPeerConnectionTest, SetLocalDescriptionRejectsNullCallbacks) {
    EXPECT_EQ(RTCPeerConnection_SetLocalDescription(pc_, "", "offer", nullptr,
                                                    nullptr, &OnSdpFailure),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(RTCPeerConnection_SetLocalDescription(
                  pc_, "", "offer", nullptr, &OnSetSdpSuccess, nullptr),
              rtcResultU4::kInvalidParameter);
}

TEST_F(RTCPeerConnectionTest, SetRemoteDescriptionRejectsNullCallbacks) {
    EXPECT_EQ(RTCPeerConnection_SetRemoteDescription(pc_, "", "offer", nullptr,
                                                     nullptr, &OnSdpFailure),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(RTCPeerConnection_SetRemoteDescription(
                  pc_, "", "offer", nullptr, &OnSetSdpSuccess, nullptr),
              rtcResultU4::kInvalidParameter);
}

TEST_F(RTCPeerConnectionTest, GetLocalDescriptionRejectsNullCallbacks) {
    EXPECT_EQ(RTCPeerConnection_GetLocalDescription(pc_, nullptr, nullptr,
                                                    &OnSdpFailure),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(RTCPeerConnection_GetLocalDescription(pc_, nullptr,
                                                    &OnGetSdpSuccess, nullptr),
              rtcResultU4::kInvalidParameter);
}

TEST_F(RTCPeerConnectionTest, GetRemoteDescriptionRejectsNullCallbacks) {
    EXPECT_EQ(RTCPeerConnection_GetRemoteDescription(pc_, nullptr, nullptr,
                                                     &OnSdpFailure),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(RTCPeerConnection_GetRemoteDescription(pc_, nullptr,
                                                     &OnGetSdpSuccess, nullptr),
              rtcResultU4::kInvalidParameter);
}

TEST_F(RTCPeerConnectionTest, GetStatsRejectsNullCallbacks) {
    EXPECT_EQ(RTCPeerConnection_GetStats(pc_, nullptr, nullptr, &OnSdpFailure),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(RTCPeerConnection_GetStats(pc_, nullptr, &OnStatsSuccess, nullptr),
              rtcResultU4::kInvalidParameter);
}

TEST_F(RTCPeerConnectionTest, GetSenderStatsRejectsNullSender) {
    rtcBool32 retval = rtcBool32::kTrue;
    EXPECT_EQ(RTCPeerConnection_GetSenderStats(pc_, nullptr, nullptr,
                                               &OnStatsSuccess, &OnSdpFailure,
                                               &retval),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(retval, rtcBool32::kFalse);
}

TEST_F(RTCPeerConnectionTest, GetReceiverStatsRejectsNullReceiver) {
    rtcBool32 retval = rtcBool32::kTrue;
    EXPECT_EQ(RTCPeerConnection_GetReceiverStats(pc_, nullptr, nullptr,
                                                 &OnStatsSuccess, &OnSdpFailure,
                                                 &retval),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(retval, rtcBool32::kFalse);
}

// --- Misc ---

TEST_F(RTCPeerConnectionTest, RestartIceSucceeds) {
    EXPECT_EQ(RTCPeerConnection_RestartIce(pc_), rtcResultU4::kSuccess);
}

TEST_F(RTCPeerConnectionTest, AddCandidateAcceptsParameters) {
    EXPECT_EQ(RTCPeerConnection_AddCandidate(pc_, "0", 0, ""),
              rtcResultU4::kSuccess);
}

TEST_F(RTCPeerConnectionTest, RegisterAndUnregisterObserver) {
    rtcPeerConnectionObserverCallbacks callbacks{};
    EXPECT_EQ(RTCPeerConnection_RegisterObserver(pc_, &callbacks),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCPeerConnection_UnregisterObserver(pc_),
              rtcResultU4::kSuccess);
}

TEST_F(RTCPeerConnectionTest, RegisterObserverWithNullCallbacksFails) {
    EXPECT_NE(RTCPeerConnection_RegisterObserver(pc_, nullptr),
              rtcResultU4::kSuccess);
}

// --- Async round trip: CreateOffer + SetLocalDescription ---

TEST_F(RTCPeerConnectionTest, CreateOfferProducesSdp) {
    // Add a transceiver so the offer has at least one m-line.
    rtcRtpTransceiverHandle transceiver = nullptr;
    ASSERT_EQ(RTCPeerConnection_AddTransceiver1(
                  pc_, RTCMediaType::AUDIO, &transceiver),
              rtcResultU4::kSuccess);
    if (transceiver) RefCountedObject_Release(transceiver);

    rtcMediaConstraintsHandle constraints = MediaConstraints_Create();
    ASSERT_NE(constraints, nullptr);

    OfferAnswerResult offer;
    ASSERT_EQ(RTCPeerConnection_CreateOffer(pc_, &offer, &OnGetSdpSuccess,
                                            &OnSdpFailure, constraints),
              rtcResultU4::kSuccess);

    ASSERT_TRUE(offer.latch.WaitFor(std::chrono::seconds(5)))
        << "CreateOffer callback was not invoked";
    EXPECT_TRUE(offer.succeeded) << "CreateOffer failed: " << offer.error;
    EXPECT_FALSE(offer.sdp.empty());
    EXPECT_EQ(offer.type, "offer");

    RefCountedObject_Release(constraints);
}

TEST_F(RTCPeerConnectionTest, SetLocalDescriptionAfterCreateOffer) {
    rtcRtpTransceiverHandle transceiver = nullptr;
    ASSERT_EQ(RTCPeerConnection_AddTransceiver1(
                  pc_, RTCMediaType::AUDIO, &transceiver),
              rtcResultU4::kSuccess);
    if (transceiver) RefCountedObject_Release(transceiver);

    rtcMediaConstraintsHandle constraints = MediaConstraints_Create();
    ASSERT_NE(constraints, nullptr);

    OfferAnswerResult offer;
    ASSERT_EQ(RTCPeerConnection_CreateOffer(pc_, &offer, &OnGetSdpSuccess,
                                            &OnSdpFailure, constraints),
              rtcResultU4::kSuccess);
    ASSERT_TRUE(offer.latch.WaitFor(std::chrono::seconds(5)));
    ASSERT_TRUE(offer.succeeded);

    OfferAnswerResult set;
    ASSERT_EQ(RTCPeerConnection_SetLocalDescription(
                  pc_, offer.sdp.c_str(), offer.type.c_str(), &set,
                  &OnSetSdpSuccess, &OnSdpFailure),
              rtcResultU4::kSuccess);
    ASSERT_TRUE(set.latch.WaitFor(std::chrono::seconds(5)));
    EXPECT_TRUE(set.succeeded) << "SetLocalDescription failed: " << set.error;

    rtcSignalingState state{};
    EXPECT_EQ(RTCPeerConnection_GetSignalingState(pc_, &state),
              rtcResultU4::kSuccess);
    EXPECT_EQ(state, rtcSignalingState::kHaveLocalOffer);

    RefCountedObject_Release(constraints);
}

TEST_F(RTCPeerConnectionTest, StateGettersReturnClosedAfterClose) {
    EXPECT_EQ(RTCPeerConnection_Close(pc_), rtcResultU4::kSuccess);

    rtcSignalingState signaling{};
    EXPECT_EQ(RTCPeerConnection_GetSignalingState(pc_, &signaling),
              rtcResultU4::kPeerConnectionClosed);

    rtcIceConnectionState ice{};
    EXPECT_EQ(RTCPeerConnection_GetIceConnectionState(pc_, &ice),
              rtcResultU4::kPeerConnectionClosed);
    EXPECT_EQ(
        RTCPeerConnection_GetStandardizedIceConnectionState(pc_, &ice),
        rtcResultU4::kPeerConnectionClosed);

    rtcPeerConnectionState pc_state{};
    EXPECT_EQ(RTCPeerConnection_GetPeerConnectionState(pc_, &pc_state),
              rtcResultU4::kPeerConnectionClosed);

    rtcIceGatheringState gathering{};
    EXPECT_EQ(RTCPeerConnection_GetIceGatheringState(pc_, &gathering),
              rtcResultU4::kPeerConnectionClosed);

    rtcBool32 initialized = rtcBool32::kTrue;
    EXPECT_EQ(RTCPeerConnection_IsInitialized(pc_, &initialized),
              rtcResultU4::kSuccess);
    EXPECT_EQ(initialized, rtcBool32::kFalse);
}

}  // namespace libwebrtc
