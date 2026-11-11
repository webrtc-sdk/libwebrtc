#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_ice_candidate.h"
#include "rtc_sdp_parse_error.h"
#include "libwebrtc.h"

namespace libwebrtc {
namespace {

// A valid ICE candidate SDP attribute line.
const char kCandidate[] =
    "candidate:1 1 UDP 2130706431 192.168.1.1 50000 typ host";

}  // namespace

// SSL is required to parse candidate strings, so initialize the library.
// Exercises the C++ RTCIceCandidate API.
class RTCIceCandidateTest : public ::testing::Test {
 protected:
  void SetUp() override { ASSERT_TRUE(LibWebRTC::Initialize()); }
  void TearDown() override { LibWebRTC::Terminate(); }
};

TEST_F(RTCIceCandidateTest, CreateReturnsObject) {
  scoped_refptr<RTCSdpParseError> error = RTCSdpParseError::Create();
  scoped_refptr<RTCIceCandidate> candidate =
      RTCIceCandidate::Create(kCandidate, "audio", 0, error.get());
  ASSERT_TRUE(candidate.get() != nullptr);

  EXPECT_STREQ(candidate->sdp_mid().c_string(), "audio");
  EXPECT_EQ(candidate->sdp_mline_index(), 0);
  EXPECT_GT(candidate->candidate().size(), 0u);
}

TEST_F(RTCIceCandidateTest, CreateWithNullErrorSucceeds) {
  scoped_refptr<RTCIceCandidate> candidate =
      RTCIceCandidate::Create(kCandidate, "video", 1, nullptr);
  ASSERT_TRUE(candidate.get() != nullptr);

  EXPECT_STREQ(candidate->sdp_mid().c_string(), "video");
  EXPECT_EQ(candidate->sdp_mline_index(), 1);
}

TEST_F(RTCIceCandidateTest, CandidatePreservesHostType) {
  scoped_refptr<RTCIceCandidate> candidate =
      RTCIceCandidate::Create(kCandidate, "audio", 0, nullptr);
  ASSERT_TRUE(candidate.get() != nullptr);

  // The serialized candidate retains the host candidate marker.
  std::string cand = candidate->candidate().std_string();
  EXPECT_NE(cand.find("typ host"), std::string::npos);
}

TEST_F(RTCIceCandidateTest, ToStringYieldsNonEmpty) {
  scoped_refptr<RTCIceCandidate> candidate =
      RTCIceCandidate::Create(kCandidate, "audio", 0, nullptr);
  ASSERT_TRUE(candidate.get() != nullptr);

  string out;
  EXPECT_TRUE(candidate->ToString(out));
  EXPECT_GT(out.size(), 0u);
}

TEST_F(RTCIceCandidateTest, MalformedCandidateReturnsNull) {
  scoped_refptr<RTCSdpParseError> error = RTCSdpParseError::Create();
  ASSERT_TRUE(error.get() != nullptr);

  // A non-candidate string fails to parse and yields a null result.
  scoped_refptr<RTCIceCandidate> candidate =
      RTCIceCandidate::Create("not a candidate", "audio", 0, error.get());
  EXPECT_TRUE(candidate.get() == nullptr);
}

}  // namespace libwebrtc
