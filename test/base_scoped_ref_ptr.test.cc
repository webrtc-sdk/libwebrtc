#include <utility>

#include "test/gmock.h"
#include "test/gtest.h"

#include "base/scoped_ref_ptr.h"

namespace libwebrtc {
namespace {

// Minimal reference-counted type compatible with scoped_refptr. It self-deletes
// when the count hits zero and bumps *destroyed so tests can observe lifetime.
class Counted {
 public:
  explicit Counted(int* destroyed) : destroyed_(destroyed) {}

  int AddRef() const { return ++ref_count_; }
  int Release() const {
    int count = --ref_count_;
    if (count == 0) {
      if (destroyed_) ++(*destroyed_);
      delete this;
    }
    return count;
  }
  int ref_count() const { return ref_count_; }

  int payload = 0;

 private:
  ~Counted() = default;  // only Release() may destroy.
  mutable int ref_count_ = 0;
  int* destroyed_;
};

}  // namespace

TEST(ScopedRefptrTest, DefaultIsNull) {
  scoped_refptr<Counted> r;
  EXPECT_EQ(r.get(), nullptr);
  EXPECT_FALSE(r);
}

TEST(ScopedRefptrTest, ConstructFromRawAddRefsAndReleasesOnScopeExit) {
  int destroyed = 0;
  {
    scoped_refptr<Counted> r(new Counted(&destroyed));
    EXPECT_EQ(r->ref_count(), 1);
    EXPECT_TRUE(r);
    EXPECT_NE(r.get(), nullptr);
  }
  EXPECT_EQ(destroyed, 1);
}

TEST(ScopedRefptrTest, CopyConstructorSharesOwnership) {
  int destroyed = 0;
  {
    scoped_refptr<Counted> a(new Counted(&destroyed));
    {
      scoped_refptr<Counted> b(a);
      EXPECT_EQ(a->ref_count(), 2);
      EXPECT_EQ(b.get(), a.get());
    }
    EXPECT_EQ(a->ref_count(), 1);  // b released its reference.
    EXPECT_EQ(destroyed, 0);
  }
  EXPECT_EQ(destroyed, 1);
}

TEST(ScopedRefptrTest, CopyAssignmentReleasesPrevious) {
  int d1 = 0, d2 = 0;
  scoped_refptr<Counted> a(new Counted(&d1));
  scoped_refptr<Counted> b(new Counted(&d2));
  b = a;  // b drops its old object (d2) and shares a's.
  EXPECT_EQ(d2, 1);
  EXPECT_EQ(a->ref_count(), 2);
  EXPECT_EQ(b.get(), a.get());
}

TEST(ScopedRefptrTest, RawAssignmentAndSelfAssignmentAreSafe) {
  int destroyed = 0;
  scoped_refptr<Counted> r(new Counted(&destroyed));
  Counted* raw = r.get();
  r = raw;  // AddRef before Release => count stays 1.
  EXPECT_EQ(r->ref_count(), 1);
  EXPECT_EQ(destroyed, 0);
}

TEST(ScopedRefptrTest, ReleaseRelinquishesWithoutDecrementing) {
  int destroyed = 0;
  Counted* raw = nullptr;
  {
    scoped_refptr<Counted> r(new Counted(&destroyed));
    raw = r.release();
    EXPECT_EQ(r.get(), nullptr);
  }
  EXPECT_EQ(destroyed, 0);     // scoped_refptr no longer owned it.
  EXPECT_EQ(raw->ref_count(), 1);
  EXPECT_EQ(raw->Release(), 0);  // manual cleanup.
  EXPECT_EQ(destroyed, 1);
}

TEST(ScopedRefptrTest, MoveConstructorTransfersWithoutRefChurn) {
  int destroyed = 0;
  scoped_refptr<Counted> a(new Counted(&destroyed));
  scoped_refptr<Counted> b(std::move(a));
  EXPECT_EQ(a.get(), nullptr);
  EXPECT_EQ(b->ref_count(), 1);
  EXPECT_EQ(destroyed, 0);
}

TEST(ScopedRefptrTest, SwapExchangesPointers) {
  int d1 = 0, d2 = 0;
  scoped_refptr<Counted> a(new Counted(&d1));
  scoped_refptr<Counted> b(new Counted(&d2));
  Counted* pa = a.get();
  Counted* pb = b.get();
  a.swap(b);
  EXPECT_EQ(a.get(), pb);
  EXPECT_EQ(b.get(), pa);
}

TEST(ScopedRefptrTest, MemberAccess) {
  int destroyed = 0;
  scoped_refptr<Counted> r(new Counted(&destroyed));
  r->payload = 99;
  EXPECT_EQ(r->payload, 99);
  EXPECT_EQ(r.get()->payload, 99);
}

}  // namespace libwebrtc
