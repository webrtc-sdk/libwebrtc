#ifndef LIB_WEBRTC_RTC_MEDIA_STREAM_HXX
#define LIB_WEBRTC_RTC_MEDIA_STREAM_HXX

#include "rtc_audio_track.h"
#include "rtc_types.h"
#include "rtc_video_track.h"
#include "rtc_base_list.h"

namespace libwebrtc {

class RTCMediaStream : public RefCountInterface {
 public:
  virtual bool AddTrack(scoped_refptr<RTCAudioTrack> track) = 0;

  virtual bool AddTrack(scoped_refptr<RTCVideoTrack> track) = 0;

  virtual bool RemoveTrack(scoped_refptr<RTCAudioTrack> track) = 0;

  virtual bool RemoveTrack(scoped_refptr<RTCVideoTrack> track) = 0;

  virtual vector<scoped_refptr<RTCAudioTrack>> audio_tracks() = 0;

  virtual vector<scoped_refptr<RTCVideoTrack>> video_tracks() = 0;

  virtual vector<scoped_refptr<RTCMediaTrack>> tracks() = 0;

  virtual scoped_refptr<RTCAudioTrack> FindAudioTrack(
      const string track_id) = 0;

  virtual scoped_refptr<RTCVideoTrack> FindVideoTrack(
      const string track_id) = 0;

  virtual const string label() = 0;

  virtual const string id() = 0;

 protected:
  ~RTCMediaStream() {}
};

/**
 * class RTCMediaStreamList
 */
class RTCMediaStreamList : public RTCBaseList<scoped_refptr<RTCMediaStream>> {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCMediaStreamList> Create(
    const vector<scoped_refptr<RTCMediaStream>>& source);

 protected:
  RTCMediaStreamList(const vector<scoped_refptr<RTCMediaStream>>& source)
    : RTCBaseList<scoped_refptr<RTCMediaStream>>(source)
  {}

  ~RTCMediaStreamList() {}
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_MEDIA_STREAM_HXX
