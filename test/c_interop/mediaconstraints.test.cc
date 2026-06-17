#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

TEST(MediaConstraints, CreateReturnsHandle) {
    rtcMediaConstraintsHandle constraints = MediaConstraints_Create();
    EXPECT_NE(constraints, nullptr);
    if (constraints) RefCountedObject_Release(constraints);
}

TEST(MediaConstraints, AddMandatoryConstraintSucceeds) {
    rtcMediaConstraintsHandle constraints = MediaConstraints_Create();
    ASSERT_NE(constraints, nullptr);

    EXPECT_EQ(MediaConstraints_AddMandatoryConstraint(constraints, "key", "value"),
              rtcResultU4::kSuccess);

    RefCountedObject_Release(constraints);
}

TEST(MediaConstraints, AddOptionalConstraintSucceeds) {
    rtcMediaConstraintsHandle constraints = MediaConstraints_Create();
    ASSERT_NE(constraints, nullptr);

    EXPECT_EQ(MediaConstraints_AddOptionalConstraint(constraints, "key", "value"),
              rtcResultU4::kSuccess);

    RefCountedObject_Release(constraints);
}

// --- Negative paths ---

TEST(MediaConstraints, AddMandatoryConstraintNullHandleFails) {
    EXPECT_EQ(MediaConstraints_AddMandatoryConstraint(nullptr, "key", "value"),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(MediaConstraints, AddMandatoryConstraintNullKeyFails) {
    rtcMediaConstraintsHandle constraints = MediaConstraints_Create();
    ASSERT_NE(constraints, nullptr);

    EXPECT_EQ(MediaConstraints_AddMandatoryConstraint(constraints, nullptr, "value"),
              rtcResultU4::kInvalidParameter);

    RefCountedObject_Release(constraints);
}

TEST(MediaConstraints, AddMandatoryConstraintNullValueFails) {
    rtcMediaConstraintsHandle constraints = MediaConstraints_Create();
    ASSERT_NE(constraints, nullptr);

    EXPECT_EQ(MediaConstraints_AddMandatoryConstraint(constraints, "key", nullptr),
              rtcResultU4::kInvalidParameter);

    RefCountedObject_Release(constraints);
}

TEST(MediaConstraints, AddOptionalConstraintNullHandleFails) {
    EXPECT_EQ(MediaConstraints_AddOptionalConstraint(nullptr, "key", "value"),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(MediaConstraints, AddOptionalConstraintNullKeyFails) {
    rtcMediaConstraintsHandle constraints = MediaConstraints_Create();
    ASSERT_NE(constraints, nullptr);

    EXPECT_EQ(MediaConstraints_AddOptionalConstraint(constraints, nullptr, "value"),
              rtcResultU4::kInvalidParameter);

    RefCountedObject_Release(constraints);
}

TEST(MediaConstraints, AddOptionalConstraintNullValueFails) {
    rtcMediaConstraintsHandle constraints = MediaConstraints_Create();
    ASSERT_NE(constraints, nullptr);

    EXPECT_EQ(MediaConstraints_AddOptionalConstraint(constraints, "key", nullptr),
              rtcResultU4::kInvalidParameter);

    RefCountedObject_Release(constraints);
}

}  // namespace libwebrtc
