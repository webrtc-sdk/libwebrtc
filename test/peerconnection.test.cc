#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_peerconnection_factory.h"
#include "rtc_peerconnection.h"
#include "rtc_audio_source.h"
#include "rtc_audio_track.h"
#include "rtc_data_channel.h"
#include "rtc_media_stream.h"
#include "rtc_mediaconstraints.h"
#include "rtc_rtp_sender.h"
#include "rtc_rtp_receiver.h"
#include "rtc_rtp_transceiver.h"
#include "libwebrtc.h"

namespace libwebrtc {
namespace {

// A simple latch usable from any thread to wait for an SDP callback.
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

}  // namespace

// Fixture that owns a fresh peer connection for each test. Mirrors
// test/c_interop/peerconnection.test.cc using the C++ class API.
class RTCPeerConnectionTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(LibWebRTC::Initialize());
    factory_ = LibWebRTC::CreateRTCPeerConnectionFactory();
    ASSERT_TRUE(factory_.get() != nullptr);
    ASSERT_TRUE(factory_->Initialize());

    RTCConfiguration config;
    constraints_ = RTCMediaConstraints::Create();
    ASSERT_TRUE(constraints_.get() != nullptr);
    pc_ = factory_->Create(config, constraints_);
    ASSERT_TRUE(pc_.get() != nullptr);
  }

  void TearDown() override {
    if (pc_) {
      pc_->Close();
      factory_->Delete(pc_);
      pc_ = nullptr;
    }
    constraints_ = nullptr;
    if (factory_) {
      EXPECT_TRUE(factory_->Terminate());
      factory_ = nullptr;
    }
    LibWebRTC::Terminate();
  }

  scoped_refptr<RTCAudioTrack> CreateAudioTrack(const string track_id) {
    scoped_refptr<RTCAudioSource> source =
        factory_->CreateAudioSource("test-source");
    EXPECT_TRUE(source.get() != nullptr);
    if (!source) return scoped_refptr<RTCAudioTrack>();
    return factory_->CreateAudioTrack(source, track_id);
  }

  scoped_refptr<RTCPeerConnectionFactory> factory_;
  scoped_refptr<RTCMediaConstraints> constraints_;
  scoped_refptr<RTCPeerConnection> pc_;
};

// --- State queries ---

TEST_F(RTCPeerConnectionTest, IsInitializedReturnsTrue) {
  EXPECT_TRUE(pc_->IsInitialized());
}

TEST_F(RTCPeerConnectionTest, InitialSignalingStateIsStable) {
  EXPECT_EQ(pc_->signaling_state(), RTCSignalingStateStable);
}

TEST_F(RTCPeerConnectionTest, InitialIceConnectionStateIsNew) {
  EXPECT_EQ(pc_->ice_connection_state(), RTCIceConnectionStateNew);
  EXPECT_EQ(pc_->standardized_ice_connection_state(),
            RTCIceConnectionStateNew);
}

TEST_F(RTCPeerConnectionTest, InitialPeerConnectionStateIsNew) {
  EXPECT_EQ(pc_->peer_connection_state(), RTCPeerConnectionStateNew);
}

TEST_F(RTCPeerConnectionTest, InitialIceGatheringStateIsNew) {
  EXPECT_EQ(pc_->ice_gathering_state(), RTCIceGatheringStateNew);
}

// --- Stream / data channel ---

TEST_F(RTCPeerConnectionTest, CreateLocalMediaStreamReturnsObject) {
  scoped_refptr<RTCMediaStream> stream = pc_->CreateLocalMediaStream("stream-1");
  EXPECT_NE(stream.get(), nullptr);
}

TEST_F(RTCPeerConnectionTest, CreateDataChannelDefault) {
  RTCDataChannelInit init;
  scoped_refptr<RTCDataChannel> dc = pc_->CreateDataChannel("data", &init);
  EXPECT_NE(dc.get(), nullptr);
}

TEST_F(RTCPeerConnectionTest, CreateDataChannelWithInit) {
  RTCDataChannelInit init;
  init.ordered = true;
  init.maxRetransmits = 3;
  scoped_refptr<RTCDataChannel> dc = pc_->CreateDataChannel("data", &init);
  EXPECT_NE(dc.get(), nullptr);
}

// --- Transceivers / tracks ---

TEST_F(RTCPeerConnectionTest, AddTransceiverAudio) {
  scoped_refptr<RTCRtpTransceiver> transceiver =
      pc_->AddTransceiver(RTCMediaType::AUDIO);
  EXPECT_NE(transceiver.get(), nullptr);
}

TEST_F(RTCPeerConnectionTest, AddTransceiverVideo) {
  scoped_refptr<RTCRtpTransceiver> transceiver =
      pc_->AddTransceiver(RTCMediaType::VIDEO);
  EXPECT_NE(transceiver.get(), nullptr);
}

TEST_F(RTCPeerConnectionTest, AddTransceiverWithTrack) {
  scoped_refptr<RTCAudioTrack> track = CreateAudioTrack("audio-track");
  ASSERT_TRUE(track.get() != nullptr);

  scoped_refptr<RTCRtpTransceiver> transceiver = pc_->AddTransceiver(track);
  EXPECT_NE(transceiver.get(), nullptr);
}

TEST_F(RTCPeerConnectionTest, AddTrackAudioReturnsSender) {
  scoped_refptr<RTCAudioTrack> track = CreateAudioTrack("audio-track");
  ASSERT_TRUE(track.get() != nullptr);

  std::vector<string> stream_ids;
  stream_ids.push_back(string("stream-id"));
  scoped_refptr<RTCRtpSender> sender = pc_->AddTrack(track, stream_ids);
  EXPECT_NE(sender.get(), nullptr);
}

// --- List getters ---

TEST_F(RTCPeerConnectionTest, GetSendersReturnsList) {
  vector<scoped_refptr<RTCRtpSender>> senders = pc_->senders();
  EXPECT_EQ(senders.size(), 0u);

  scoped_refptr<RTCAudioTrack> track = CreateAudioTrack("audio-track");
  ASSERT_TRUE(track.get() != nullptr);
  std::vector<string> stream_ids;
  stream_ids.push_back(string("stream-id"));
  pc_->AddTrack(track, stream_ids);

  senders = pc_->senders();
  EXPECT_EQ(senders.size(), 1u);
}

TEST_F(RTCPeerConnectionTest, GetReceiversReturnsList) {
  vector<scoped_refptr<RTCRtpReceiver>> receivers = pc_->receivers();
  EXPECT_EQ(receivers.size(), 0u);
}

TEST_F(RTCPeerConnectionTest, GetTransceiversReturnsList) {
  vector<scoped_refptr<RTCRtpTransceiver>> transceivers = pc_->transceivers();
  EXPECT_EQ(transceivers.size(), 0u);

  pc_->AddTransceiver(RTCMediaType::AUDIO);
  transceivers = pc_->transceivers();
  EXPECT_EQ(transceivers.size(), 1u);
}

TEST_F(RTCPeerConnectionTest, GetLocalStreamsReturnsList) {
  vector<scoped_refptr<RTCMediaStream>> streams = pc_->local_streams();
  EXPECT_EQ(streams.size(), 0u);
}

TEST_F(RTCPeerConnectionTest, GetRemoteStreamsReturnsList) {
  vector<scoped_refptr<RTCMediaStream>> streams = pc_->remote_streams();
  EXPECT_EQ(streams.size(), 0u);
}

// --- Misc ---

TEST_F(RTCPeerConnectionTest, RestartIceDoesNotCrash) {
  pc_->RestartIce();
}

TEST_F(RTCPeerConnectionTest, AddCandidateAcceptsParameters) {
  // No remote description set; this just verifies the call is accepted.
  pc_->AddCandidate("0", 0, "");
}

// --- Async round trip: CreateOffer + SetLocalDescription ---

TEST_F(RTCPeerConnectionTest, CreateOfferProducesSdp) {
  // Add a transceiver so the offer has at least one m-line.
  scoped_refptr<RTCRtpTransceiver> transceiver =
      pc_->AddTransceiver(RTCMediaType::AUDIO);
  ASSERT_TRUE(transceiver.get() != nullptr);

  scoped_refptr<RTCMediaConstraints> constraints = RTCMediaConstraints::Create();
  ASSERT_TRUE(constraints.get() != nullptr);

  OfferAnswerResult offer;
  pc_->CreateOffer(
      [&offer](const string sdp, const string type) {
        offer.sdp = sdp.std_string();
        offer.type = type.std_string();
        offer.succeeded = true;
        offer.latch.Signal();
      },
      [&offer](const char* error) {
        if (error) offer.error = error;
        offer.succeeded = false;
        offer.latch.Signal();
      },
      constraints);

  ASSERT_TRUE(offer.latch.WaitFor(std::chrono::seconds(5)))
      << "CreateOffer callback was not invoked";
  EXPECT_TRUE(offer.succeeded) << "CreateOffer failed: " << offer.error;
  EXPECT_FALSE(offer.sdp.empty());
  EXPECT_EQ(offer.type, "offer");
}

TEST_F(RTCPeerConnectionTest, SetLocalDescriptionAfterCreateOffer) {
  scoped_refptr<RTCRtpTransceiver> transceiver =
      pc_->AddTransceiver(RTCMediaType::AUDIO);
  ASSERT_TRUE(transceiver.get() != nullptr);

  scoped_refptr<RTCMediaConstraints> constraints = RTCMediaConstraints::Create();
  ASSERT_TRUE(constraints.get() != nullptr);

  OfferAnswerResult offer;
  pc_->CreateOffer(
      [&offer](const string sdp, const string type) {
        offer.sdp = sdp.std_string();
        offer.type = type.std_string();
        offer.succeeded = true;
        offer.latch.Signal();
      },
      [&offer](const char* error) {
        if (error) offer.error = error;
        offer.succeeded = false;
        offer.latch.Signal();
      },
      constraints);
  ASSERT_TRUE(offer.latch.WaitFor(std::chrono::seconds(5)));
  ASSERT_TRUE(offer.succeeded);

  OfferAnswerResult set;
  pc_->SetLocalDescription(
      string(offer.sdp), string(offer.type),
      [&set]() {
        set.succeeded = true;
        set.latch.Signal();
      },
      [&set](const char* error) {
        if (error) set.error = error;
        set.succeeded = false;
        set.latch.Signal();
      });
  ASSERT_TRUE(set.latch.WaitFor(std::chrono::seconds(5)));
  EXPECT_TRUE(set.succeeded) << "SetLocalDescription failed: " << set.error;

  EXPECT_EQ(pc_->signaling_state(), RTCSignalingStateHaveLocalOffer);
}

TEST_F(RTCPeerConnectionTest, NotInitializedAfterClose) {
  EXPECT_TRUE(pc_->IsInitialized());
  pc_->Close();
  // After Close() the peer connection is no longer initialized. The C++ state
  // getters (signaling_state(), etc.) dereference the released native object
  // and must not be called post-close — only the C interop wrapper guards this
  // via IsInitialized(), so the C++-level observable is IsInitialized().
  EXPECT_FALSE(pc_->IsInitialized());
}

}  // namespace libwebrtc
