#include <memory>

#include "test/gmock.h"
#include "test/gtest.h"

#include "base/refcount.h"
#include "base/refcountedobject.h"
#include "base/scoped_ref_ptr.h"

namespace libwebrtc {
namespace {

// Plain class used to exercise RefCountedObject's constructor forwarding.
class Widget {
 public:
  Widget() : value(0) {}
  explicit Widget(int v) : value(v) {}
  Widget(int a, int b) : value(a + b) {}
  virtual ~Widget() {}
  int value;
};

// Interface + impl to exercise RefCountInterface dispatch and destruction.
class IThing : public RefCountInterface {
 public:
  virtual int value() const = 0;
};

class ThingImpl : public IThing {
 public:
  ThingImpl(int v, bool* destroyed) : v_(v), destroyed_(destroyed) {}
  int value() const override { return v_; }
  ~ThingImpl() override {
    if (destroyed_) *destroyed_ = true;
  }

 private:
  int v_;
  bool* destroyed_;
};

}  // namespace

TEST(RefCountedObjectTest, DefaultConstruct) {
  RefCountedObject<Widget>* w = new RefCountedObject<Widget>();
  EXPECT_EQ(w->value, 0);
  EXPECT_EQ(w->AddRef(), 1);
  EXPECT_EQ(w->Release(), 0);  // deletes w
}

TEST(RefCountedObjectTest, ForwardsSingleArg) {
  RefCountedObject<Widget>* w = new RefCountedObject<Widget>(7);
  EXPECT_EQ(w->value, 7);
  w->AddRef();
  w->Release();
}

TEST(RefCountedObjectTest, ForwardsMultipleArgs) {
  RefCountedObject<Widget>* w = new RefCountedObject<Widget>(3, 4);
  EXPECT_EQ(w->value, 7);
  w->AddRef();
  w->Release();
}

TEST(RefCountedObjectTest, AddRefReleaseAndHasOneRef) {
  RefCountedObject<Widget>* w = new RefCountedObject<Widget>();
  EXPECT_EQ(w->AddRef(), 1);
  EXPECT_TRUE(w->HasOneRef());
  EXPECT_EQ(w->AddRef(), 2);
  EXPECT_FALSE(w->HasOneRef());
  EXPECT_EQ(w->Release(), 1);
  EXPECT_TRUE(w->HasOneRef());
  EXPECT_EQ(w->Release(), 0);  // deletes w
}

TEST(RefCountedObjectTest, DeletesWhenRefCountReachesZero) {
  bool destroyed = false;
  IThing* thing = new RefCountedObject<ThingImpl>(5, &destroyed);
  EXPECT_EQ(thing->value(), 5);

  thing->AddRef();
  EXPECT_FALSE(destroyed);
  thing->Release();
  EXPECT_TRUE(destroyed);
}

TEST(RefCountedObjectTest, WorksWithScopedRefptr) {
  bool destroyed = false;
  {
    scoped_refptr<IThing> thing =
        new RefCountedObject<ThingImpl>(11, &destroyed);
    EXPECT_EQ(thing->value(), 11);
    EXPECT_FALSE(destroyed);
  }
  EXPECT_TRUE(destroyed);
}

}  // namespace libwebrtc
