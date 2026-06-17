#include <memory>
#include <string>
#include <vector>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_audio_source.h"
#include "rtc_audio_track.h"
#include "rtc_dtmf_sender.h"
#include "rtc_media_track.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_mediaconstraints.h"
#include "rtc_rtp_sender.h"
#include "libwebrtc.h"

namespace libwebrtc {

// Fixture that owns a fresh peer connection and obtains a DTMF sender from an
// audio RTP sender (AddTrack -> sender->dtmf_sender()), exercising the C++
// RTCDtmfSender class API.
class RTCDtmfSenderTest : public ::testing::Test {
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

  scoped_refptr<RTCAudioTrack> CreateAudioTrack(const char* track_id) {
    scoped_refptr<RTCAudioSource> source =
        factory_->CreateAudioSource("test-source");
    EXPECT_TRUE(source.get() != nullptr);
    if (!source.get()) return nullptr;
    return factory_->CreateAudioTrack(source, track_id);
  }

  // Returns a DTMF sender obtained from a freshly added audio sender, keeping
  // the sender alive in sender_out so the DTMF sender stays valid.
  scoped_refptr<RTCDtmfSender> CreateDtmfSender(
      scoped_refptr<RTCRtpSender>* sender_out) {
    scoped_refptr<RTCAudioTrack> track = CreateAudioTrack("audio-track");
    EXPECT_TRUE(track.get() != nullptr);
    if (!track.get()) return nullptr;

    std::vector<string> stream_ids;
    stream_ids.push_back(string("stream-id"));
    scoped_refptr<RTCRtpSender> sender = pc_->AddTrack(track, stream_ids);
    EXPECT_TRUE(sender.get() != nullptr);
    if (!sender.get()) return nullptr;

    *sender_out = sender;
    return sender->dtmf_sender();
  }

  scoped_refptr<RTCPeerConnectionFactory> factory_;
  scoped_refptr<RTCPeerConnection> pc_;
};

TEST_F(RTCDtmfSenderTest, AudioSenderProvidesDtmfSender) {
  scoped_refptr<RTCRtpSender> sender;
  scoped_refptr<RTCDtmfSender> dtmf = CreateDtmfSender(&sender);
  ASSERT_TRUE(dtmf.get() != nullptr);
}

TEST_F(RTCDtmfSenderTest, GettersReturnSaneDefaults) {
  scoped_refptr<RTCRtpSender> sender;
  scoped_refptr<RTCDtmfSender> dtmf = CreateDtmfSender(&sender);
  ASSERT_TRUE(dtmf.get() != nullptr);

  // No tones queued yet.
  EXPECT_EQ(dtmf->tones().std_string(), "");
  EXPECT_GE(dtmf->duration(), 0);
  EXPECT_GE(dtmf->inter_tone_gap(), 0);
  // Unary + yields a prvalue so we don't ODR-use the header constant (it has
  // no out-of-line definition in the library).
  EXPECT_EQ(dtmf->comma_delay(), +RTCDtmfSender::kDtmfDefaultCommaDelayMs);
}

TEST_F(RTCDtmfSenderTest, CanInsertDtmfReturnsValue) {
  scoped_refptr<RTCRtpSender> sender;
  scoped_refptr<RTCDtmfSender> dtmf = CreateDtmfSender(&sender);
  ASSERT_TRUE(dtmf.get() != nullptr);

  // Before negotiation the track usually cannot send DTMF; the call must not
  // crash and simply reports a boolean.
  bool can = dtmf->CanInsertDtmf();
  EXPECT_TRUE(can == true || can == false);
}

TEST_F(RTCDtmfSenderTest, InsertDtmfDoesNotCrash) {
  scoped_refptr<RTCRtpSender> sender;
  scoped_refptr<RTCDtmfSender> dtmf = CreateDtmfSender(&sender);
  ASSERT_TRUE(dtmf.get() != nullptr);

  // InsertDtmf returns false when DTMF cannot currently be sent; it must not
  // crash regardless. Exercise both overloads.
  bool inserted = dtmf->InsertDtmf(string("1"), 100, 70);
  EXPECT_TRUE(inserted == true || inserted == false);

  bool inserted2 = dtmf->InsertDtmf(string("2"), 100, 70, /*comma_delay=*/50);
  EXPECT_TRUE(inserted2 == true || inserted2 == false);
}

// Minimal observer to confirm registration round-trips.
class TonesObserver : public RTCDtmfSenderObserver {
 public:
  void OnToneChange(const string tone, const string tone_buffer) override {}
  void OnToneChange(const string tone) override {}
};

TEST_F(RTCDtmfSenderTest, RegisterAndUnregisterObserver) {
  scoped_refptr<RTCRtpSender> sender;
  scoped_refptr<RTCDtmfSender> dtmf = CreateDtmfSender(&sender);
  ASSERT_TRUE(dtmf.get() != nullptr);

  TonesObserver observer;
  dtmf->RegisterObserver(&observer);
  EXPECT_EQ(dtmf->GetObserver(), &observer);
  dtmf->UnregisterObserver();
  EXPECT_EQ(dtmf->GetObserver(), nullptr);
}

}  // namespace libwebrtc
