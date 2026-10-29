#include "rtc_logging.h"
#include "rtc_base/logging.h"
#include "rtc_base/log_sinks.h"
#include "rtc_logging_impl.h"

#include <memory>
#include <utility>
using std::unique_ptr;

namespace libwebrtc {

  void EnsureLoggingInitialized() {
    // The function-local static makes this run once and thread-safe.
    static const bool initialized = [] {
      webrtc::LoggingConfig logging_config;
#if defined(NDEBUG)
      // Before m150, release builds printed nothing to stderr by default.
      // LoggingConfig defaults to LS_INFO in every build type, so restore that.
      logging_config.set_debug_severity(webrtc::LS_NONE);
      logging_config.set_min_severity(webrtc::LS_NONE);
#endif
      return webrtc::InitializeLogging(std::move(logging_config));
    }();
    (void)initialized;
  }

  inline webrtc::LoggingSeverity getNativeLoggingSeverity(RTCLoggingSeverity severity) {
    switch (severity) {
      case Verbose:
        return webrtc::LS_VERBOSE;
      case Info:
        return webrtc::LS_INFO;
      case Warning:
        return webrtc::LS_WARNING;
      case Error:
        return webrtc::LS_ERROR;
      case None:
        return webrtc::LS_NONE;
    }
  }

  class CallbackLogSink final : public webrtc::LogSink {
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
    EnsureLoggingInitialized();
    webrtc::LogMessage::LogToDebug(getNativeLoggingSeverity(severity));
  }

  void LibWebRTCLogging::setLogSink(RTCLoggingSeverity severity, RTCCallbackLoggerMessageHandler callbackHandler) {
    // Registering a sink fixes the logging configuration, so apply ours first.
    EnsureLoggingInitialized();
    removeLogSink();
    log_sink.reset(new CallbackLogSink(callbackHandler));
    webrtc::LogMessage::AddLogToStream(log_sink.get(), getNativeLoggingSeverity(severity));
  }

  void LibWebRTCLogging::removeLogSink() {
    if(log_sink)
      webrtc::LogMessage::RemoveLogToStream(log_sink.get());
  }
}  // namespace libwebrtc
