#ifndef LIB_WEBRTC_RTC_FIELD_TRIALS_HXX
#define LIB_WEBRTC_RTC_FIELD_TRIALS_HXX

#include "rtc_types.h"

namespace libwebrtc {

/**
 * The only valid value for the following keys, if set, is
 * kRTCFieldTrialEnabledValue.
 */
constexpr char kRTCFieldTrialAudioForceABWENoTWCCKey[] =
    "WebRTC-Audio-ABWENoTWCC";
constexpr char kRTCFieldTrialFlexFec03AdvertisedKey[] =
    "WebRTC-FlexFEC-03-Advertised";
constexpr char kRTCFieldTrialFlexFec03Key[] = "WebRTC-FlexFEC-03";
constexpr char kRTCFieldTrialH264HighProfileKey[] = "WebRTC-H264HighProfile";
constexpr char kRTCFieldTrialMinimizeResamplingOnMobileKey[] =
    "WebRTC-Audio-MinimizeResamplingOnMobile";
constexpr char kRTCFieldTrialUseNWPathMonitorKey[] =
    "WebRTC-Network-UseNWPathMonitor";
constexpr char kRTCFieldTrialIceHandshakeDtlsKey[] = "WebRTC-IceHandshakeDtls";

/** The valid value for the field trials above. */
constexpr char kRTCFieldTrialEnabledValue[] = "Enabled";

/** The value used to explicitly turn a field trial off. */
constexpr char kRTCFieldTrialDisabledValue[] = "Disabled";

/**
 * @class RTCFieldTrials
 * @brief Configures and queries the global WebRTC field trials.
 *
 * Field trials allow clients to turn on feature code that is not enabled by
 * default. They must be initialized before any other call into WebRTC, i.e.
 * before LibWebRTC::Initialize() and before any peer connection factory is
 * created, otherwise the objects already created keep using the previous
 * configuration.
 *
 * A field trial is described by a "Key/Value" pair, where the key is one of
 * the constants above (or any other trial name known to WebRTC) and the value
 * is the group to assign the process to, usually kRTCFieldTrialEnabledValue.
 *
 * See also: webrtc/system_wrappers/include/field_trial.h
 */
class RTCFieldTrials {
 public:
  /**
   * @brief Assembles a WebRTC field trial string out of a list of entries.
   *
   * Each entry is either "Key/Value" or "Key/Value/"; empty entries are
   * skipped. The returned string is the concatenation of all the entries in
   * the canonical "Key1/Value1/Key2/Value2/" form. No extra format checking is
   * performed here, that is done by the underlying WebRTC calls.
   *
   * @param field_trials The field trial entries.
   * @return The assembled field trial string.
   */
  LIB_WEBRTC_API static string BuildFieldTrialsString(
      const vector<string>& field_trials);

  /**
   * @brief Initializes the global field trials from a list of entries.
   *
   * Convenience wrapper around BuildFieldTrialsString() followed by
   * InitFieldTrialsFromString(). Must be called before any other call into
   * WebRTC.
   *
   * @param field_trials The field trial entries, e.g.
   * {"WebRTC-FlexFEC-03/Enabled/"}.
   * @return true if the field trials were applied, false if the resulting
   * string is invalid.
   */
  LIB_WEBRTC_API static bool InitFieldTrials(
      const vector<string>& field_trials);

  /**
   * @brief Initializes the global field trials from a string.
   *
   * The string must be in the "Key1/Value1/Key2/Value2/" form, note the
   * trailing separator. An empty string clears every previously configured
   * trial. Must be called before any other call into WebRTC.
   *
   * @param trials_string The field trial string.
   * @return true if the string is valid and was applied, false otherwise.
   */
  LIB_WEBRTC_API static bool InitFieldTrialsFromString(
      const string& trials_string);

  /**
   * @brief Returns the field trial string currently in use, or an empty
   * string if none was configured.
   */
  LIB_WEBRTC_API static string GetFieldTrialsString();

  /**
   * @brief Returns the group the process is assigned to for `key`, or an
   * empty string if the trial is not configured.
   */
  LIB_WEBRTC_API static string Lookup(const string& key);

  /**
   * @brief Returns true if the group configured for `key` starts with
   * "Enabled".
   */
  LIB_WEBRTC_API static bool IsEnabled(const string& key);

  /**
   * @brief Returns true if the group configured for `key` starts with
   * "Disabled".
   */
  LIB_WEBRTC_API static bool IsDisabled(const string& key);
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_FIELD_TRIALS_HXX
