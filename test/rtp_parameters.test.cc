#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_rtp_parameters.h"
#include "rtc_rtp_sender.h"
#include "rtc_rtp_transceiver.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_mediaconstraints.h"
#include "libwebrtc.h"

namespace libwebrtc {

// Exercises the C++ RTCRtpParameters API obtained from a live sender created
// via pc->AddTransceiver(...) -> sender()->parameters().
class RTCRtpParametersTest : public ::testing::Test {
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

  // Returns a live RTCRtpParameters from an audio sender.
  scoped_refptr<RTCRtpParameters> GetAudioSenderParameters() {
    scoped_refptr<RTCRtpTransceiver> transceiver =
        pc_->AddTransceiver(RTCMediaType::AUDIO);
    EXPECT_TRUE(transceiver.get() != nullptr);
    if (!transceiver) return nullptr;

    scoped_refptr<RTCRtpSender> sender = transceiver->sender();
    EXPECT_TRUE(sender.get() != nullptr);
    if (!sender) return nullptr;

    return sender->parameters();
  }

  scoped_refptr<RTCPeerConnectionFactory> factory_;
  scoped_refptr<RTCPeerConnection> pc_;
};

TEST_F(RTCRtpParametersTest, ExposeTransactionId) {
  scoped_refptr<RTCRtpParameters> params = GetAudioSenderParameters();
  ASSERT_TRUE(params.get() != nullptr);

  // transaction_id is opaque but retrievable; mid likewise.
  params->transaction_id();
  params->mid();
}

TEST_F(RTCRtpParametersTest, ExposeCodecs) {
  scoped_refptr<RTCRtpParameters> params = GetAudioSenderParameters();
  ASSERT_TRUE(params.get() != nullptr);

  vector<scoped_refptr<RTCRtpCodecParameters>> codecs = params->codecs();
  // A sender before negotiation may have an empty codec list; just ensure the
  // accessor and per-codec getters behave when entries exist.
  for (size_t i = 0; i < codecs.size(); ++i) {
    scoped_refptr<RTCRtpCodecParameters> codec = codecs[i];
    ASSERT_TRUE(codec.get() != nullptr);

    codec->mime_type();
    codec->payload_type();
    codec->clock_rate();
    codec->num_channels();

    // Sub-object: rtcp feedback list.
    vector<scoped_refptr<RTCRtcpFeedback>> fb = codec->rtcp_feedback();
    for (size_t j = 0; j < fb.size(); ++j) {
      ASSERT_TRUE(fb[j].get() != nullptr);
      fb[j]->type();
      fb[j]->message_type();
    }

    // Sub-object: codec parameters map (vector of string pairs).
    vector<std::pair<string, string>> kv = codec->parameters();
    EXPECT_GE(kv.size(), 0u);
  }
}

TEST_F(RTCRtpParametersTest, ExposeEncodings) {
  scoped_refptr<RTCRtpParameters> params = GetAudioSenderParameters();
  ASSERT_TRUE(params.get() != nullptr);

  vector<scoped_refptr<RTCRtpEncodingParameters>> encodings =
      params->encodings();
  // A sender typically has at least one default encoding.
  for (size_t i = 0; i < encodings.size(); ++i) {
    scoped_refptr<RTCRtpEncodingParameters> enc = encodings[i];
    ASSERT_TRUE(enc.get() != nullptr);

    enc->active();
    enc->bitrate_priority();
    enc->ssrc();
    enc->rid();
  }
}

TEST_F(RTCRtpParametersTest, ExposeHeaderExtensions) {
  scoped_refptr<RTCRtpParameters> params = GetAudioSenderParameters();
  ASSERT_TRUE(params.get() != nullptr);

  vector<scoped_refptr<RTCRtpExtension>> exts = params->header_extensions();
  for (size_t i = 0; i < exts.size(); ++i) {
    scoped_refptr<RTCRtpExtension> ext = exts[i];
    ASSERT_TRUE(ext.get() != nullptr);

    string uri = ext->uri();
    EXPECT_GT(uri.std_string().size(), 0u);

    ext->id();
    ext->encrypt();
  }
}

TEST_F(RTCRtpParametersTest, ExposeRtcpParameters) {
  scoped_refptr<RTCRtpParameters> params = GetAudioSenderParameters();
  ASSERT_TRUE(params.get() != nullptr);

  scoped_refptr<RTCRtcpParameters> rtcp = params->rtcp_parameters();
  ASSERT_TRUE(rtcp.get() != nullptr);

  rtcp->ssrc();
  rtcp->cname();
  rtcp->reduced_size();
  rtcp->mux();
}

TEST_F(RTCRtpParametersTest, ExposeDegradationPreference) {
  scoped_refptr<RTCRtpParameters> params = GetAudioSenderParameters();
  ASSERT_TRUE(params.get() != nullptr);

  // Simply call the getter; the value is implementation-defined.
  params->GetDegradationPreference();
}

}  // namespace libwebrtc
