#ifndef LIB_WEBRTC_RTC_SESSION_DESCRIPTION_IMPL_HXX
#define LIB_WEBRTC_RTC_SESSION_DESCRIPTION_IMPL_HXX
#include "api/jsep.h"
#include "rtc_session_description.h"
#include "rtc_types.h"

namespace libwebrtc {

class RTCSessionDescriptionImpl : public RTCSessionDescription {
 public:
  RTCSessionDescriptionImpl(
      std::unique_ptr<webrtc::SessionDescriptionInterface> description);
  virtual ~RTCSessionDescriptionImpl() {}

  virtual const string sdp() const override;

  virtual SdpType GetType() override;

  virtual const string type() override;

  virtual bool ToString(string& out) override;

  webrtc::SessionDescriptionInterface* description() {
    return description_.get();
  }

 private:
  std::unique_ptr<webrtc::SessionDescriptionInterface> description_;
  std::string sdp_;
  std::string type_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_SESSION_DESCRIPTION_IMPL_HXX