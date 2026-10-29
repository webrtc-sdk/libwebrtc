#include <set>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "helper.h"

namespace libwebrtc {

TEST(HelperTest, CreateRandomUuidIsNonEmpty) {
  string uuid = Helper::CreateRandomUuid();
  EXPECT_GT(uuid.size(), 0u);
}

TEST(HelperTest, CreateRandomUuidsAreDistinct) {
  std::set<std::string> seen;
  for (int i = 0; i < 100; ++i) {
    seen.insert(Helper::CreateRandomUuid().std_string());
  }
  // Random UUIDs should essentially never collide across 100 draws.
  EXPECT_EQ(seen.size(), 100u);
}

}  // namespace libwebrtc
