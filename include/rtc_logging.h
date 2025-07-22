#ifndef LIB_WEBRTC_RTC_LOGGING_HXX
#define LIB_WEBRTC_RTC_LOGGING_HXX

#include "rtc_types.h"

namespace libwebrtc {

  enum RTCLoggingSeverity {
    Verbose,
    Info,
    Warning,
    Error,
    None,
  };

  typedef void (*RTCCallbackLoggerMessageHandler)(const string& message);

  class LibWebRTCLogging {
    public:
      LIB_WEBRTC_API static void setMinDebugLogLevel(RTCLoggingSeverity severity);
      LIB_WEBRTC_API static void setLogSink(RTCLoggingSeverity severity, RTCCallbackLoggerMessageHandler callbackHandler);
      LIB_WEBRTC_API static void removeLogSink();
  };
}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_LOGGING_HXX
