#include <memory>
#include <string>
#include <vector>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_rtp_sender.h"
#include "rtc_rtp_receiver.h"
#include "rtc_rtp_transceiver.h"
#include "rtc_rtp_parameters.h"
#include "rtc_media_track.h"
#include "rtc_media_stream.h"
#include "rtc_audio_source.h"
#include "rtc_audio_track.h"
#include "rtc_dtmf_sender.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_mediaconstraints.h"
#include "libwebrtc.h"

namespace libwebrtc {

// Exercises the C++ RTCRtpSender and RTCRtpReceiver APIs obtained via
// pc->AddTrack(...) and pc->AddTransceiver(...).
class RTCRtpSenderReceiverTest : public ::testing::Test {
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

  scoped_refptr<RTCAudioTrack> CreateAudioTrack(const string& track_id) {
    scoped_refptr<RTCAudioSource> source =
        factory_->CreateAudioSource("test-source");
    EXPECT_TRUE(source.get() != nullptr);
    if (!source) return nullptr;
    return factory_->CreateAudioTrack(source, track_id);
  }

  scoped_refptr<RTCRtpTransceiver> AddAudioTransceiver() {
    return pc_->AddTransceiver(RTCMediaType::AUDIO);
  }

  scoped_refptr<RTCPeerConnectionFactory> factory_;
  scoped_refptr<RTCPeerConnection> pc_;
};

// --- Sender via AddTrack ---

TEST_F(RTCRtpSenderReceiverTest, SenderFromAddTrackExposesTrack) {
  scoped_refptr<RTCAudioTrack> track = CreateAudioTrack("audio-track");
  ASSERT_TRUE(track.get() != nullptr);

  std::vector<string> stream_ids;
  stream_ids.push_back(string("stream-id"));
  scoped_refptr<RTCRtpSender> sender = pc_->AddTrack(track, stream_ids);
  ASSERT_TRUE(sender.get() != nullptr);

  scoped_refptr<RTCMediaTrack> sender_track = sender->track();
  ASSERT_TRUE(sender_track.get() != nullptr);
  EXPECT_EQ(sender_track->id().std_string(), track->id().std_string());
  EXPECT_EQ(sender_track->kind().std_string(), std::string("audio"));

  EXPECT_EQ(sender->media_type(), RTCMediaType::AUDIO);
  EXPECT_GT(sender->id().std_string().size(), 0u);
}

// --- Sender via transceiver ---

TEST_F(RTCRtpSenderReceiverTest, SenderGetters) {
  scoped_refptr<RTCRtpTransceiver> transceiver = AddAudioTransceiver();
  ASSERT_TRUE(transceiver.get() != nullptr);

  scoped_refptr<RTCRtpSender> sender = transceiver->sender();
  ASSERT_TRUE(sender.get() != nullptr);

  EXPECT_EQ(sender->media_type(), RTCMediaType::AUDIO);
  EXPECT_GT(sender->id().std_string().size(), 0u);

  // ssrc / stream_ids / init_send_encodings are retrievable.
  sender->ssrc();
  sender->stream_ids();
  sender->init_send_encodings();
}

TEST_F(RTCRtpSenderReceiverTest, SenderSetStreamIds) {
  scoped_refptr<RTCRtpTransceiver> transceiver = AddAudioTransceiver();
  ASSERT_TRUE(transceiver.get() != nullptr);

  scoped_refptr<RTCRtpSender> sender = transceiver->sender();
  ASSERT_TRUE(sender.get() != nullptr);

  std::vector<string> ids;
  ids.push_back(string("s1"));
  sender->set_stream_ids(ids);
}

TEST_F(RTCRtpSenderReceiverTest, SenderSetTrack) {
  scoped_refptr<RTCRtpTransceiver> transceiver = AddAudioTransceiver();
  ASSERT_TRUE(transceiver.get() != nullptr);

  scoped_refptr<RTCRtpSender> sender = transceiver->sender();
  ASSERT_TRUE(sender.get() != nullptr);

  scoped_refptr<RTCAudioTrack> track = CreateAudioTrack("attached-track");
  ASSERT_TRUE(track.get() != nullptr);

  EXPECT_TRUE(sender->set_track(track));

  scoped_refptr<RTCMediaTrack> got = sender->track();
  ASSERT_TRUE(got.get() != nullptr);
  EXPECT_EQ(got->id().std_string(), track->id().std_string());
}

TEST_F(RTCRtpSenderReceiverTest, SenderParametersRoundTrip) {
  scoped_refptr<RTCRtpTransceiver> transceiver = AddAudioTransceiver();
  ASSERT_TRUE(transceiver.get() != nullptr);

  scoped_refptr<RTCRtpSender> sender = transceiver->sender();
  ASSERT_TRUE(sender.get() != nullptr);

  scoped_refptr<RTCRtpParameters> params = sender->parameters();
  ASSERT_TRUE(params.get() != nullptr);

  // Setting back the unmodified parameters should succeed.
  EXPECT_TRUE(sender->set_parameters(params));
}

TEST_F(RTCRtpSenderReceiverTest, SenderDtmfSender) {
  scoped_refptr<RTCRtpTransceiver> transceiver = AddAudioTransceiver();
  ASSERT_TRUE(transceiver.get() != nullptr);

  scoped_refptr<RTCRtpSender> sender = transceiver->sender();
  ASSERT_TRUE(sender.get() != nullptr);

  // Audio senders expose a DTMF sender; just confirm the accessor works.
  sender->dtmf_sender();
}

// --- Receiver via transceiver ---

TEST_F(RTCRtpSenderReceiverTest, ReceiverGetters) {
  scoped_refptr<RTCRtpTransceiver> transceiver = AddAudioTransceiver();
  ASSERT_TRUE(transceiver.get() != nullptr);

  scoped_refptr<RTCRtpReceiver> receiver = transceiver->receiver();
  ASSERT_TRUE(receiver.get() != nullptr);

  EXPECT_EQ(receiver->media_type(), RTCMediaType::AUDIO);
  EXPECT_GT(receiver->id().std_string().size(), 0u);

  scoped_refptr<RTCMediaTrack> track = receiver->track();
  ASSERT_TRUE(track.get() != nullptr);
  EXPECT_EQ(track->kind().std_string(), std::string("audio"));

  scoped_refptr<RTCRtpParameters> params = receiver->parameters();
  EXPECT_TRUE(params.get() != nullptr);

  // streams() and stream_ids() are retrievable (may be empty pre-negotiation).
  receiver->streams();
  receiver->stream_ids();
}

// --- List getters ---

TEST_F(RTCRtpSenderReceiverTest, GetSendersListsSender) {
  scoped_refptr<RTCRtpTransceiver> transceiver = AddAudioTransceiver();
  ASSERT_TRUE(transceiver.get() != nullptr);

  vector<scoped_refptr<RTCRtpSender>> senders = pc_->senders();
  ASSERT_GE(senders.size(), 1u);

  scoped_refptr<RTCRtpSender> first = senders[0];
  ASSERT_TRUE(first.get() != nullptr);
  EXPECT_EQ(first->media_type(), RTCMediaType::AUDIO);
}

TEST_F(RTCRtpSenderReceiverTest, GetReceiversListsReceiver) {
  scoped_refptr<RTCRtpTransceiver> transceiver = AddAudioTransceiver();
  ASSERT_TRUE(transceiver.get() != nullptr);

  vector<scoped_refptr<RTCRtpReceiver>> receivers = pc_->receivers();
  ASSERT_GE(receivers.size(), 1u);

  scoped_refptr<RTCRtpReceiver> first = receivers[0];
  ASSERT_TRUE(first.get() != nullptr);
  EXPECT_EQ(first->media_type(), RTCMediaType::AUDIO);
}

}  // namespace libwebrtc
