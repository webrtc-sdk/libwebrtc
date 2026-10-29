#ifndef LIB_WEBRTC_RTC_LOGGING_IMPL_HXX
#define LIB_WEBRTC_RTC_LOGGING_IMPL_HXX

namespace libwebrtc {

/**
 * Applies this library's default logging configuration, once per process.
 * Since m150 WebRTC fixes its logging configuration on the first log call or
 * sink registration, and its default prints LS_INFO to stderr in every build
 * type. Release builds are kept silent by default, as they were before m150.
 * Must be called before anything logs or registers a log sink.
 */
void EnsureLoggingInitialized();

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_LOGGING_IMPL_HXX
