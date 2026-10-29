#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_sdp_parse_error.h"
#include "libwebrtc.h"

namespace libwebrtc {

// RTCSdpParseError is a plain ref-counted value object created via the static
// Create() factory; it does not require library initialization.

TEST(RTCSdpParseErrorTest, CreateReturnsObject) {
  scoped_refptr<RTCSdpParseError> error = RTCSdpParseError::Create();
  ASSERT_TRUE(error.get() != nullptr);
}

TEST(RTCSdpParseErrorTest, DefaultsAreEmpty) {
  scoped_refptr<RTCSdpParseError> error = RTCSdpParseError::Create();
  ASSERT_TRUE(error.get() != nullptr);

  EXPECT_EQ(error->line().std_string(), "");
  EXPECT_EQ(error->description().std_string(), "");
}

TEST(RTCSdpParseErrorTest, SetAndGetLine) {
  scoped_refptr<RTCSdpParseError> error = RTCSdpParseError::Create();
  ASSERT_TRUE(error.get() != nullptr);

  error->set_line("m=audio 9 UDP/TLS/RTP/SAVPF 111");
  EXPECT_STREQ(error->line().c_string(), "m=audio 9 UDP/TLS/RTP/SAVPF 111");
}

TEST(RTCSdpParseErrorTest, SetAndGetDescription) {
  scoped_refptr<RTCSdpParseError> error = RTCSdpParseError::Create();
  ASSERT_TRUE(error.get() != nullptr);

  error->set_description("Invalid media line");
  EXPECT_STREQ(error->description().c_string(), "Invalid media line");
}

TEST(RTCSdpParseErrorTest, FieldsAreIndependent) {
  scoped_refptr<RTCSdpParseError> error = RTCSdpParseError::Create();
  ASSERT_TRUE(error.get() != nullptr);

  error->set_line("a=bad");
  error->set_description("parse failed");
  EXPECT_STREQ(error->line().c_string(), "a=bad");
  EXPECT_STREQ(error->description().c_string(), "parse failed");

  // Overwriting one field leaves the other intact.
  error->set_line("a=worse");
  EXPECT_STREQ(error->line().c_string(), "a=worse");
  EXPECT_STREQ(error->description().c_string(), "parse failed");
}

}  // namespace libwebrtc
