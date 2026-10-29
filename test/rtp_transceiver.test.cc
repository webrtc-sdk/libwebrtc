#include <memory>
#include <string>
#include <vector>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_rtp_transceiver.h"
#include "rtc_rtp_sender.h"
#include "rtc_rtp_receiver.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_mediaconstraints.h"
#include "libwebrtc.h"

namespace libwebrtc {

// Exercises the C++ RTCRtpTransceiver API created via pc->AddTransceiver(...).
class RTCRtpTransceiverTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(LibWebRTC::Initialize());
    factory_ = LibWebRTC::CreateRTCPeerConnectionFactory();
    ASSERT_TRUE(factory_.get() != nullptr);
    ASSERT_TRUE(factory_->Initialize());

    RTCConfiguration config{};
    scoped_refptr<RTCMediaConstraints> constraints =
        RTCMediaConstraints::Create();
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

  scoped_refptr<RTCRtpTransceiver> AddAudioTransceiver() {
    return pc_->AddTransceiver(RTCMediaType::AUDIO);
  }

  scoped_refptr<RTCPeerConnectionFactory> factory_;
  scoped_refptr<RTCPeerConnection> pc_;
};

TEST_F(RTCRtpTransceiverTest, MediaTypeIsAudio) {
  scoped_refptr<RTCRtpTransceiver> transceiver = AddAudioTransceiver();
  ASSERT_TRUE(transceiver.get() != nullptr);

  EXPECT_EQ(transceiver->media_type(), RTCMediaType::AUDIO);
}

TEST_F(RTCRtpTransceiverTest, SenderAndReceiverAreValid) {
  scoped_refptr<RTCRtpTransceiver> transceiver = AddAudioTransceiver();
  ASSERT_TRUE(transceiver.get() != nullptr);

  scoped_refptr<RTCRtpSender> sender = transceiver->sender();
  EXPECT_TRUE(sender.get() != nullptr);

  scoped_refptr<RTCRtpReceiver> receiver = transceiver->receiver();
  EXPECT_TRUE(receiver.get() != nullptr);
}

TEST_F(RTCRtpTransceiverTest, DirectionGetAndSet) {
  scoped_refptr<RTCRtpTransceiver> transceiver = AddAudioTransceiver();
  ASSERT_TRUE(transceiver.get() != nullptr);

  // Default direction is retrievable.
  transceiver->direction();

  // Switch to recv-only and verify the change took effect (empty error = ok).
  string err =
      transceiver->SetDirectionWithError(RTCRtpTransceiverDirection::kRecvOnly);
  EXPECT_EQ(err.std_string().size(), 0u);
  EXPECT_EQ(transceiver->direction(), RTCRtpTransceiverDirection::kRecvOnly);

  // current_direction / fired_direction are retrievable.
  transceiver->current_direction();
  transceiver->fired_direction();
}

TEST_F(RTCRtpTransceiverTest, MidAndTransceiverId) {
  scoped_refptr<RTCRtpTransceiver> transceiver = AddAudioTransceiver();
  ASSERT_TRUE(transceiver.get() != nullptr);

  // mid is typically unset before negotiation; just confirm retrievable.
  transceiver->mid();

  // transceiver_id should be a non-empty internal identifier.
  string id = transceiver->transceiver_id();
  EXPECT_GE(id.std_string().size(), 0u);
}

TEST_F(RTCRtpTransceiverTest, StoppedAndStopping) {
  scoped_refptr<RTCRtpTransceiver> transceiver = AddAudioTransceiver();
  ASSERT_TRUE(transceiver.get() != nullptr);

  EXPECT_FALSE(transceiver->Stopped());
  EXPECT_FALSE(transceiver->Stopping());
}

TEST_F(RTCRtpTransceiverTest, StopStandardReturnsNoError) {
  scoped_refptr<RTCRtpTransceiver> transceiver = AddAudioTransceiver();
  ASSERT_TRUE(transceiver.get() != nullptr);

  // StopStandard returns an error string ("" on success).
  string err = transceiver->StopStandard();
  EXPECT_EQ(err.std_string().size(), 0u);
}

TEST_F(RTCRtpTransceiverTest, GetTransceiversListsAddedTransceiver) {
  scoped_refptr<RTCRtpTransceiver> added = AddAudioTransceiver();
  ASSERT_TRUE(added.get() != nullptr);

  vector<scoped_refptr<RTCRtpTransceiver>> list = pc_->transceivers();
  ASSERT_GE(list.size(), 1u);

  scoped_refptr<RTCRtpTransceiver> first = list[0];
  ASSERT_TRUE(first.get() != nullptr);
  EXPECT_EQ(first->media_type(), RTCMediaType::AUDIO);
}

}  // namespace libwebrtc
