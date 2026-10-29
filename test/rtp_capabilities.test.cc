#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_rtp_capabilities.h"
#include "rtc_peerconnection_factory.h"
#include "libwebrtc.h"

namespace libwebrtc {

// Exercises the C++ RTCRtpCapabilities API obtained from the peer connection
// factory's GetRtpSenderCapabilities / GetRtpReceiverCapabilities.
class RTCRtpCapabilitiesTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(LibWebRTC::Initialize());
    factory_ = LibWebRTC::CreateRTCPeerConnectionFactory();
    ASSERT_TRUE(factory_.get() != nullptr);
    ASSERT_TRUE(factory_->Initialize());
  }

  void TearDown() override {
    if (factory_) {
      factory_->Terminate();
      factory_ = nullptr;
    }
    LibWebRTC::Terminate();
  }

  // Walks a codec capability list and asserts that the getters return values.
  void CheckCodecList(
      const vector<scoped_refptr<RTCRtpCodecCapability>>& codecs) {
    ASSERT_GT(codecs.size(), 0u);
    for (size_t i = 0; i < codecs.size(); ++i) {
      scoped_refptr<RTCRtpCodecCapability> codec = codecs[i];
      ASSERT_TRUE(codec.get() != nullptr);

      // mime_type should be non-empty for a real codec.
      string mime = codec->mime_type();
      EXPECT_GT(mime.std_string().size(), 0u);

      // clock_rate / channels are simple value getters; just call them.
      int clock_rate = codec->clock_rate();
      EXPECT_GE(clock_rate, 0);

      // channels is the audio channel count; video codecs report -1 (N/A).
      int channels = codec->channels();
      EXPECT_GE(channels, -1);

      // sdp_fmtp_line may be empty but must be retrievable.
      codec->sdp_fmtp_line();
    }
  }

  scoped_refptr<RTCPeerConnectionFactory> factory_;
};

TEST_F(RTCRtpCapabilitiesTest, AudioSenderCapabilitiesExposeCodecs) {
  scoped_refptr<RTCRtpCapabilities> caps =
      factory_->GetRtpSenderCapabilities(RTCMediaType::AUDIO);
  ASSERT_TRUE(caps.get() != nullptr);

  vector<scoped_refptr<RTCRtpCodecCapability>> codecs = caps->codecs();
  CheckCodecList(codecs);

  // At least one audio codec should carry a non-zero clock rate (e.g. opus).
  bool saw_clock_rate = false;
  for (size_t i = 0; i < codecs.size(); ++i) {
    if (codecs[i]->clock_rate() > 0) saw_clock_rate = true;
  }
  EXPECT_TRUE(saw_clock_rate);
}

TEST_F(RTCRtpCapabilitiesTest, VideoSenderCapabilitiesExposeCodecs) {
  scoped_refptr<RTCRtpCapabilities> caps =
      factory_->GetRtpSenderCapabilities(RTCMediaType::VIDEO);
  ASSERT_TRUE(caps.get() != nullptr);

  vector<scoped_refptr<RTCRtpCodecCapability>> codecs = caps->codecs();
  CheckCodecList(codecs);
}

TEST_F(RTCRtpCapabilitiesTest, AudioReceiverCapabilitiesExposeCodecs) {
  scoped_refptr<RTCRtpCapabilities> caps =
      factory_->GetRtpReceiverCapabilities(RTCMediaType::AUDIO);
  ASSERT_TRUE(caps.get() != nullptr);

  vector<scoped_refptr<RTCRtpCodecCapability>> codecs = caps->codecs();
  CheckCodecList(codecs);
}

TEST_F(RTCRtpCapabilitiesTest, VideoReceiverCapabilitiesExposeCodecs) {
  scoped_refptr<RTCRtpCapabilities> caps =
      factory_->GetRtpReceiverCapabilities(RTCMediaType::VIDEO);
  ASSERT_TRUE(caps.get() != nullptr);

  vector<scoped_refptr<RTCRtpCodecCapability>> codecs = caps->codecs();
  CheckCodecList(codecs);
}

TEST_F(RTCRtpCapabilitiesTest, AudioCapabilitiesExposeHeaderExtensions) {
  scoped_refptr<RTCRtpCapabilities> caps =
      factory_->GetRtpSenderCapabilities(RTCMediaType::AUDIO);
  ASSERT_TRUE(caps.get() != nullptr);

  vector<scoped_refptr<RTCRtpHeaderExtensionCapability>> exts =
      caps->header_extensions();
  // Audio should advertise header extensions (e.g. abs-send-time, mid).
  EXPECT_GT(exts.size(), 0u);

  for (size_t i = 0; i < exts.size(); ++i) {
    scoped_refptr<RTCRtpHeaderExtensionCapability> ext = exts[i];
    ASSERT_TRUE(ext.get() != nullptr);

    string uri = ext->uri();
    EXPECT_GT(uri.std_string().size(), 0u);

    // preferred_id / preferred_encrypt are plain getters.
    ext->preferred_id();
    ext->preferred_encrypt();
  }
}

TEST_F(RTCRtpCapabilitiesTest, VideoCapabilitiesExposeHeaderExtensions) {
  scoped_refptr<RTCRtpCapabilities> caps =
      factory_->GetRtpSenderCapabilities(RTCMediaType::VIDEO);
  ASSERT_TRUE(caps.get() != nullptr);

  vector<scoped_refptr<RTCRtpHeaderExtensionCapability>> exts =
      caps->header_extensions();
  EXPECT_GT(exts.size(), 0u);

  for (size_t i = 0; i < exts.size(); ++i) {
    ASSERT_TRUE(exts[i].get() != nullptr);
    exts[i]->uri();
  }
}

}  // namespace libwebrtc
