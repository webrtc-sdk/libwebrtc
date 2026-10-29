#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {
namespace {

void LIB_WEBRTC_CALL OnLogMessage(rtcObjectHandle /*user_data*/,
                                  const char* /*message*/) {}

}  // namespace

TEST(RTCLogging, SetMinDebugLogLevelAcceptsAllSeverities) {
    EXPECT_EQ(RTCLogging_SetMinDebugLogLevel(rtcLoggingSeverity::kVerbose),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCLogging_SetMinDebugLogLevel(rtcLoggingSeverity::kInfo),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCLogging_SetMinDebugLogLevel(rtcLoggingSeverity::kWarning),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCLogging_SetMinDebugLogLevel(rtcLoggingSeverity::kError),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCLogging_SetMinDebugLogLevel(rtcLoggingSeverity::kNone),
              rtcResultU4::kSuccess);
}

TEST(RTCLogging, SetMinDebugLogLevelRejectsInvalidSeverity) {
    EXPECT_EQ(RTCLogging_SetMinDebugLogLevel(static_cast<rtcLoggingSeverity>(99)),
              rtcResultU4::kInvalidParameter);
}

TEST(RTCLogging, SetLogSinkSucceeds) {
    EXPECT_EQ(RTCLogging_SetLogSink(rtcLoggingSeverity::kInfo, nullptr,
                                    &OnLogMessage),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCLogging_RemoveLogSink(), rtcResultU4::kSuccess);
}

TEST(RTCLogging, SetLogSinkRejectsInvalidSeverity) {
    EXPECT_EQ(RTCLogging_SetLogSink(static_cast<rtcLoggingSeverity>(99), nullptr,
                                    &OnLogMessage),
              rtcResultU4::kInvalidParameter);
}

TEST(RTCLogging, SetLogSinkAcceptsNullCallback) {
    // The implementation does not reject a null callback delegate.
    EXPECT_EQ(RTCLogging_SetLogSink(rtcLoggingSeverity::kInfo, nullptr, nullptr),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCLogging_RemoveLogSink(), rtcResultU4::kSuccess);
}

TEST(RTCLogging, RemoveLogSinkSucceedsWithoutSink) {
    EXPECT_EQ(RTCLogging_RemoveLogSink(), rtcResultU4::kSuccess);
}

}  // namespace libwebrtc
