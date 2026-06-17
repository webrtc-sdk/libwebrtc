#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_data_channel.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_mediaconstraints.h"
#include "libwebrtc.h"

namespace libwebrtc {

// Fixture that owns a fresh peer connection for each test, exercising the C++
// RTCDataChannel class API (pc_->CreateDataChannel).
class RTCDataChannelTest : public ::testing::Test {
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

  scoped_refptr<RTCDataChannel> CreateDataChannel(const char* label,
                                                  RTCDataChannelInit* init) {
    // The C++ API requires a non-null RTCDataChannelInit (it is dereferenced
    // unconditionally); supply a default when the caller passes nullptr.
    RTCDataChannelInit default_init;
    return pc_->CreateDataChannel(label, init ? init : &default_init);
  }

  scoped_refptr<RTCPeerConnectionFactory> factory_;
  scoped_refptr<RTCPeerConnection> pc_;
};

TEST_F(RTCDataChannelTest, CreateDefaultReturnsObject) {
  scoped_refptr<RTCDataChannel> dc = CreateDataChannel("chat", nullptr);
  ASSERT_TRUE(dc.get() != nullptr);
}

TEST_F(RTCDataChannelTest, LabelMatchesCreated) {
  scoped_refptr<RTCDataChannel> dc = CreateDataChannel("chat", nullptr);
  ASSERT_TRUE(dc.get() != nullptr);
  EXPECT_STREQ(dc->label().c_string(), "chat");
}

TEST_F(RTCDataChannelTest, IdIsAccessible) {
  scoped_refptr<RTCDataChannel> dc = CreateDataChannel("chat", nullptr);
  ASSERT_TRUE(dc.get() != nullptr);
  // id() returns a valid integer (may be -1 until negotiated).
  int id = dc->id();
  EXPECT_GE(id, -1);
}

TEST_F(RTCDataChannelTest, StateStartsConnecting) {
  scoped_refptr<RTCDataChannel> dc = CreateDataChannel("chat", nullptr);
  ASSERT_TRUE(dc.get() != nullptr);
  // Before any negotiation the channel is in the connecting state.
  EXPECT_EQ(dc->state(), RTCDataChannelConnecting);
}

TEST_F(RTCDataChannelTest, BufferedAmountInitiallyZero) {
  scoped_refptr<RTCDataChannel> dc = CreateDataChannel("chat", nullptr);
  ASSERT_TRUE(dc.get() != nullptr);
  EXPECT_EQ(dc->buffered_amount(), 0u);
}

TEST_F(RTCDataChannelTest, CreateWithInitGettersMatch) {
  RTCDataChannelInit init;
  init.ordered = true;
  init.maxRetransmits = 3;
  init.protocol = string("sctp");

  scoped_refptr<RTCDataChannel> dc = CreateDataChannel("reliable", &init);
  ASSERT_TRUE(dc.get() != nullptr);
  EXPECT_STREQ(dc->label().c_string(), "reliable");
  EXPECT_EQ(dc->state(), RTCDataChannelConnecting);
}

TEST_F(RTCDataChannelTest, CreateNegotiatedWithIdMatchesId) {
  RTCDataChannelInit init;
  init.negotiated = true;
  init.id = 7;

  scoped_refptr<RTCDataChannel> dc = CreateDataChannel("negotiated", &init);
  ASSERT_TRUE(dc.get() != nullptr);
  // A negotiated channel carries the explicitly requested id.
  EXPECT_EQ(dc->id(), 7);
}

// Minimal observer used to confirm RegisterObserver/UnregisterObserver work.
class CountingObserver : public RTCDataChannelObserver {
 public:
  void OnStateChange(RTCDataChannelState state) override { ++state_changes_; }
  void OnMessage(const char* buffer, int length, bool binary) override {
    ++messages_;
  }
  int state_changes_ = 0;
  int messages_ = 0;
};

TEST_F(RTCDataChannelTest, RegisterAndUnregisterObserver) {
  scoped_refptr<RTCDataChannel> dc = CreateDataChannel("chat", nullptr);
  ASSERT_TRUE(dc.get() != nullptr);

  CountingObserver observer;
  dc->RegisterObserver(&observer);
  dc->UnregisterObserver();
  // No crash and the channel remains usable.
  EXPECT_STREQ(dc->label().c_string(), "chat");
}

TEST_F(RTCDataChannelTest, SendBeforeOpenDoesNotCrash) {
  scoped_refptr<RTCDataChannel> dc = CreateDataChannel("chat", nullptr);
  ASSERT_TRUE(dc.get() != nullptr);

  const uint8_t data[] = {'p', 'i', 'n', 'g'};
  dc->Send(data, sizeof(data), /*binary=*/true);
  // Sending before the channel opens is a no-op rather than a crash.
  EXPECT_TRUE(dc.get() != nullptr);
}

TEST_F(RTCDataChannelTest, CloseIsCallable) {
  scoped_refptr<RTCDataChannel> dc = CreateDataChannel("chat", nullptr);
  ASSERT_TRUE(dc.get() != nullptr);

  // A freshly created channel (no negotiated SCTP transport) starts connecting.
  EXPECT_EQ(dc->state(), RTCDataChannelConnecting);

  // Close() is asynchronous: the Closing/Closed transition is delivered later
  // through the observer, so we only assert it is callable without crashing.
  dc->Close();
}

}  // namespace libwebrtc
