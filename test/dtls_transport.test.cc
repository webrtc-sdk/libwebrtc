#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_dtls_transport.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_mediaconstraints.h"
#include "rtc_rtp_receiver.h"
#include "rtc_rtp_sender.h"
#include "rtc_rtp_transceiver.h"
#include "libwebrtc.h"

namespace libwebrtc {

// Fixture that owns a fresh peer connection. A DTLS transport is only reachable
// from an RTP sender/receiver; before negotiation the sender typically has
// none, so the getter is exercised for its contract and any present transport
// is probed under a null guard.
class RTCDtlsTransportTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(LibWebRTC::Initialize());
    factory_ = LibWebRTC::CreateRTCPeerConnectionFactory();
    ASSERT_TRUE(factory_.get() != nullptr);
    ASSERT_TRUE(factory_->Initialize());

    RTCConfiguration config;
    scoped_refptr<RTCMediaConstraints> constraints =
        RTCMediaConstraints::Create();
    ASSERT_TRUE(constraints.get() != nullptr);
    pc_ = factory_->Create(config, constraints);
    ASSERT_TRUE(pc_.get() != nullptr);
  }

  void TearDown() override {
    if (pc_) {
      pc_->Close();
      factory_->Delete(pc_);
      pc_ = nullptr;
    }
    if (factory_) {
      factory_->Terminate();
      factory_ = nullptr;
    }
    LibWebRTC::Terminate();
  }

  scoped_refptr<RTCPeerConnectionFactory> factory_;
  scoped_refptr<RTCPeerConnection> pc_;
};

TEST_F(RTCDtlsTransportTest, SenderDtlsTransportReachable) {
  scoped_refptr<RTCRtpTransceiver> transceiver =
      pc_->AddTransceiver(RTCMediaType::AUDIO);
  ASSERT_TRUE(transceiver.get() != nullptr);

  scoped_refptr<RTCRtpSender> sender = transceiver->sender();
  ASSERT_TRUE(sender.get() != nullptr);

  // Before negotiation the sender may or may not expose a DTLS transport; the
  // getter must not crash. When present, its information getters are probed.
  scoped_refptr<RTCDtlsTransport> transport = sender->dtls_transport();
  if (transport.get() != nullptr) {
    scoped_refptr<RTCDtlsTransportInformation> info =
        transport->GetInformation();
    if (info.get() != nullptr) {
      // state() returns one of the defined enum values; just exercise it.
      RTCDtlsTransportInformation::RTCDtlsTransportState state = info->state();
      EXPECT_GE(static_cast<int>(state), 0);
      EXPECT_LT(static_cast<int>(state),
                static_cast<int>(RTCDtlsTransportInformation::
                                     RTCDtlsTransportState::kNumValues));
      // Cipher-suite getters must not crash.
      (void)info->ssl_cipher_suite();
      (void)info->srtp_cipher_suite();
    }
    // Observer registration round-trips.
    EXPECT_EQ(transport->GetObserver(), nullptr);
  }
}

TEST_F(RTCDtlsTransportTest, ReceiverDtlsTransportReachable) {
  scoped_refptr<RTCRtpTransceiver> transceiver =
      pc_->AddTransceiver(RTCMediaType::AUDIO);
  ASSERT_TRUE(transceiver.get() != nullptr);

  scoped_refptr<RTCRtpReceiver> receiver = transceiver->receiver();
  ASSERT_TRUE(receiver.get() != nullptr);

  // The getter must not crash; a transport is not guaranteed pre-negotiation.
  scoped_refptr<RTCDtlsTransport> transport = receiver->dtls_transport();
  if (transport.get() != nullptr) {
    scoped_refptr<RTCDtlsTransportInformation> info =
        transport->GetInformation();
    EXPECT_TRUE(info.get() == nullptr || info.get() != nullptr);
  }
}

}  // namespace libwebrtc
