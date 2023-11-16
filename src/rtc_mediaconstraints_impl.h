#ifndef LIB_WEBRTC_RTC_MEDIA_CONSTRAINTS_IMPL_HXX
#define LIB_WEBRTC_RTC_MEDIA_CONSTRAINTS_IMPL_HXX

#include "rtc_mediaconstraints.h"
#include "sdk/media_constraints.h"

namespace libwebrtc {

class RTCMediaConstraintsImpl : public RTCMediaConstraints,
                                public webrtc::MediaConstraints {
 public:
  RTCMediaConstraintsImpl() {}
  virtual ~RTCMediaConstraintsImpl() {}

  const Constraints& GetMandatory() const { return mandatory_; }
  const Constraints& GetOptional() const { return optional_; }

  void AddMandatoryConstraint(const string key, const string value) override;

  void AddOptionalConstraint(const string key, const string value) override;

 private:
  webrtc::MediaConstraints::Constraints mandatory_;
  webrtc::MediaConstraints::Constraints optional_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_MEDIA_CONSTRAINTS_IMPL_HXX
