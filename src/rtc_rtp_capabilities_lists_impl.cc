#include "rtc_rtp_capabilities_lists_impl.h"

#include "rtc_base/logging.h"
#include "rtc_rtp_capabilities.h"
#include "rtc_types.h"

namespace libwebrtc {

scoped_refptr<RTCRtpCodecCapabilityList> RTCRtpCodecCapabilityList::Create(
    const vector<scoped_refptr<RTCRtpCodecCapability>>& source) {
  return new RefCountedObject<RTCRtpCodecCapabilityListImpl>(source);
}

RTCRtpCodecCapabilityListImpl::RTCRtpCodecCapabilityListImpl(
    const vector<scoped_refptr<RTCRtpCodecCapability>>& source)
    : RTCRtpCodecCapabilityList(source) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor ";
}

RTCRtpCodecCapabilityListImpl::~RTCRtpCodecCapabilityListImpl() {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}


scoped_refptr<RTCRtpHeaderExtensionCapabilityList> RTCRtpHeaderExtensionCapabilityList::Create(
    const vector<scoped_refptr<RTCRtpHeaderExtensionCapability>>& source) {
  return new RefCountedObject<RTCRtpHeaderExtensionCapabilityListImpl>(source);
}

RTCRtpHeaderExtensionCapabilityListImpl::RTCRtpHeaderExtensionCapabilityListImpl(
    const vector<scoped_refptr<RTCRtpHeaderExtensionCapability>>& source)
    : RTCRtpHeaderExtensionCapabilityList(source) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor ";
}

RTCRtpHeaderExtensionCapabilityListImpl::~RTCRtpHeaderExtensionCapabilityListImpl() {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}

}  // namespace libwebrtc
