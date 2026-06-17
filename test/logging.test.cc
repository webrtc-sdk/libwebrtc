#include <cstdint>
#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_logging.h"
#include "rtc_types.h"
#include "libwebrtc.h"

namespace libwebrtc {
namespace {

void OnLogMessage(const string& /*message*/) {}

}  // namespace

TEST(LibWebRTCLoggingTest, SetMinDebugLogLevelAcceptsAllSeverities) {
  LibWebRTCLogging::setMinDebugLogLevel(RTCLoggingSeverity::Verbose);
  LibWebRTCLogging::setMinDebugLogLevel(RTCLoggingSeverity::Info);
  LibWebRTCLogging::setMinDebugLogLevel(RTCLoggingSeverity::Warning);
  LibWebRTCLogging::setMinDebugLogLevel(RTCLoggingSeverity::Error);
  LibWebRTCLogging::setMinDebugLogLevel(RTCLoggingSeverity::None);
  // Restore a quiet default for the rest of the suite.
  LibWebRTCLogging::setMinDebugLogLevel(RTCLoggingSeverity::None);
}

TEST(LibWebRTCLoggingTest, SetAndRemoveLogSink) {
  LibWebRTCLogging::setLogSink(RTCLoggingSeverity::Info, &OnLogMessage);
  LibWebRTCLogging::removeLogSink();
}

TEST(LibWebRTCLoggingTest, SetLogSinkWithNullHandlerIsAccepted) {
  LibWebRTCLogging::setLogSink(RTCLoggingSeverity::Info, nullptr);
  LibWebRTCLogging::removeLogSink();
}

TEST(LibWebRTCLoggingTest, RemoveLogSinkWithoutSinkIsSafe) {
  LibWebRTCLogging::removeLogSink();
}

}  // namespace libwebrtc
