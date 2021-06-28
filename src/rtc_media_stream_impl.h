#ifndef LIB_WEBRTC_MEDIA_STREAM_IMPL_HXX
#define LIB_WEBRTC_MEDIA_STREAM_IMPL_HXX

#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include "api/stats_types.h"

#include "rtc_media_stream.h"
#include "rtc_peerconnection.h"

namespace libwebrtc {

class WebRTCStatsObserver : public webrtc::StatsObserver {
 public:
  WebRTCStatsObserver(scoped_refptr<TrackStatsObserver> observer,
                      std::string direction)
      : observer_(observer), direction_(direction) {}

  ~WebRTCStatsObserver() {}

  static rtc::scoped_refptr<WebRTCStatsObserver> Create(
      scoped_refptr<TrackStatsObserver> observer,
      std::string direction) {
    rtc::scoped_refptr<WebRTCStatsObserver> rtc_stats_observer =
        rtc::scoped_refptr<WebRTCStatsObserver>(
            new rtc::RefCountedObject<WebRTCStatsObserver>(observer,
                                                           direction));
    rtc_stats_observer->AddRef();
    return rtc_stats_observer;
  }

  virtual void OnComplete(const webrtc::StatsReports& reports) override;

 private:
  scoped_refptr<TrackStatsObserver> observer_;
  std::string direction_;
};

class MediaStreamImpl : public RTCMediaStream,
                        public webrtc::ObserverInterface {
 public:
  MediaStreamImpl(
      rtc::scoped_refptr<webrtc::MediaStreamInterface> rtc_media_stream);

  ~MediaStreamImpl();

  virtual bool AddTrack(scoped_refptr<RTCAudioTrack> track) override;

  virtual bool AddTrack(scoped_refptr<RTCVideoTrack> track) override;

  virtual bool RemoveTrack(scoped_refptr<RTCAudioTrack> track) override;

  virtual bool RemoveTrack(scoped_refptr<RTCVideoTrack> track) override;

  virtual vector<scoped_refptr<RTCAudioTrack>> audio_tracks() override;

  virtual vector<scoped_refptr<RTCVideoTrack>> video_tracks() override;

  virtual vector<scoped_refptr<RTCMediaTrack>> tracks() override;

  virtual scoped_refptr<RTCAudioTrack> FindAudioTrack(
      const string track_id) override;

  virtual scoped_refptr<RTCVideoTrack> FindVideoTrack(
      const string track_id) override;

  virtual const string label() override { return label_; }

  virtual const string id() override { return id_; }

  virtual void OnChanged() override;

 public:
  rtc::scoped_refptr<webrtc::MediaStreamInterface> rtc_media_stream() {
    return rtc_media_stream_;
  }

  void RegisterRTCPeerConnectionObserver(RTCPeerConnectionObserver* observer) {
    observer_ = observer;
  }

 private:
  rtc::scoped_refptr<webrtc::MediaStreamInterface> rtc_media_stream_;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> rtc_peerconnection_;
  vector<scoped_refptr<RTCAudioTrack>> audio_tracks_;
  vector<scoped_refptr<RTCVideoTrack>> video_tracks_;
  RTCPeerConnectionObserver* observer_ = nullptr;
  string label_, id_;
};

} // namespace libwebrtc
#endif  //! LIB_WEBRTC_MEDIA_STREAM_IMPL_HXX