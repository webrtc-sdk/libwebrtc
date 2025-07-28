#include "rtc_logging.h"
#include "rtc_base/logging.h"
#include "rtc_base/log_sinks.h"

#include <memory>
using std::unique_ptr;

namespace libwebrtc {

  inline rtc::LoggingSeverity getNativeLoggingSeverity(RTCLoggingSeverity severity) {
    switch (severity) {
      case Verbose:
        return rtc::LS_VERBOSE;
      case Info:
        return rtc::LS_INFO;
      case Warning:
        return rtc::LS_WARNING;
      case Error:
        return rtc::LS_ERROR;
      case None:
        return rtc::LS_NONE;
    }
  }

  class CallbackLogSink final : public rtc::LogSink {
    public:
      CallbackLogSink(RTCCallbackLoggerMessageHandler _callbackHandler)
          : callback_handler(_callbackHandler) {}

      void OnLogMessage(const std::string& message) override {
          callback_handler(string(message));
      }

    private:
      RTCCallbackLoggerMessageHandler callback_handler;
  };

  static std::unique_ptr<CallbackLogSink> log_sink;

  void LibWebRTCLogging::setMinDebugLogLevel(RTCLoggingSeverity severity) {
    rtc::LogMessage::LogToDebug(getNativeLoggingSeverity(severity));
  }

  void LibWebRTCLogging::setLogSink(RTCLoggingSeverity severity, RTCCallbackLoggerMessageHandler callbackHandler) {
    removeLogSink();
    log_sink.reset(new CallbackLogSink(callbackHandler));
    rtc::LogMessage::AddLogToStream(log_sink.get(), getNativeLoggingSeverity(severity));
  }

  void LibWebRTCLogging::removeLogSink() {
    if(log_sink)
      rtc::LogMessage::RemoveLogToStream(log_sink.get());
  }
}  // namespace libwebrtc
