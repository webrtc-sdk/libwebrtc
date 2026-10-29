#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_mediaconstraints.h"
#include "libwebrtc.h"

namespace libwebrtc {

TEST(RTCMediaConstraintsTest, CreateReturnsInstance) {
  scoped_refptr<RTCMediaConstraints> constraints = RTCMediaConstraints::Create();
  EXPECT_NE(constraints.get(), nullptr);
}

TEST(RTCMediaConstraintsTest, AddMandatoryConstraint) {
  scoped_refptr<RTCMediaConstraints> constraints = RTCMediaConstraints::Create();
  ASSERT_TRUE(constraints.get() != nullptr);

  // Should not crash; these are the standard W3C offer/answer keys.
  constraints->AddMandatoryConstraint(RTCMediaConstraints::kOfferToReceiveAudio,
                                      RTCMediaConstraints::kValueTrue);
  constraints->AddMandatoryConstraint(RTCMediaConstraints::kOfferToReceiveVideo,
                                      RTCMediaConstraints::kValueFalse);
}

TEST(RTCMediaConstraintsTest, AddOptionalConstraint) {
  scoped_refptr<RTCMediaConstraints> constraints = RTCMediaConstraints::Create();
  ASSERT_TRUE(constraints.get() != nullptr);

  constraints->AddOptionalConstraint(RTCMediaConstraints::kIceRestart,
                                     RTCMediaConstraints::kValueTrue);
  constraints->AddOptionalConstraint(RTCMediaConstraints::kEnableDscp,
                                     RTCMediaConstraints::kValueFalse);
}

TEST(RTCMediaConstraintsTest, AddMandatoryAndOptionalTogether) {
  scoped_refptr<RTCMediaConstraints> constraints = RTCMediaConstraints::Create();
  ASSERT_TRUE(constraints.get() != nullptr);

  constraints->AddMandatoryConstraint(RTCMediaConstraints::kOfferToReceiveAudio,
                                      RTCMediaConstraints::kValueTrue);
  constraints->AddOptionalConstraint(RTCMediaConstraints::kEnableIPv6,
                                     RTCMediaConstraints::kValueTrue);
}

}  // namespace libwebrtc
