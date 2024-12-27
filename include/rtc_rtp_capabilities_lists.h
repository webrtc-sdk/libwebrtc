#ifndef LIB_WBBRTC_RTC_RTP_CAPABILITIES_LISTS_HXX
#define LIB_WBBRTC_RTC_RTP_CAPABILITIES_LISTS_HXX

#include "rtc_types.h"
#include "rtc_base_list.h"
#include "rtc_rtp_capabilities.h"

namespace libwebrtc {

/**
 * class RTCRtpCodecCapabilityList
 */
class RTCRtpCodecCapabilityList : public RTCBaseList<scoped_refptr<RTCRtpCodecCapability>> {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCRtpCodecCapabilityList> Create(
    const vector<scoped_refptr<RTCRtpCodecCapability>>& source);

 protected:
  RTCRtpCodecCapabilityList(const vector<scoped_refptr<RTCRtpCodecCapability>>& source)
    : RTCBaseList<scoped_refptr<RTCRtpCodecCapability>>(source)
  {}

  ~RTCRtpCodecCapabilityList() {}
};

/**
 * class RTCRtpHeaderExtensionCapabilityList
 */
class RTCRtpHeaderExtensionCapabilityList : public RTCBaseList<scoped_refptr<RTCRtpHeaderExtensionCapability>> {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCRtpHeaderExtensionCapabilityList> Create(
    const vector<scoped_refptr<RTCRtpHeaderExtensionCapability>>& source);

 protected:
  RTCRtpHeaderExtensionCapabilityList(const vector<scoped_refptr<RTCRtpHeaderExtensionCapability>>& source)
    : RTCBaseList<scoped_refptr<RTCRtpHeaderExtensionCapability>>(source)
  {}

  ~RTCRtpHeaderExtensionCapabilityList() {}
};

} // namespace libwebrtc

#endif // LIB_WBBRTC_RTC_RTP_CAPABILITIES_LISTS_HXX
