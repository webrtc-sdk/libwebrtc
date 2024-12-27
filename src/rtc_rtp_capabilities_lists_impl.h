#ifndef LIB_WBBRTC_RTC_RTP_CAPABILITIES_LISTS_IMPL_HXX
#define LIB_WBBRTC_RTC_RTP_CAPABILITIES_LISTS_IMPL_HXX

#include "rtc_rtp_capabilities.h"
#include "rtc_rtp_capabilities_lists.h"

namespace libwebrtc {

/**
 * class RTCRtpCodecCapabilityListImpl
 */
class RTCRtpCodecCapabilityListImpl : public RTCRtpCodecCapabilityList {
 public:
  RTCRtpCodecCapabilityListImpl(const vector<scoped_refptr<RTCRtpCodecCapability>>& source);
  ~RTCRtpCodecCapabilityListImpl();
};

/**
 * class RTCRtpHeaderExtensionCapabilityListImpl
 */
class RTCRtpHeaderExtensionCapabilityListImpl : public RTCRtpHeaderExtensionCapabilityList {
 public:
  RTCRtpHeaderExtensionCapabilityListImpl(const vector<scoped_refptr<RTCRtpHeaderExtensionCapability>>& source);
  ~RTCRtpHeaderExtensionCapabilityListImpl();
};

}  // namespace libwebrtc

#endif  //! LIB_WBBRTC_RTC_RTP_CAPABILITIES_LISTS_IMPL_HXX