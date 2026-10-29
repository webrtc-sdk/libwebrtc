#include "rtc_field_trials.h"

#include <memory>
#include <mutex>
#include <string>
#include <utility>

#include "api/field_trials.h"
#include "rtc_base/logging.h"
#include "rtc_field_trials_impl.h"

namespace libwebrtc {

namespace {

constexpr char kFieldTrialSeparator = '/';

std::mutex& FieldTrialsLock() {
  static std::mutex* lock = new std::mutex();
  return *lock;
}

std::string& FieldTrialsString() {
  static std::string* trials_string = new std::string();
  return *trials_string;
}

std::unique_ptr<webrtc::FieldTrials>& GlobalFieldTrials() {
  static std::unique_ptr<webrtc::FieldTrials>* field_trials =
      new std::unique_ptr<webrtc::FieldTrials>();
  return *field_trials;
}

}  // namespace

std::unique_ptr<webrtc::FieldTrialsView> CopyGlobalFieldTrials() {
  std::lock_guard<std::mutex> lock(FieldTrialsLock());
  const std::unique_ptr<webrtc::FieldTrials>& field_trials =
      GlobalFieldTrials();
  return field_trials == nullptr ? nullptr : field_trials->CreateCopy();
}

string RTCFieldTrials::BuildFieldTrialsString(
    const vector<string>& field_trials) {
  // We don't perform any extra format checking. That should be done by the
  // underlying WebRTC calls.
  std::string trials_string;
  for (size_t i = 0; i < field_trials.size(); ++i) {
    std::string entry = field_trials[i].std_string();
    if (entry.empty()) {
      continue;
    }
    trials_string += entry;
    if (entry.back() != kFieldTrialSeparator) {
      trials_string += kFieldTrialSeparator;
    }
  }
  return string(trials_string);
}

bool RTCFieldTrials::InitFieldTrials(const vector<string>& field_trials) {
  return InitFieldTrialsFromString(BuildFieldTrialsString(field_trials));
}

bool RTCFieldTrials::InitFieldTrialsFromString(const string& trials_string) {
  std::string trials = trials_string.std_string();
  std::unique_ptr<webrtc::FieldTrials> parsed =
      webrtc::FieldTrials::Create(trials);
  if (parsed == nullptr) {
    RTC_LOG(LS_ERROR) << "Invalid field trials string: " << trials;
    return false;
  }

  std::lock_guard<std::mutex> lock(FieldTrialsLock());
  GlobalFieldTrials() = std::move(parsed);
  FieldTrialsString() = trials;
  RTC_LOG(LS_INFO) << "Field trials set to: " << trials;
  return true;
}

string RTCFieldTrials::GetFieldTrialsString() {
  std::lock_guard<std::mutex> lock(FieldTrialsLock());
  return string(FieldTrialsString());
}

string RTCFieldTrials::Lookup(const string& key) {
  std::lock_guard<std::mutex> lock(FieldTrialsLock());
  const std::unique_ptr<webrtc::FieldTrials>& field_trials =
      GlobalFieldTrials();
  return string(field_trials == nullptr
                    ? std::string()
                    : field_trials->Lookup(key.std_string()));
}

bool RTCFieldTrials::IsEnabled(const string& key) {
  std::lock_guard<std::mutex> lock(FieldTrialsLock());
  const std::unique_ptr<webrtc::FieldTrials>& field_trials =
      GlobalFieldTrials();
  return field_trials != nullptr && field_trials->IsEnabled(key.std_string());
}

bool RTCFieldTrials::IsDisabled(const string& key) {
  std::lock_guard<std::mutex> lock(FieldTrialsLock());
  const std::unique_ptr<webrtc::FieldTrials>& field_trials =
      GlobalFieldTrials();
  return field_trials != nullptr && field_trials->IsDisabled(key.std_string());
}

}  // namespace libwebrtc
