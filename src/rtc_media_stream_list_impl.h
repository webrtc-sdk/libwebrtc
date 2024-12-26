#ifndef LIB_WEBRTC_MEDIA_STREAM_LIST_IMPL_HXX
#define LIB_WEBRTC_MEDIA_STREAM_LIST_IMPL_HXX

#include "rtc_media_stream_list.h"
#include "rtc_media_stream.h"

namespace libwebrtc {

class RTCMediaStreamListImpl : public RTCMediaStreamList {
 public:
  RTCMediaStreamListImpl(const vector<scoped_refptr<RTCMediaStream>>& source);
  ~RTCMediaStreamListImpl();
};

}  // namespace libwebrtc

#endif  //! LIB_WEBRTC_MEDIA_STREAM_LIST_IMPL_HXX