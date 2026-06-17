#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// A populated MediaRTCStatsList is only obtainable from a live peer
// connection's GetStats callback, which is out of scope for a standalone unit
// test. These tests therefore cover the safe negative paths of the standalone
// list accessors declared in include/interop_api.h (impl in
// src/interop/rtc_peerconnection_interop.cc).

TEST(RTCMediaRTCStatsListNegative, GetCountWithNullHandleReturnsZero) {
    // DECLARE_LIST_GET_COUNT uses CHECK_POINTER_EX(handle, 0).
    EXPECT_EQ(RTCMediaRTCStatsList_GetCount(nullptr), 0);
}

TEST(RTCMediaRTCStatsListNegative, GetItemWithNullOutPointerFails) {
    // CHECK_OUT_POINTER runs first: a null out-pointer is kInvalidPointer.
    EXPECT_EQ(RTCMediaRTCStatsList_GetItem(nullptr, 0, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCMediaRTCStatsListNegative, GetItemWithNullHandleFails) {
    // With a valid out-pointer, CHECK_NATIVE_HANDLE reports the null handle.
    rtcMediaRTCStatsHandle item = nullptr;
    EXPECT_EQ(RTCMediaRTCStatsList_GetItem(nullptr, 0, &item),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(item, nullptr);
}

}  // namespace libwebrtc
