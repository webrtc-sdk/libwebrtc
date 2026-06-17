#ifndef LIB_WEBRTC_MEDIA_SESSION_IMPL_HXX
#define LIB_WEBRTC_MEDIA_SESSION_IMPL_HXX

#include <deque>
#include <map>
#include <set>
#include <string>

#include "api/data_channel_interface.h"
#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include "api/scoped_refptr.h"
#include "modules/video_capture/video_capture.h"
#include "rtc_audio_track_impl.h"
#include "rtc_base/synchronization/mutex.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_video_sink_adapter.h"
#include "rtc_video_source.h"
#include "rtc_video_source_impl.h"
#include "rtc_video_track_impl.h"
#include "src/internal/video_capturer.h"

namespace webrtc {
class VideoCaptureModule;
}  // namespace webrtc

namespace cricket {
class VideoRenderer;
}  // namespace cricket

namespace libwebrtc {

/**
 * class RTCStatsMemberListImpl
 */
class RTCStatsMemberListImpl : public RTCStatsMemberList {
 public:
  RTCStatsMemberListImpl(const vector<scoped_refptr<RTCStatsMember>>& source);
  ~RTCStatsMemberListImpl();
}; // end class RTCStatsMemberListImpl

/**
 * class MediaRTCStatsListImpl
 */
class MediaRTCStatsListImpl : public MediaRTCStatsList {
 public:
  MediaRTCStatsListImpl(const vector<scoped_refptr<MediaRTCStats>>& source);
  ~MediaRTCStatsListImpl();
}; // end class MediaRTCStatsListImpl

/**
 * class RTCPeerConnectionObserverImpl
 */
class RTCPeerConnectionObserverImpl : public RTCPeerConnectionObserver
{
 public:
   RTCPeerConnectionObserverImpl(void* callbacks /* rtcPeerConnectionObserverCallbacks* */);
   ~RTCPeerConnectionObserverImpl();

   void OnSignalingState(RTCSignalingState state) override;

   void OnPeerConnectionState(RTCPeerConnectionState state) override;

   void OnIceGatheringState(RTCIceGatheringState state) override;

   void OnIceConnectionState(RTCIceConnectionState state) override;

   void OnIceCandidate(scoped_refptr<RTCIceCandidate> candidate) override;

   void OnAddStream(scoped_refptr<RTCMediaStream> stream) override;

   void OnRemoveStream(scoped_refptr<RTCMediaStream> stream) override;

   void OnDataChannel(scoped_refptr<RTCDataChannel> data_channel) override;

   void OnRenegotiationNeeded() override;

   void OnTrack(scoped_refptr<RTCRtpTransceiver> transceiver) override;

   void OnAddTrack(vector<scoped_refptr<RTCMediaStream>> streams,
                          scoped_refptr<RTCRtpReceiver> receiver) override;

   void OnRemoveTrack(scoped_refptr<RTCRtpReceiver> receiver) override;

 private:
   void* callbacks_ /* rtcPeerConnectionObserverCallbacks* */;
}; // end class RTCPeerConnectionObserverImpl

/**
 * class RTCPeerConnectionImpl
 */
class RTCPeerConnectionImpl : public RTCPeerConnection,
                              public webrtc::PeerConnectionObserver {
 public:
  virtual bool Initialize();
  virtual bool IsInitialized() const override;

  virtual void CreateOffer(
      OnSdpCreateSuccess success, OnSdpCreateFailure failure,
      scoped_refptr<RTCMediaConstraints> constraints) override;

  virtual void CreateAnswer(
      OnSdpCreateSuccess success, OnSdpCreateFailure failure,
      scoped_refptr<RTCMediaConstraints> constraints) override;

  virtual void SetLocalDescription(const string sdp, const string type,
                                   OnSetSdpSuccess success,
                                   OnSetSdpFailure failure) override;

  virtual void SetRemoteDescription(const string sdp, const string type,
                                    OnSetSdpSuccess success,
                                    OnSetSdpFailure failure) override;

  virtual void GetLocalDescription(OnGetSdpSuccess success,
                                   OnGetSdpFailure failure) override;

  virtual void GetRemoteDescription(OnGetSdpSuccess success,
                                    OnGetSdpFailure failure) override;

  virtual void AddCandidate(const string mid, int midx,
                            const string candiate) override;

  virtual void RestartIce() override;

  virtual void Close() override;

  virtual void RegisterRTCPeerConnectionObserver(
      RTCPeerConnectionObserver* observer) override;

  virtual void DeRegisterRTCPeerConnectionObserver() override;

  virtual scoped_refptr<RTCRtpTransceiver> AddTransceiver(
      scoped_refptr<RTCMediaTrack> track,
      scoped_refptr<RTCRtpTransceiverInit> init) override;

  virtual scoped_refptr<RTCRtpTransceiver> AddTransceiver(
      scoped_refptr<RTCMediaTrack> track) override;

  virtual scoped_refptr<RTCRtpTransceiver> AddTransceiver(
      RTCMediaType media_type) override;

  virtual scoped_refptr<RTCRtpTransceiver> AddTransceiver(
      RTCMediaType media_type,
      scoped_refptr<RTCRtpTransceiverInit> init) override;

  virtual scoped_refptr<RTCRtpSender> AddTrack(
      scoped_refptr<RTCMediaTrack> track, vector<string> streamIds) override;

  virtual bool RemoveTrack(scoped_refptr<RTCRtpSender> render) override;

  virtual vector<scoped_refptr<RTCRtpSender>> senders() override;

  virtual vector<scoped_refptr<RTCRtpTransceiver>> transceivers() override;

  virtual vector<scoped_refptr<RTCRtpReceiver>> receivers() override;

 public:
  virtual int AddStream(scoped_refptr<RTCMediaStream> stream) override;

  virtual int RemoveStream(scoped_refptr<RTCMediaStream> stream) override;

  virtual scoped_refptr<RTCMediaStream> CreateLocalMediaStream(
      const string stream_id) override;

  virtual vector<scoped_refptr<RTCMediaStream>> local_streams() override {
    return local_streams_;
  }

  virtual vector<scoped_refptr<RTCMediaStream>> remote_streams() override {
    return remote_streams_;
  }

  virtual RTCSignalingState signaling_state() override;

  virtual RTCIceConnectionState ice_connection_state() override;

  virtual RTCIceConnectionState standardized_ice_connection_state() override;

  virtual RTCPeerConnectionState peer_connection_state() override;

  virtual RTCIceGatheringState ice_gathering_state() override;

  virtual scoped_refptr<RTCDataChannel> CreateDataChannel(
      const string label, RTCDataChannelInit* dataChannelDict) override;

  virtual bool GetStats(scoped_refptr<RTCRtpSender> sender,
                        OnStatsCollectorSuccess success,
                        OnStatsCollectorFailure failure) override;

  virtual bool GetStats(scoped_refptr<RTCRtpReceiver> receiver,
                        OnStatsCollectorSuccess success,
                        OnStatsCollectorFailure failure) override;

  virtual void GetStats(OnStatsCollectorSuccess success,
                        OnStatsCollectorFailure failure) override;

 public:
  RTCPeerConnectionImpl(
      const RTCConfiguration& configuration,
      scoped_refptr<RTCMediaConstraints> constraints,
      webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
          peer_connection_factory);

 protected:
  ~RTCPeerConnectionImpl();

  virtual void OnAddTrack(
      webrtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
      const std::vector<webrtc::scoped_refptr<webrtc::MediaStreamInterface>>&
          streams) override;

  virtual void OnTrack(
      webrtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) override;

  virtual void OnRemoveTrack(
      webrtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;

  virtual void OnAddStream(
      webrtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;

  virtual void OnRemoveStream(
      webrtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;

  virtual void OnDataChannel(
      webrtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;

  virtual void OnRenegotiationNeeded() override;

  virtual void OnConnectionChange(
      webrtc::PeerConnectionInterface::PeerConnectionState new_state) override;

  virtual void OnIceCandidate(
      const webrtc::IceCandidateInterface* candidate) override;

  virtual void OnIceGatheringChange(
      webrtc::PeerConnectionInterface::IceGatheringState new_state) override;

  virtual void OnIceConnectionChange(
      webrtc::PeerConnectionInterface::IceConnectionState new_state) override;

  virtual void OnSignalingChange(
      webrtc::PeerConnectionInterface::SignalingState new_state) override;

  virtual RTCPeerConnectionObserver* GetObserver() override { return observer_; }

 protected:
  webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
      rtc_peerconnection_factory_;
  webrtc::scoped_refptr<webrtc::PeerConnectionInterface> rtc_peerconnection_;
  const RTCConfiguration& configuration_;
  scoped_refptr<RTCMediaConstraints> constraints_;
  webrtc::PeerConnectionInterface::RTCOfferAnswerOptions offer_answer_options_;
  RTCPeerConnectionObserver* observer_ = nullptr;
  std::unique_ptr<webrtc::Mutex> callback_crt_sec_;
  std::unique_ptr<webrtc::Mutex> initialize_crt_sec_;
  bool initialize_offer_sent = false;
  bool initialized_ = false;
  std::vector<scoped_refptr<RTCMediaStream>> local_streams_;
  std::vector<scoped_refptr<RTCMediaStream>> remote_streams_;
  scoped_refptr<RTCDataChannel> data_channel_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_MEDIA_SESSION_IMPL_HXX