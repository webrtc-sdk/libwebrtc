#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// --- RTCDtlsTransportInformation null-handle negatives ---

TEST(RTCDtlsTransportInformationNegative, GetStateWithNullHandleFails) {
    rtcDtlsTransportState state{};
    EXPECT_EQ(RTCDtlsTransportInformation_GetState(nullptr, &state),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDtlsTransportInformationNegative, GetStateWithNullOutPointerFails) {
    EXPECT_EQ(RTCDtlsTransportInformation_GetState(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCDtlsTransportInformationNegative, GetSslCipherSuiteWithNullHandleFails) {
    int suite = 0;
    EXPECT_EQ(RTCDtlsTransportInformation_GetSslCipherSuite(nullptr, &suite),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDtlsTransportInformationNegative,
     GetSslCipherSuiteWithNullOutPointerFails) {
    EXPECT_EQ(RTCDtlsTransportInformation_GetSslCipherSuite(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCDtlsTransportInformationNegative,
     GetSrtpCipherSuiteWithNullHandleFails) {
    int suite = 0;
    EXPECT_EQ(RTCDtlsTransportInformation_GetSrtpCipherSuite(nullptr, &suite),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDtlsTransportInformationNegative,
     GetSrtpCipherSuiteWithNullOutPointerFails) {
    EXPECT_EQ(RTCDtlsTransportInformation_GetSrtpCipherSuite(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

// --- RTCDtlsTransport null-handle negatives ---

TEST(RTCDtlsTransportNegative, GetInformationWithNullHandleFails) {
    rtcDtlsTransportInformationHandle info = nullptr;
    EXPECT_EQ(RTCDtlsTransport_GetInformation(nullptr, &info),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDtlsTransportNegative, RegisterObserverWithNullHandleFails) {
    EXPECT_EQ(RTCDtlsTransport_RegisterObserver(nullptr, nullptr),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCDtlsTransportNegative, UnregisterObserverWithNullHandleFails) {
    EXPECT_EQ(RTCDtlsTransport_UnregisterObserver(nullptr),
              rtcResultU4::kInvalidNativeHandle);
}

// Fixture that owns a fresh peer connection. A DTLS transport is only reachable
// from an RTP sender; before negotiation the sender has none, so the getter is
// exercised for its return code without assuming a live transport.
class RTCDtlsTransportTest : public ::testing::Test {
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

    rtcPeerConnectionFactoryHandle factory_ = nullptr;
    rtcPeerConnectionHandle pc_ = nullptr;
};

// Before negotiation a sender typically has no DTLS transport; the call must
// still return a valid result code (success with a null/absent transport).
TEST_F(RTCDtlsTransportTest, SenderGetDtlsTransportReturnsValidResult) {
    rtcRtpTransceiverHandle transceiver = nullptr;
    ASSERT_EQ(RTCPeerConnection_AddTransceiver1(
                  pc_, RTCMediaType::AUDIO, &transceiver),
              rtcResultU4::kSuccess);
    ASSERT_NE(transceiver, nullptr);

    rtcRtpSenderHandle sender = nullptr;
    ASSERT_EQ(RTCRtpTransceiver_GetSender(transceiver, &sender),
              rtcResultU4::kSuccess);
    ASSERT_NE(sender, nullptr);

    rtcDtlsTransportHandle transport = nullptr;
    EXPECT_EQ(RTCRtpSender_GetDtlsTransport(sender, &transport),
              rtcResultU4::kSuccess);

    // If a transport exists, GetInformation should also succeed.
    if (transport) {
        rtcDtlsTransportInformationHandle info = nullptr;
        EXPECT_EQ(RTCDtlsTransport_GetInformation(transport, &info),
                  rtcResultU4::kSuccess);
        if (info) {
            rtcDtlsTransportState state{};
            EXPECT_EQ(RTCDtlsTransportInformation_GetState(info, &state),
                      rtcResultU4::kSuccess);
            RefCountedObject_Release(info);
        }
        RefCountedObject_Release(transport);
    }

    RefCountedObject_Release(sender);
    RefCountedObject_Release(transceiver);
}

TEST_F(RTCDtlsTransportTest, ReceiverGetDtlsTransportReturnsValidResult) {
    rtcRtpTransceiverHandle transceiver = nullptr;
    ASSERT_EQ(RTCPeerConnection_AddTransceiver1(
                  pc_, RTCMediaType::AUDIO, &transceiver),
              rtcResultU4::kSuccess);
    ASSERT_NE(transceiver, nullptr);

    rtcRtpReceiverHandle receiver = nullptr;
    ASSERT_EQ(RTCRtpTransceiver_GetReceiver(transceiver, &receiver),
              rtcResultU4::kSuccess);
    ASSERT_NE(receiver, nullptr);

    rtcDtlsTransportHandle transport = nullptr;
    EXPECT_EQ(RTCRtpReceiver_GetDtlsTransport(receiver, &transport),
              rtcResultU4::kSuccess);
    if (transport) RefCountedObject_Release(transport);

    RefCountedObject_Release(receiver);
    RefCountedObject_Release(transceiver);
}

}  // namespace libwebrtc
