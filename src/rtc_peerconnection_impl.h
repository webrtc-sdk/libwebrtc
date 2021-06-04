#ifndef LIB_WEBRTC_MEDIA_SESSION_IMPL_HXX
#define LIB_WEBRTC_MEDIA_SESSION_IMPL_HXX

#include "rtc_audio_track_impl.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_video_sink_adapter.h"
#include "rtc_video_source.h"
#include "rtc_video_source_impl.h"
#include "rtc_video_track_impl.h"

#include "api/data_channel_interface.h"
#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"

#include "src/internal/video_capturer.h"
#include "modules/video_capture/video_capture.h"
#include "rtc_base/synchronization/mutex.h"
#include "api/scoped_refptr.h"

#include <deque>
#include <map>
#include <set>
#include <string>

namespace webrtc {
class VideoCaptureModule;
}  // namespace webrtc

namespace cricket {
class VideoRenderer;
}  // namespace cricket

namespace libwebrtc {

class RTCPeerConnectionImpl : public RTCPeerConnection,
                              public webrtc::PeerConnectionObserver {
 public:
  virtual bool Initialize();

  virtual void CreateOffer(
      OnSdpCreateSuccess success,
      OnSdpCreateFailure failure,
      scoped_refptr<RTCMediaConstraints> constraints) override;

  virtual void CreateAnswer(
      OnSdpCreateSuccess success,
      OnSdpCreateFailure failure,
      scoped_refptr<RTCMediaConstraints> constraints) override;

  virtual void SetLocalDescription(const char* sdp,
                                   const char* type,
                                   OnSetSdpSuccess success,
                                   OnSetSdpFailure failure) override;

  virtual void SetRemoteDescription(const char* sdp,
                                    const char* type,
                                    OnSetSdpSuccess success,
                                    OnSetSdpFailure failure) override;

  virtual void GetLocalDescription(OnGetSdpSuccess success,
                                   OnGetSdpFailure failure) override;

  virtual void GetRemoteDescription(OnGetSdpSuccess success,
                                    OnGetSdpFailure failure) override;

  virtual void AddCandidate(const char* mid,
                            int midx,
                            const char* candiate) override;

  virtual void Close() override;

  virtual void RegisterRTCPeerConnectionObserver(
      RTCPeerConnectionObserver* observer) override;

  virtual void DeRegisterRTCPeerConnectionObserver() override;

 public:
  virtual int AddStream(scoped_refptr<RTCMediaStream> stream) override;

  virtual int RemoveStream(scoped_refptr<RTCMediaStream> stream) override;

  virtual MediaStreamVector local_streams() override;

  virtual MediaStreamVector remote_streams() override;

  virtual scoped_refptr<RTCDataChannel> CreateDataChannel(
      const char* label,
      const RTCDataChannelInit *dataChannelDict) override;

  virtual bool GetStats(const RTCAudioTrack* track,
                        scoped_refptr<TrackStatsObserver> observer) override;

  virtual bool GetStats(const RTCVideoTrack* track,
                        scoped_refptr<TrackStatsObserver> observer) override;

 public:
  RTCPeerConnectionImpl(
      const RTCConfiguration& configuration,
      scoped_refptr<RTCMediaConstraints> constraints,
      rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
          peer_connection_factory);

 protected:
  ~RTCPeerConnectionImpl();

  virtual void OnAddStream(
      rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;

  virtual void OnRemoveStream(
      rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;

  virtual void OnDataChannel(
      rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;

  virtual void OnRenegotiationNeeded() override {
    if (observer_)
      observer_->OnRenegotiationNeeded();
  }

  virtual void OnIceCandidate(
      const webrtc::IceCandidateInterface* candidate) override;

  virtual void OnIceGatheringChange(
      webrtc::PeerConnectionInterface::IceGatheringState new_state) override;

  virtual void OnIceConnectionChange(
      webrtc::PeerConnectionInterface::IceConnectionState new_state) override;

  virtual void OnSignalingChange(
      webrtc::PeerConnectionInterface::SignalingState new_state) override;

 protected:
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
      rtc_peerconnection_factory_;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> rtc_peerconnection_;
  const RTCConfiguration& configuration_;
  scoped_refptr<RTCMediaConstraints> constraints_;
  webrtc::PeerConnectionInterface::RTCOfferAnswerOptions offer_answer_options_;
  RTCPeerConnectionObserver* observer_ = nullptr;
  std::unique_ptr<webrtc::Mutex> callback_crt_sec_;
  bool initialize_offer_sent = false;
  MediaStreamVector local_streams_;
  MediaStreamVector remote_streams_;
  scoped_refptr<RTCDataChannel> data_channel_;
};

} // namespace libwebrtc

#endif  // LIB_WEBRTC_MEDIA_SESSION_IMPL_HXX
