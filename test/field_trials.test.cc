#include <string>
#include <vector>

#include "libwebrtc.h"
#include "rtc_field_trials.h"
#include "rtc_peerconnection_factory.h"
#include "test/gtest.h"

namespace {

using libwebrtc::LibWebRTC;
using libwebrtc::RTCFieldTrials;
using libwebrtc::scoped_refptr;

// Trial names that no WebRTC code looks up, so that configuring them cannot
// change the behaviour of the other tests running in this binary.
constexpr char kTrialA[] = "WebRTC-LibWebRTC-UnitTestTrialA";
constexpr char kTrialB[] = "WebRTC-LibWebRTC-UnitTestTrialB";
constexpr char kUnconfiguredTrial[] = "WebRTC-LibWebRTC-UnitTestTrialUnknown";

std::string ToStd(const libwebrtc::string& value) { return value.std_string(); }

libwebrtc::vector<libwebrtc::string> Trials(
    const std::vector<std::string>& entries) {
  std::vector<libwebrtc::string> converted;
  converted.reserve(entries.size());
  for (const std::string& entry : entries) {
    converted.push_back(libwebrtc::string(entry));
  }
  return libwebrtc::vector<libwebrtc::string>(converted);
}

// The field trials are process global, so every test starts from a known
// configuration and clears it again afterwards.
class FieldTrialsTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(RTCFieldTrials::InitFieldTrialsFromString(""));
  }

  void TearDown() override {
    EXPECT_TRUE(RTCFieldTrials::InitFieldTrialsFromString(""));
  }
};

TEST_F(FieldTrialsTest, BuildFieldTrialsStringAppendsMissingSeparator) {
  EXPECT_EQ("WebRTC-Foo/Enabled/", ToStd(RTCFieldTrials::BuildFieldTrialsString(
                                       Trials({"WebRTC-Foo/Enabled"}))));
  EXPECT_EQ("WebRTC-Foo/Enabled/", ToStd(RTCFieldTrials::BuildFieldTrialsString(
                                       Trials({"WebRTC-Foo/Enabled/"}))));
}

TEST_F(FieldTrialsTest, BuildFieldTrialsStringConcatenatesEntries) {
  EXPECT_EQ("WebRTC-Foo/Enabled/WebRTC-Bar/Disabled/",
            ToStd(RTCFieldTrials::BuildFieldTrialsString(
                Trials({"WebRTC-Foo/Enabled", "WebRTC-Bar/Disabled/"}))));
}

TEST_F(FieldTrialsTest, BuildFieldTrialsStringSkipsEmptyEntries) {
  EXPECT_EQ("WebRTC-Foo/Enabled/",
            ToStd(RTCFieldTrials::BuildFieldTrialsString(
                Trials({"", "WebRTC-Foo/Enabled", ""}))));
  EXPECT_EQ("", ToStd(RTCFieldTrials::BuildFieldTrialsString(Trials({}))));
}

TEST_F(FieldTrialsTest, InitFieldTrialsFromStringAppliesTrials) {
  const std::string trials =
      std::string(kTrialA) + "/Enabled/" + kTrialB + "/Disabled/";
  ASSERT_TRUE(RTCFieldTrials::InitFieldTrialsFromString(trials));

  EXPECT_EQ(trials, ToStd(RTCFieldTrials::GetFieldTrialsString()));

  EXPECT_EQ("Enabled", ToStd(RTCFieldTrials::Lookup(kTrialA)));
  EXPECT_TRUE(RTCFieldTrials::IsEnabled(kTrialA));
  EXPECT_FALSE(RTCFieldTrials::IsDisabled(kTrialA));

  EXPECT_EQ("Disabled", ToStd(RTCFieldTrials::Lookup(kTrialB)));
  EXPECT_TRUE(RTCFieldTrials::IsDisabled(kTrialB));
  EXPECT_FALSE(RTCFieldTrials::IsEnabled(kTrialB));
}

TEST_F(FieldTrialsTest, UnconfiguredTrialIsNeitherEnabledNorDisabled) {
  ASSERT_TRUE(RTCFieldTrials::InitFieldTrialsFromString(std::string(kTrialA) +
                                                        "/Enabled/"));

  EXPECT_EQ("", ToStd(RTCFieldTrials::Lookup(kUnconfiguredTrial)));
  EXPECT_FALSE(RTCFieldTrials::IsEnabled(kUnconfiguredTrial));
  EXPECT_FALSE(RTCFieldTrials::IsDisabled(kUnconfiguredTrial));
}

TEST_F(FieldTrialsTest, InitFieldTrialsAcceptsEntriesWithoutTrailingSeparator) {
  ASSERT_TRUE(RTCFieldTrials::InitFieldTrials(Trials(
      {std::string(kTrialA) + "/" + libwebrtc::kRTCFieldTrialEnabledValue,
       std::string(kTrialB) + "/" + libwebrtc::kRTCFieldTrialDisabledValue +
           "/"})));

  EXPECT_EQ(std::string(kTrialA) + "/Enabled/" + kTrialB + "/Disabled/",
            ToStd(RTCFieldTrials::GetFieldTrialsString()));
  EXPECT_TRUE(RTCFieldTrials::IsEnabled(kTrialA));
  EXPECT_TRUE(RTCFieldTrials::IsDisabled(kTrialB));
}

TEST_F(FieldTrialsTest, WellKnownKeysAreAccepted) {
  ASSERT_TRUE(RTCFieldTrials::InitFieldTrials(Trials({
      std::string(libwebrtc::kRTCFieldTrialAudioForceABWENoTWCCKey) + "/" +
          libwebrtc::kRTCFieldTrialEnabledValue,
      std::string(libwebrtc::kRTCFieldTrialFlexFec03AdvertisedKey) + "/" +
          libwebrtc::kRTCFieldTrialEnabledValue,
      std::string(libwebrtc::kRTCFieldTrialFlexFec03Key) + "/" +
          libwebrtc::kRTCFieldTrialEnabledValue,
      std::string(libwebrtc::kRTCFieldTrialH264HighProfileKey) + "/" +
          libwebrtc::kRTCFieldTrialEnabledValue,
      std::string(libwebrtc::kRTCFieldTrialMinimizeResamplingOnMobileKey) +
          "/" + libwebrtc::kRTCFieldTrialEnabledValue,
      std::string(libwebrtc::kRTCFieldTrialUseNWPathMonitorKey) + "/" +
          libwebrtc::kRTCFieldTrialEnabledValue,
      std::string(libwebrtc::kRTCFieldTrialIceHandshakeDtlsKey) + "/" +
          libwebrtc::kRTCFieldTrialEnabledValue,
  })));

  EXPECT_TRUE(
      RTCFieldTrials::IsEnabled(libwebrtc::kRTCFieldTrialH264HighProfileKey));
  EXPECT_TRUE(
      RTCFieldTrials::IsEnabled(libwebrtc::kRTCFieldTrialIceHandshakeDtlsKey));
}

TEST_F(FieldTrialsTest, InvalidStringIsRejectedAndKeepsPreviousTrials) {
  const std::string valid = std::string(kTrialA) + "/Enabled/";
  ASSERT_TRUE(RTCFieldTrials::InitFieldTrialsFromString(valid));

  // Missing the trailing separator.
  EXPECT_FALSE(RTCFieldTrials::InitFieldTrialsFromString(std::string(kTrialB) +
                                                         "/Enabled"));
  // Missing the trial name.
  EXPECT_FALSE(RTCFieldTrials::InitFieldTrialsFromString("/Enabled/"));
  // Same trial configured twice with conflicting groups.
  EXPECT_FALSE(RTCFieldTrials::InitFieldTrialsFromString(
      std::string(kTrialB) + "/Enabled/" + kTrialB + "/Disabled/"));

  // None of the rejected strings replaced the configuration in use.
  EXPECT_EQ(valid, ToStd(RTCFieldTrials::GetFieldTrialsString()));
  EXPECT_TRUE(RTCFieldTrials::IsEnabled(kTrialA));
  EXPECT_FALSE(RTCFieldTrials::IsEnabled(kTrialB));
}

TEST_F(FieldTrialsTest, EmptyStringClearsTrials) {
  ASSERT_TRUE(RTCFieldTrials::InitFieldTrialsFromString(std::string(kTrialA) +
                                                        "/Enabled/"));
  ASSERT_TRUE(RTCFieldTrials::IsEnabled(kTrialA));

  EXPECT_TRUE(RTCFieldTrials::InitFieldTrialsFromString(""));
  EXPECT_EQ("", ToStd(RTCFieldTrials::GetFieldTrialsString()));
  EXPECT_EQ("", ToStd(RTCFieldTrials::Lookup(kTrialA)));
  EXPECT_FALSE(RTCFieldTrials::IsEnabled(kTrialA));
}

TEST_F(FieldTrialsTest, ReinitializationReplacesPreviousTrials) {
  ASSERT_TRUE(RTCFieldTrials::InitFieldTrialsFromString(std::string(kTrialA) +
                                                        "/Enabled/"));
  ASSERT_TRUE(RTCFieldTrials::InitFieldTrialsFromString(std::string(kTrialB) +
                                                        "/Enabled/"));

  EXPECT_TRUE(RTCFieldTrials::IsEnabled(kTrialB));
  EXPECT_FALSE(RTCFieldTrials::IsEnabled(kTrialA));
}

// The field trials have to be in place before the PeerConnectionFactory is
// built, since that is when they are baked into the webrtc::Environment.
TEST_F(FieldTrialsTest, InitializeWithFieldTrialsAppliesTrialsBeforeFactory) {
  LibWebRTC::InitializeWithFieldTrials(Trials(
      {std::string(kTrialA) + "/" + libwebrtc::kRTCFieldTrialEnabledValue}));

  EXPECT_TRUE(RTCFieldTrials::IsEnabled(kTrialA));

  scoped_refptr<libwebrtc::RTCPeerConnectionFactory> factory =
      LibWebRTC::CreateRTCPeerConnectionFactory();
  ASSERT_TRUE(factory.get() != nullptr);
  ASSERT_TRUE(factory->Initialize());

  EXPECT_TRUE(RTCFieldTrials::IsEnabled(kTrialA));

  factory->Terminate();
  factory = nullptr;
  LibWebRTC::Terminate();
}

}  // namespace
