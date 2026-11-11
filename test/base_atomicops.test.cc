#include "test/gmock.h"
#include "test/gtest.h"

// A bare `#include "base/atomicops.h"` resolves to WebRTC's own
// base/atomicops.h on the build include path, and the GN flags expose
// .../libwebrtc/include (not .../libwebrtc), so an "include/"-prefixed path is
// not found either. base/refcountedobject.h is a libwebrtc-only base header
// that relative-includes the correct base/atomicops.h (libwebrtc::AtomicOps).
#include "base/refcountedobject.h"

namespace libwebrtc {

TEST(AtomicOpsTest, IncrementReturnsNewValue) {
  volatile int v = 0;
  EXPECT_EQ(AtomicOps::Increment(&v), 1);
  EXPECT_EQ(AtomicOps::Increment(&v), 2);
  EXPECT_EQ(v, 2);
}

TEST(AtomicOpsTest, DecrementReturnsNewValue) {
  volatile int v = 2;
  EXPECT_EQ(AtomicOps::Decrement(&v), 1);
  EXPECT_EQ(AtomicOps::Decrement(&v), 0);
  EXPECT_EQ(v, 0);
}

TEST(AtomicOpsTest, AcquireLoadReleaseStore) {
  volatile int v = 0;
  AtomicOps::ReleaseStore(&v, 42);
  EXPECT_EQ(AtomicOps::AcquireLoad(&v), 42);
}

TEST(AtomicOpsTest, CompareAndSwapSwapsOnMatch) {
  volatile int v = 5;
  // old value matches -> swap happens, previous value returned.
  EXPECT_EQ(AtomicOps::CompareAndSwap(&v, 5, 9), 5);
  EXPECT_EQ(AtomicOps::AcquireLoad(&v), 9);
}

TEST(AtomicOpsTest, CompareAndSwapNoOpOnMismatch) {
  volatile int v = 9;
  // old value does not match -> no swap, current value returned.
  EXPECT_EQ(AtomicOps::CompareAndSwap(&v, 5, 1), 9);
  EXPECT_EQ(AtomicOps::AcquireLoad(&v), 9);
}

TEST(AtomicOpsTest, PointerLoadAndCompareAndSwap) {
  int a = 1, b = 2;
  int* volatile p = &a;
  EXPECT_EQ(AtomicOps::AcquireLoadPtr(&p), &a);

  // Swap a -> b when current matches &a.
  EXPECT_EQ(AtomicOps::CompareAndSwapPtr(&p, &a, &b), &a);
  EXPECT_EQ(AtomicOps::AcquireLoadPtr(&p), &b);

  // No swap when current no longer matches &a.
  EXPECT_EQ(AtomicOps::CompareAndSwapPtr(&p, &a, &a), &b);
  EXPECT_EQ(AtomicOps::AcquireLoadPtr(&p), &b);
}

}  // namespace libwebrtc
