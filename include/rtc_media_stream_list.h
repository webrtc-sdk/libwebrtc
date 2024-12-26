#ifndef LIB_WEBRTC_RTC_MEDIA_STREAM_LIST_HXX
#define LIB_WEBRTC_RTC_MEDIA_STREAM_LIST_HXX

#include "rtc_types.h"
#include "rtc_base_list.h"
#include "rtc_media_stream.h"

namespace libwebrtc {

class RTCMediaStreamList : public RTCBaseList<scoped_refptr<RTCMediaStream>> {
 public:
  static LIB_WEBRTC_API scoped_refptr<RTCMediaStreamList> Create(
    const vector<scoped_refptr<RTCMediaStream>>& source);

 protected:
  RTCMediaStreamList(const vector<scoped_refptr<RTCMediaStream>>& source)
    : RTCBaseList<scoped_refptr<RTCMediaStream>>(source)
  {}
  
  ~RTCMediaStreamList() {}
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_MEDIA_STREAM_LIST_HXX
