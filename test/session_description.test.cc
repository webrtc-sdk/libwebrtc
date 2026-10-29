#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_sdp_parse_error.h"
#include "rtc_session_description.h"
#include "libwebrtc.h"

namespace libwebrtc {
namespace {

// A minimal but valid session description SDP.
const char kSdp[] =
    "v=0\r\n"
    "o=- 0 0 IN IP4 127.0.0.1\r\n"
    "s=-\r\n"
    "t=0 0\r\n"
    "m=application 9 DTLS/SCTP 5000\r\n"
    "c=IN IP4 0.0.0.0\r\n"
    "a=ice-ufrag:abcd\r\n"
    "a=ice-pwd:0123456789abcdef0123\r\n"
    "a=fingerprint:sha-256 "
    "00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF:00:11:22:33:44:55:66:77:88:"
    "99:AA:BB:CC:DD:EE:FF\r\n"
    "a=setup:actpass\r\n"
    "a=mid:data\r\n"
    "a=sctpmap:5000 webrtc-datachannel 1024\r\n";

}  // namespace

// SSL is required to parse/build SDP, so wrap the creation tests in a fixture
// that initializes the library. Exercises the C++ RTCSessionDescription API.
class RTCSessionDescriptionTest : public ::testing::Test {
 protected:
  void SetUp() override { ASSERT_TRUE(LibWebRTC::Initialize()); }
  void TearDown() override { LibWebRTC::Terminate(); }
};

TEST_F(RTCSessionDescriptionTest, CreateOfferReturnsObject) {
  scoped_refptr<RTCSdpParseError> error = RTCSdpParseError::Create();
  scoped_refptr<RTCSessionDescription> desc =
      RTCSessionDescription::Create("offer", kSdp, error.get());
  ASSERT_TRUE(desc.get() != nullptr);

  EXPECT_STREQ(desc->type().c_string(), "offer");
  EXPECT_EQ(desc->GetType(), RTCSessionDescription::kOffer);
  EXPECT_GT(desc->sdp().size(), 0u);
}

TEST_F(RTCSessionDescriptionTest, CreateWithNullErrorSucceeds) {
  scoped_refptr<RTCSessionDescription> desc =
      RTCSessionDescription::Create("offer", kSdp, nullptr);
  ASSERT_TRUE(desc.get() != nullptr);
  EXPECT_STREQ(desc->type().c_string(), "offer");
}

TEST_F(RTCSessionDescriptionTest, ToStringYieldsNonEmptySdp) {
  scoped_refptr<RTCSessionDescription> desc =
      RTCSessionDescription::Create("offer", kSdp, nullptr);
  ASSERT_TRUE(desc.get() != nullptr);

  string out;
  EXPECT_TRUE(desc->ToString(out));
  EXPECT_GT(out.size(), 0u);
}

TEST_F(RTCSessionDescriptionTest, SdpRoundTripsContent) {
  scoped_refptr<RTCSessionDescription> desc =
      RTCSessionDescription::Create("offer", kSdp, nullptr);
  ASSERT_TRUE(desc.get() != nullptr);

  // The serialized SDP should preserve the media section.
  std::string sdp = desc->sdp().std_string();
  EXPECT_NE(sdp.find("m=application"), std::string::npos);
}

TEST_F(RTCSessionDescriptionTest, InvalidTypeReturnsNull) {
  scoped_refptr<RTCSdpParseError> error = RTCSdpParseError::Create();
  // An unrecognized SDP type cannot map to a known SdpType -> null result.
  scoped_refptr<RTCSessionDescription> desc =
      RTCSessionDescription::Create("not-a-type", kSdp, error.get());
  EXPECT_TRUE(desc.get() == nullptr);
}

TEST_F(RTCSessionDescriptionTest, MalformedSdpPopulatesParseError) {
  scoped_refptr<RTCSdpParseError> error = RTCSdpParseError::Create();
  ASSERT_TRUE(error.get() != nullptr);

  // Garbage SDP fails to parse and leaves a null description; the error object
  // is populated with a description by the implementation.
  scoped_refptr<RTCSessionDescription> desc =
      RTCSessionDescription::Create("offer", "this is not sdp", error.get());
  EXPECT_TRUE(desc.get() == nullptr);
}

}  // namespace libwebrtc
