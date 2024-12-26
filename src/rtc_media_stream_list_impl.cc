#include "rtc_media_stream_list_impl.h"

#include "rtc_base/logging.h"
#include "rtc_media_stream.h"
#include "rtc_types.h"

namespace libwebrtc {

scoped_refptr<RTCMediaStreamList> RTCMediaStreamList::Create(
    const vector<scoped_refptr<RTCMediaStream>>& source) {
  return new RefCountedObject<RTCMediaStreamListImpl>(source);
}

RTCMediaStreamListImpl::RTCMediaStreamListImpl(
    const vector<scoped_refptr<RTCMediaStream>>& source)
    : RTCMediaStreamList(source) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor ";
}

RTCMediaStreamListImpl::~RTCMediaStreamListImpl() {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}

}  // namespace libwebrtc
