#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// Negative-path tests that don't need a peer connection.

TEST(RTCRtpTransceiverNegative, GetMediaTypeWithNullHandleFails) {
    rtcMediaType type{};
    EXPECT_EQ(RTCRtpTransceiver_GetMediaType(nullptr, &type),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpTransceiverNegative, GetMediaTypeWithNullOutPointerFails) {
    EXPECT_EQ(RTCRtpTransceiver_GetMediaType(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCRtpTransceiverNegative, GetSenderWithNullHandleFails) {
    rtcRtpSenderHandle sender = nullptr;
    EXPECT_EQ(RTCRtpTransceiver_GetSender(nullptr, &sender),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpTransceiverNegative, GetReceiverWithNullHandleFails) {
    rtcRtpReceiverHandle receiver = nullptr;
    EXPECT_EQ(RTCRtpTransceiver_GetReceiver(nullptr, &receiver),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpTransceiverNegative, GetDirectionWithNullHandleFails) {
    rtcRtpTransceiverDirection dir{};
    EXPECT_EQ(RTCRtpTransceiver_GetDirection(nullptr, &dir),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpTransceiverNegative, GetMidWithNullHandleFails) {
    char buf[64] = {};
    EXPECT_EQ(RTCRtpTransceiver_GetMid(nullptr, buf, sizeof(buf)),
              rtcResultU4::kInvalidNativeHandle);
}


TEST(RTCRtpTransceiverNegative, StoppedWithNullHandleFails) {
    rtcBool32 stopped{};
    EXPECT_EQ(RTCRtpTransceiver_Stopped(nullptr, &stopped),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpTransceiverNegative, ListGetItemWithNullHandleFails) {
    rtcRtpTransceiverHandle item = nullptr;
    EXPECT_EQ(RTCRtpTransceiverList_GetItem(nullptr, 0, &item),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpTransceiverNegative, ListGetCountWithNullHandleReturnsZero) {
    EXPECT_EQ(RTCRtpTransceiverList_GetCount(nullptr), 0);
}

// RTCRtpTransceiverInit tests (no peer connection required).

TEST(RTCRtpTransceiverInit, CreateAndGetDirection) {
    rtcRtpTransceiverInitHandle init = nullptr;
    EXPECT_EQ(RTCRtpTransceiverInit_Create(
                  rtcRtpTransceiverDirection::kSendRecv, "stream-1", nullptr,
                  &init),
              rtcResultU4::kSuccess);
    ASSERT_NE(init, nullptr);

    rtcRtpTransceiverDirection dir{};
    EXPECT_EQ(RTCRtpTransceiverInit_GetDirection(init, &dir),
              rtcResultU4::kSuccess);
    EXPECT_EQ(dir, rtcRtpTransceiverDirection::kSendRecv);

    EXPECT_EQ(RTCRtpTransceiverInit_SetDirection(
                  init, rtcRtpTransceiverDirection::kRecvOnly),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCRtpTransceiverInit_GetDirection(init, &dir),
              rtcResultU4::kSuccess);
    EXPECT_EQ(dir, rtcRtpTransceiverDirection::kRecvOnly);

    RefCountedObject_Release(init);
}

TEST(RTCRtpTransceiverInit, GetAndSetStreamIds) {
    rtcRtpTransceiverInitHandle init = nullptr;
    ASSERT_EQ(RTCRtpTransceiverInit_Create(
                  rtcRtpTransceiverDirection::kSendRecv, "stream-1", nullptr,
                  &init),
              rtcResultU4::kSuccess);
    ASSERT_NE(init, nullptr);

    EXPECT_EQ(RTCRtpTransceiverInit_SetStreamIds(init, "abc"),
              rtcResultU4::kSuccess);

    char buf[128] = {};
    EXPECT_EQ(RTCRtpTransceiverInit_GetStreamIds(init, buf, sizeof(buf)),
              rtcResultU4::kSuccess);
    EXPECT_STREQ(buf, "abc");

    RefCountedObject_Release(init);
}

TEST(RTCRtpTransceiverInit, CreateWithNullOutPointerFails) {
    EXPECT_EQ(RTCRtpTransceiverInit_Create(
                  rtcRtpTransceiverDirection::kSendRecv, "stream-1", nullptr,
                  nullptr),
              rtcResultU4::kInvalidPointer);
}

// Fixture that owns a fresh peer connection for each test.
class RTCRtpTransceiverTest : public ::testing::Test {
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

TEST_F(RTCRtpTransceiverTest, GetMediaTypeIsAudio) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    rtcMediaType type{};
    EXPECT_EQ(RTCRtpTransceiver_GetMediaType(transceiver, &type),
              rtcResultU4::kSuccess);
    EXPECT_EQ(type, RTCMediaType::AUDIO);

    RefCountedObject_Release(transceiver);
}

TEST_F(RTCRtpTransceiverTest, GetSenderReturnsHandle) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    rtcRtpSenderHandle sender = nullptr;
    EXPECT_EQ(RTCRtpTransceiver_GetSender(transceiver, &sender),
              rtcResultU4::kSuccess);
    EXPECT_NE(sender, nullptr);

    if (sender) RefCountedObject_Release(sender);
    RefCountedObject_Release(transceiver);
}

TEST_F(RTCRtpTransceiverTest, GetReceiverReturnsHandle) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    rtcRtpReceiverHandle receiver = nullptr;
    EXPECT_EQ(RTCRtpTransceiver_GetReceiver(transceiver, &receiver),
              rtcResultU4::kSuccess);
    EXPECT_NE(receiver, nullptr);

    if (receiver) RefCountedObject_Release(receiver);
    RefCountedObject_Release(transceiver);
}

TEST_F(RTCRtpTransceiverTest, GetMidWithNullBufferFails) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    // Valid handle, null output buffer -> the buffer pointer is rejected.
    EXPECT_EQ(RTCRtpTransceiver_GetMid(transceiver, nullptr, 64),
              rtcResultU4::kInvalidPointer);

    RefCountedObject_Release(transceiver);
}

TEST_F(RTCRtpTransceiverTest, GetDirectionReturnsSuccess) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    rtcRtpTransceiverDirection dir{};
    EXPECT_EQ(RTCRtpTransceiver_GetDirection(transceiver, &dir),
              rtcResultU4::kSuccess);

    RefCountedObject_Release(transceiver);
}

TEST_F(RTCRtpTransceiverTest, SetDirectionWithErrorReturnsSuccess) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    char err[256] = {};
    EXPECT_EQ(RTCRtpTransceiver_SetDirectionWithError(
                  transceiver, rtcRtpTransceiverDirection::kRecvOnly, err,
                  sizeof(err)),
              rtcResultU4::kSuccess);

    rtcRtpTransceiverDirection dir{};
    EXPECT_EQ(RTCRtpTransceiver_GetDirection(transceiver, &dir),
              rtcResultU4::kSuccess);
    EXPECT_EQ(dir, rtcRtpTransceiverDirection::kRecvOnly);

    RefCountedObject_Release(transceiver);
}

TEST_F(RTCRtpTransceiverTest, GetMidReturnsSuccess) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    char buf[128] = {};
    EXPECT_EQ(RTCRtpTransceiver_GetMid(transceiver, buf, sizeof(buf)),
              rtcResultU4::kSuccess);

    RefCountedObject_Release(transceiver);
}

TEST_F(RTCRtpTransceiverTest, GetTransceiverIdReturnsSuccess) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    char buf[128] = {};
    EXPECT_EQ(RTCRtpTransceiver_GetTransceiverId(transceiver, buf, sizeof(buf)),
              rtcResultU4::kSuccess);

    RefCountedObject_Release(transceiver);
}

TEST_F(RTCRtpTransceiverTest, StoppedAndStoppingReturnSuccess) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    rtcBool32 stopped = rtcBool32::kTrue;
    EXPECT_EQ(RTCRtpTransceiver_Stopped(transceiver, &stopped),
              rtcResultU4::kSuccess);
    EXPECT_EQ(stopped, rtcBool32::kFalse);

    rtcBool32 stopping = rtcBool32::kTrue;
    EXPECT_EQ(RTCRtpTransceiver_Stopping(transceiver, &stopping),
              rtcResultU4::kSuccess);
    EXPECT_EQ(stopping, rtcBool32::kFalse);

    RefCountedObject_Release(transceiver);
}

TEST_F(RTCRtpTransceiverTest, CurrentAndFiredDirectionReturnSuccess) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    rtcRtpTransceiverDirection current{};
    EXPECT_EQ(RTCRtpTransceiver_GetCurrentDirection(transceiver, &current),
              rtcResultU4::kSuccess);

    rtcRtpTransceiverDirection fired{};
    EXPECT_EQ(RTCRtpTransceiver_GetFiredDirection(transceiver, &fired),
              rtcResultU4::kSuccess);

    RefCountedObject_Release(transceiver);
}

TEST_F(RTCRtpTransceiverTest, StopStandardReturnsSuccess) {
    rtcRtpTransceiverHandle transceiver = AddAudioTransceiver();
    ASSERT_NE(transceiver, nullptr);

    char buf[256] = {};
    EXPECT_EQ(RTCRtpTransceiver_StopStandard(transceiver, buf, sizeof(buf)),
              rtcResultU4::kSuccess);

    RefCountedObject_Release(transceiver);
}

// --- List getters via GetTransceivers ---

TEST_F(RTCRtpTransceiverTest, GetTransceiversListCountAndItem) {
    rtcRtpTransceiverHandle added = AddAudioTransceiver();
    ASSERT_NE(added, nullptr);
    RefCountedObject_Release(added);

    rtcRtpTransceiverListHandle list = nullptr;
    EXPECT_EQ(RTCPeerConnection_GetTransceivers(pc_, &list),
              rtcResultU4::kSuccess);
    ASSERT_NE(list, nullptr);

    int count = RTCRtpTransceiverList_GetCount(list);
    EXPECT_GE(count, 1);

    rtcRtpTransceiverHandle item = nullptr;
    EXPECT_EQ(RTCRtpTransceiverList_GetItem(list, 0, &item),
              rtcResultU4::kSuccess);
    EXPECT_NE(item, nullptr);
    if (item) RefCountedObject_Release(item);

    rtcRtpTransceiverHandle oob = nullptr;
    EXPECT_EQ(RTCRtpTransceiverList_GetItem(list, count, &oob),
              rtcResultU4::kOutOfRange);
    EXPECT_EQ(oob, nullptr);

    rtcRtpTransceiverHandle neg = nullptr;
    EXPECT_EQ(RTCRtpTransceiverList_GetItem(list, -1, &neg),
              rtcResultU4::kOutOfRange);

    RefCountedObject_Release(list);
}

}  // namespace libwebrtc
