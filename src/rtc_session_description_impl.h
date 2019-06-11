#ifndef LIB_WEBRTC_RTC_SESSION_DESCRIPTION_IMPL_HXX
#define LIB_WEBRTC_RTC_SESSION_DESCRIPTION_IMPL_HXX

#include "rtc_types.h"

#include "rtc_session_description.h"
#include "api/jsep.h"

namespace libwebrtc {

class RTCSessionDescriptionImpl : public RTCSessionDescription {
 public:
  RTCSessionDescriptionImpl(
      std::unique_ptr<webrtc::SessionDescriptionInterface> description);
  virtual ~RTCSessionDescriptionImpl() {}

  virtual const char* sdp() const override;

  virtual SdpType GetType() override;

  virtual const char* type() override;

  virtual bool ToString(char* out, int length) override;

  webrtc::SessionDescriptionInterface* description() {
    return description_.get();
  }

 private:
  std::unique_ptr<webrtc::SessionDescriptionInterface> description_;
  std::string sdp_;
  std::string type_;
};

} // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_SESSION_DESCRIPTION_IMPL_HXX