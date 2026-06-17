#include <utility>

#include "test/gmock.h"
#include "test/gtest.h"

#include "base/fixed_size_function.h"

// fixed_size_function and construct_type live in the global namespace.
//
// Note: this vendored type's operator() falls back to `(Ret)nullptr` for the
// empty case, so it is only usable with void (or pointer) return types — which
// is how the codebase uses it (void callback signatures). The tests below use
// void signatures and observe results through captured state.
namespace libwebrtc {

TEST(FixedSizeFunctionTest, DefaultIsEmpty) {
  fixed_size_function<void(int)> f;
  EXPECT_FALSE(static_cast<bool>(f));
  EXPECT_TRUE(f == nullptr);
  // operator!=(nullptr) is not exercised: the vendored header's `return f;`
  // relies on an implicit bool conversion, but operator bool is explicit, so
  // it does not compile when instantiated.
}

TEST(FixedSizeFunctionTest, InvokesStatelessLambda) {
  int out = 0;
  fixed_size_function<void(int)> f = [&out](int x) { out = x * 2; };
  EXPECT_TRUE(static_cast<bool>(f));
  f(21);
  EXPECT_EQ(out, 42);
}

TEST(FixedSizeFunctionTest, CapturesState) {
  int base = 10;
  int out = 0;
  fixed_size_function<void(int)> f = [base, &out](int x) { out = base + x; };
  f(5);
  EXPECT_EQ(out, 15);
}

TEST(FixedSizeFunctionTest, AssignFromNullptrResets) {
  fixed_size_function<void()> f = [] {};
  EXPECT_TRUE(static_cast<bool>(f));
  f = nullptr;
  EXPECT_FALSE(static_cast<bool>(f));
}

TEST(FixedSizeFunctionTest, ResetClearsTarget) {
  fixed_size_function<void()> f = [] {};
  f.reset();
  EXPECT_FALSE(static_cast<bool>(f));
}

TEST(FixedSizeFunctionTest, CopyPreservesBothCallables) {
  int count = 0;
  fixed_size_function<void()> f = [&count] { ++count; };
  fixed_size_function<void()> g = f;
  f();
  g();
  EXPECT_EQ(count, 2);
}

TEST(FixedSizeFunctionTest, MoveTransfersAndResetsSource) {
  int out = 0;
  fixed_size_function<void(int)> f = [&out](int x) { out = x + 1; };
  fixed_size_function<void(int)> g = std::move(f);
  g(1);
  EXPECT_EQ(out, 2);
  EXPECT_FALSE(static_cast<bool>(f));  // moved-from is reset.
}

TEST(FixedSizeFunctionTest, SwapExchangesTargets) {
  int out = 0;
  fixed_size_function<void(int)> a = [&out](int x) { out = x + 1; };
  fixed_size_function<void(int)> b = [&out](int x) { out = x * 10; };
  a.swap(b);
  a(2);
  EXPECT_EQ(out, 20);  // a now runs b's body.
  b(2);
  EXPECT_EQ(out, 3);  // b now runs a's body.
}

TEST(FixedSizeFunctionTest, RunsSideEffectsEachCall) {
  int counter = 0;
  fixed_size_function<void()> f = [&counter] { ++counter; };
  f();
  f();
  EXPECT_EQ(counter, 2);
}

TEST(FixedSizeFunctionTest, ReassignReplacesTarget) {
  int out = 0;
  fixed_size_function<void(int)> f = [&out](int x) { out = x; };
  f(3);
  EXPECT_EQ(out, 3);
  f = [&out](int x) { out = x * x; };
  f(3);
  EXPECT_EQ(out, 9);
}

}  // namespace libwebrtc
