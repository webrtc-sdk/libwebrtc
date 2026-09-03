#ifndef LIB_WEBRTC_RTC_FIELD_TRIALS_IMPL_HXX
#define LIB_WEBRTC_RTC_FIELD_TRIALS_IMPL_HXX

#include <memory>

#include "api/field_trials_view.h"

namespace libwebrtc {

/**
 * Returns a snapshot of the field trials configured through RTCFieldTrials,
 * or nullptr when none were configured. Used to build the webrtc::Environment
 * of the objects created by this library.
 */
std::unique_ptr<webrtc::FieldTrialsView> CopyGlobalFieldTrials();

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_FIELD_TRIALS_IMPL_HXX
