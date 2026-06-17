#include "rtc_logging.h"
#include "interop_api.h"

using namespace libwebrtc;


// Wrapper callback to convert between C++ and C callbacks
namespace libwebrtc{
struct LoggingCallbackData {
    rtcObjectHandle userData;
    rtcLoggingMessageDelegate callback;
};

void LoggingCallbackWrapper(const string& message, void* context) {
    if (!context) return;
    
    LoggingCallbackData* data = static_cast<LoggingCallbackData*>(context);
    if (data->callback) {
        // Call C callback with the message
        data->callback(data->userData, message.c_string());
    }
}

// Global state for logging
static LoggingCallbackData* g_loggingCallbackData = nullptr;
static RTCCallbackLoggerMessageHandler g_loggingHandler = nullptr;

// Static callback that bridges to C callback
void StaticLoggingHandler(const string& message) {
    if (g_loggingCallbackData && g_loggingCallbackData->callback) {
        g_loggingCallbackData->callback(g_loggingCallbackData->userData, message.c_string());
    }
}
}  // namespace


rtcResultU4 LIB_WEBRTC_CALL
RTCLogging_SetMinDebugLogLevel(
    rtcLoggingSeverity severity
) noexcept {
    // Convert C API enum to C++ enum
    RTCLoggingSeverity nativeSeverity;
    switch (severity) {
        case rtcLoggingSeverity::kVerbose:
            nativeSeverity = RTCLoggingSeverity::Verbose;
            break;
        case rtcLoggingSeverity::kInfo:
            nativeSeverity = RTCLoggingSeverity::Info;
            break;
        case rtcLoggingSeverity::kWarning:
            nativeSeverity = RTCLoggingSeverity::Warning;
            break;
        case rtcLoggingSeverity::kError:
            nativeSeverity = RTCLoggingSeverity::Error;
            break;
        case rtcLoggingSeverity::kNone:
            nativeSeverity = RTCLoggingSeverity::None;
            break;
        default:
            return rtcResultU4::kInvalidParameter;
    }
        
    LibWebRTCLogging::setMinDebugLogLevel(nativeSeverity);
    return rtcResultU4::kSuccess;

}


rtcResultU4 LIB_WEBRTC_CALL
RTCLogging_SetLogSink(
    rtcLoggingSeverity severity,
    rtcObjectHandle userData,
    rtcLoggingMessageDelegate callback
) noexcept {
    // First remove any existing log sink
    RTCLogging_RemoveLogSink();
        
    // Convert C API enum to C++ enum
    RTCLoggingSeverity nativeSeverity;
    switch (severity) {
        case rtcLoggingSeverity::kVerbose:
            nativeSeverity = RTCLoggingSeverity::Verbose;
            break;
        case rtcLoggingSeverity::kInfo:
            nativeSeverity = RTCLoggingSeverity::Info;
            break;
        case rtcLoggingSeverity::kWarning:
            nativeSeverity = RTCLoggingSeverity::Warning;
            break;
        case rtcLoggingSeverity::kError:
            nativeSeverity = RTCLoggingSeverity::Error;
            break;
        case rtcLoggingSeverity::kNone:
            nativeSeverity = RTCLoggingSeverity::None;
            break;
        default:
            return rtcResultU4::kInvalidParameter;
    }
        
    // Create new callback data
    g_loggingCallbackData = new LoggingCallbackData{userData, callback};
    g_loggingHandler = StaticLoggingHandler;
        
    // Set the log sink
    LibWebRTCLogging::setLogSink(nativeSeverity, g_loggingHandler);
        
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCLogging_RemoveLogSink(
) noexcept {
    LibWebRTCLogging::removeLogSink();
        
    // Clean up callback data
    if (g_loggingCallbackData) {
        delete g_loggingCallbackData;
        g_loggingCallbackData = nullptr;
    }
    g_loggingHandler = nullptr;
        
    return rtcResultU4::kSuccess;
}