#include <map>
#include <string>
#include <vector>

#include "test/gmock.h"
#include "test/gtest.h"

#include "base/portable.h"

namespace libwebrtc {
namespace {

// Intrusive-refcount helper for counting_ptr.
struct CObj : public portable::counted_object {
  static int alive;
  CObj() { ++alive; }
  ~CObj() override { --alive; }
  int v = 0;
};
int CObj::alive = 0;

struct LObj {
  static int alive;
  LObj() { ++alive; }
  ~LObj() { --alive; }
};
int LObj::alive = 0;

void DestroyLObj(LObj* p) { delete p; }

}  // namespace

// --- portable::string ---

TEST(PortableStringTest, ConstructFromCStr) {
  portable::string s("hello");
  EXPECT_EQ(s.size(), 5u);
  EXPECT_STREQ(s.c_string(), "hello");
  EXPECT_EQ(s.std_string(), std::string("hello"));
}

TEST(PortableStringTest, ConstructFromStdString) {
  portable::string s(std::string("world"));
  EXPECT_EQ(s.std_string(), std::string("world"));
}

TEST(PortableStringTest, NullCStrYieldsEmpty) {
  portable::string s(static_cast<const char*>(nullptr));
  EXPECT_EQ(s.size(), 0u);
}

TEST(PortableStringTest, LongStringUsesDynamicStorage) {
  // Longer than PORTABLE_STRING_BUF_SIZE (48) to force the dynamic buffer.
  std::string long_str(100, 'x');
  portable::string s(long_str);
  EXPECT_EQ(s.size(), 100u);
  EXPECT_EQ(s.std_string(), long_str);
}

TEST(PortableStringTest, CopyConstructAndAssign) {
  portable::string a("abc");
  portable::string b(a);
  EXPECT_EQ(b.std_string(), std::string("abc"));

  portable::string c;
  c = a;
  EXPECT_EQ(c.std_string(), std::string("abc"));

  c = std::string("xyz");
  EXPECT_EQ(c.std_string(), std::string("xyz"));
}

TEST(PortableStringTest, CopyToRespectsBufferSize) {
  portable::string s("abcdef");
  char buf[4] = {};
  size_t n = s.copy_to(buf, sizeof(buf));
  EXPECT_EQ(n, 3u);  // sz_dest - 1
  EXPECT_STREQ(buf, "abc");

  EXPECT_EQ(s.copy_to(nullptr, 10), 0u);
}

TEST(PortableStringTest, Split) {
  portable::string s("a,b,c");
  std::vector<portable::string> parts = s.split(portable::string(","));
  ASSERT_EQ(parts.size(), 3u);
  EXPECT_EQ(parts[0].std_string(), std::string("a"));
  EXPECT_EQ(parts[1].std_string(), std::string("b"));
  EXPECT_EQ(parts[2].std_string(), std::string("c"));
}

TEST(PortableStringTest, SplitRemovesEmptyEntriesByDefault) {
  portable::string s("a,,c");
  std::vector<portable::string> parts = s.split(portable::string(","));
  ASSERT_EQ(parts.size(), 2u);
  EXPECT_EQ(parts[0].std_string(), std::string("a"));
  EXPECT_EQ(parts[1].std_string(), std::string("c"));
}

TEST(PortableStringTest, Join) {
  std::vector<portable::string> src;
  src.push_back(portable::string("a"));
  src.push_back(portable::string("b"));
  src.push_back(portable::string("c"));
  portable::vector<portable::string> v(src);
  EXPECT_EQ(portable::string::join(portable::string("-"), v), std::string("a-b-c"));
}

TEST(PortableStringTest, ToStdStringHelper) {
  portable::string s("zzz");
  EXPECT_EQ(portable::to_std_string(s), std::string("zzz"));
}

// --- portable::vector ---

TEST(PortableVectorTest, DefaultIsEmpty) {
  portable::vector<int> v;
  EXPECT_EQ(v.size(), 0u);
  EXPECT_EQ(v.data(), nullptr);
}

TEST(PortableVectorTest, ConstructFromStdVector) {
  std::vector<int> src{1, 2, 3};
  portable::vector<int> v(src);
  ASSERT_EQ(v.size(), 3u);
  EXPECT_EQ(v[0], 1);
  EXPECT_EQ(v[2], 3);
}

TEST(PortableVectorTest, RoundTripsToStdVector) {
  std::vector<int> src{4, 5, 6};
  portable::vector<int> v(src);
  EXPECT_EQ(v.std_vector(), src);
}

TEST(PortableVectorTest, ConvertingConstructor) {
  std::vector<int> src{1, 2, 3};
  portable::vector<int> v(src, [](int x) { return x * 10; });
  ASSERT_EQ(v.size(), 3u);
  EXPECT_EQ(v[0], 10);
  EXPECT_EQ(v[2], 30);
}

TEST(PortableVectorTest, CopyAndClear) {
  std::vector<int> src{7, 8};
  portable::vector<int> v(src);
  portable::vector<int> c(v);
  ASSERT_EQ(c.size(), 2u);
  EXPECT_EQ(c[1], 8);

  v.clear();
  EXPECT_EQ(v.size(), 0u);
}

TEST(PortableVectorTest, MoveAssignmentTransfersOwnership) {
  // move() is only safe via assignment, per the header's own note ("can't be
  // used as vector(something).move()") — the move constructor does not reset
  // the source. Use the documented assignment form.
  std::vector<int> src{1, 2};
  portable::vector<int> tmp(src);
  portable::vector<int> moved;
  moved = tmp.move();
  EXPECT_EQ(moved.size(), 2u);
  EXPECT_EQ(moved[1], 2);
  EXPECT_EQ(tmp.size(), 0u);  // source emptied.
}

// --- portable::pair / portable::map ---

TEST(PortablePairTest, HoldsKeyAndValue) {
  portable::pair<int, std::string> p;
  p.key = 7;
  p.value = "seven";
  EXPECT_EQ(p.key, 7);
  EXPECT_EQ(p.value, std::string("seven"));
}

TEST(PortableMapTest, DefaultIsEmpty) {
  portable::map<int, int> m;
  EXPECT_EQ(m.size(), 0u);
}

// --- portable::counting_ptr ---

TEST(PortableCountingPtrTest, SharesAndDeletesOnce) {
  CObj::alive = 0;
  {
    portable::counting_ptr<CObj> p(new CObj());
    EXPECT_EQ(CObj::alive, 1);
    p->v = 5;
    {
      portable::counting_ptr<CObj> q = p;
      EXPECT_EQ(q->v, 5);
      q->v = 9;
      EXPECT_EQ(p->v, 9);  // same object.
      EXPECT_EQ(CObj::alive, 1);
    }
    EXPECT_EQ(CObj::alive, 1);  // q released, p still holds.
  }
  EXPECT_EQ(CObj::alive, 0);  // deleted exactly once.
}

// --- portable::local_ptr ---

TEST(PortableLocalPtrTest, DestroysOnScopeExit) {
  LObj::alive = 0;
  {
    portable::local_ptr<LObj> lp;
    lp.set(new LObj(), &DestroyLObj);
    EXPECT_EQ(LObj::alive, 1);
    EXPECT_NE(lp.get(), nullptr);
  }
  EXPECT_EQ(LObj::alive, 0);
}

}  // namespace libwebrtc
