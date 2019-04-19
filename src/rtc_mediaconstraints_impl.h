#ifndef LIB_WEBRTC_RTC_MEDIA_CONSTRAINTS_IMPL_HXX
#define LIB_WEBRTC_RTC_MEDIA_CONSTRAINTS_IMPL_HXX

#include "rtc_mediaconstraints.h"

#include "api/mediaconstraintsinterface.h"

namespace libwebrtc {

class RTCMediaConstraintsImpl : public RTCMediaConstraints,
                                public webrtc::MediaConstraintsInterface {
 public:
  RTCMediaConstraintsImpl() {}
  virtual ~RTCMediaConstraintsImpl() {}

  const webrtc::MediaConstraintsInterface::Constraints& GetMandatory()
      const override {
    return mandatory_;
  }

  const webrtc::MediaConstraintsInterface::Constraints& GetOptional()
      const override {
    return optional_;
  }

  void AddMandatoryConstraint(const char* key, const char* value) override;

  void AddOptionalConstraint(const char* key, const char* value) override;

 private:
  webrtc::MediaConstraintsInterface::Constraints mandatory_;
  webrtc::MediaConstraintsInterface::Constraints optional_;
};

};  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_MEDIA_CONSTRAINTS_IMPL_HXX
