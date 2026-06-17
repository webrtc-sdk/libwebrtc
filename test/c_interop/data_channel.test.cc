#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// --- Null-handle negatives ---

TEST(RTCDataChannelNegative, SendWithNullHandleFails) {
    const unsigned char data[] = {'h', 'i'};
    EXPECT_EQ(RTCDataChannel_Send(nullptr, data, sizeof(data)),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDataChannelNegative, CloseWithNullHandleFails) {
    EXPECT_EQ(RTCDataChannel_Close(nullptr),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDataChannelNegative, RegisterObserverWithNullHandleFails) {
    EXPECT_EQ(RTCDataChannel_RegisterObserver(nullptr, nullptr),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDataChannelNegative, UnregisterObserverWithNullHandleFails) {
    EXPECT_EQ(RTCDataChannel_UnregisterObserver(nullptr),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDataChannelNegative, GetLabelWithNullHandleFails) {
    char buf[64] = {};
    EXPECT_EQ(RTCDataChannel_GetLabel(nullptr, buf, sizeof(buf)),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDataChannelNegative, GetIdWithNullHandleFails) {
    int id = 0;
    EXPECT_EQ(RTCDataChannel_GetId(nullptr, &id),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDataChannelNegative, GetIdWithNullOutPointerFails) {
    EXPECT_EQ(RTCDataChannel_GetId(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCDataChannelNegative, GetStateWithNullHandleFails) {
    rtcDataChannelState state{};
    EXPECT_EQ(RTCDataChannel_GetState(nullptr, &state),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDataChannelNegative, GetStateWithNullOutPointerFails) {
    EXPECT_EQ(RTCDataChannel_GetState(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

// Fixture that owns a fresh peer connection for each test.
class RTCDataChannelTest : public ::testing::Test {
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

    rtcDataChannelHandle CreateDataChannel(const char* label,
                                           rtcDataChannelInit* init) {
        rtcDataChannelHandle dc = nullptr;
        EXPECT_EQ(RTCPeerConnection_CreateDataChannel(pc_, label, init, &dc),
                  rtcResultU4::kSuccess);
        return dc;
    }

    rtcPeerConnectionFactoryHandle factory_ = nullptr;
    rtcPeerConnectionHandle pc_ = nullptr;
};

TEST_F(RTCDataChannelTest, GetLabelMatchesCreated) {
    rtcDataChannelHandle dc = CreateDataChannel("chat", nullptr);
    ASSERT_NE(dc, nullptr);

    char label[128] = {};
    EXPECT_EQ(RTCDataChannel_GetLabel(dc, label, sizeof(label)),
              rtcResultU4::kSuccess);
    EXPECT_STREQ(label, "chat");

    RefCountedObject_Release(dc);
}

TEST_F(RTCDataChannelTest, GetLabelWithNullBufferFails) {
    rtcDataChannelHandle dc = CreateDataChannel("chat", nullptr);
    ASSERT_NE(dc, nullptr);

    // Valid handle, null output buffer -> the buffer pointer is rejected.
    EXPECT_EQ(RTCDataChannel_GetLabel(dc, nullptr, 64),
              rtcResultU4::kInvalidPointer);

    RefCountedObject_Release(dc);
}

TEST_F(RTCDataChannelTest, GetIdReturnsSuccess) {
    rtcDataChannelHandle dc = CreateDataChannel("chat", nullptr);
    ASSERT_NE(dc, nullptr);

    int id = -2;
    EXPECT_EQ(RTCDataChannel_GetId(dc, &id), rtcResultU4::kSuccess);

    RefCountedObject_Release(dc);
}

TEST_F(RTCDataChannelTest, GetStateReturnsSuccess) {
    rtcDataChannelHandle dc = CreateDataChannel("chat", nullptr);
    ASSERT_NE(dc, nullptr);

    rtcDataChannelState state{};
    EXPECT_EQ(RTCDataChannel_GetState(dc, &state), rtcResultU4::kSuccess);

    RefCountedObject_Release(dc);
}

TEST_F(RTCDataChannelTest, CreateWithInitGettersSucceed) {
    rtcDataChannelInit init{};
    init.ordered = rtcBool32::kTrue;
    init.maxRetransmits = 3;

    rtcDataChannelHandle dc = CreateDataChannel("reliable", &init);
    ASSERT_NE(dc, nullptr);

    char label[128] = {};
    EXPECT_EQ(RTCDataChannel_GetLabel(dc, label, sizeof(label)),
              rtcResultU4::kSuccess);
    EXPECT_STREQ(label, "reliable");

    rtcDataChannelState state{};
    EXPECT_EQ(RTCDataChannel_GetState(dc, &state), rtcResultU4::kSuccess);

    RefCountedObject_Release(dc);
}

TEST_F(RTCDataChannelTest, RegisterAndUnregisterObserver) {
    rtcDataChannelHandle dc = CreateDataChannel("chat", nullptr);
    ASSERT_NE(dc, nullptr);

    rtcDataChannelObserverCallbacks callbacks{};
    EXPECT_EQ(RTCDataChannel_RegisterObserver(dc, &callbacks),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCDataChannel_UnregisterObserver(dc), rtcResultU4::kSuccess);

    RefCountedObject_Release(dc);
}

TEST_F(RTCDataChannelTest, RegisterObserverWithNullCallbacksFails) {
    rtcDataChannelHandle dc = CreateDataChannel("chat", nullptr);
    ASSERT_NE(dc, nullptr);

    EXPECT_EQ(RTCDataChannel_RegisterObserver(dc, nullptr),
              rtcResultU4::kInvalidParameter);

    RefCountedObject_Release(dc);
}

TEST_F(RTCDataChannelTest, SendSmallBufferReturnsSuccess) {
    rtcDataChannelHandle dc = CreateDataChannel("chat", nullptr);
    ASSERT_NE(dc, nullptr);

    const unsigned char data[] = {'p', 'i', 'n', 'g'};
    EXPECT_EQ(RTCDataChannel_Send(dc, data, sizeof(data)),
              rtcResultU4::kSuccess);

    RefCountedObject_Release(dc);
}

TEST_F(RTCDataChannelTest, CloseReturnsSuccess) {
    rtcDataChannelHandle dc = CreateDataChannel("chat", nullptr);
    ASSERT_NE(dc, nullptr);

    EXPECT_EQ(RTCDataChannel_Close(dc), rtcResultU4::kSuccess);

    RefCountedObject_Release(dc);
}

}  // namespace libwebrtc
