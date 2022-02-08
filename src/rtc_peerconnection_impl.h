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

  virtual void SetLocalDescription(const string sdp,
                                   const string type,
                                   OnSetSdpSuccess success,
                                   OnSetSdpFailure failure) override;

  virtual void SetRemoteDescription(const string sdp,
                                    const string type,
                                    OnSetSdpSuccess success,
                                    OnSetSdpFailure failure) override;

  virtual void GetLocalDescription(OnGetSdpSuccess success,
                                   OnGetSdpFailure failure) override;

  virtual void GetRemoteDescription(OnGetSdpSuccess success,
                                    OnGetSdpFailure failure) override;

  virtual void AddCandidate(const string mid,
                            int midx,
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
      scoped_refptr<RTCMediaTrack> track,
      vector<string> streamIds) override;

  virtual bool RemoveTrack(scoped_refptr<RTCRtpSender> render) override;

  virtual vector<scoped_refptr<RTCRtpSender>> senders() override;

  virtual vector<scoped_refptr<RTCRtpTransceiver>> transceivers() override;

  virtual vector<scoped_refptr<RTCRtpReceiver>> receivers() override;

 public:
  virtual int AddStream(scoped_refptr<RTCMediaStream> stream) override;

  virtual int RemoveStream(scoped_refptr<RTCMediaStream> stream) override;

  virtual vector<scoped_refptr<RTCMediaStream>> local_streams() override {
    return local_streams_;
  }

  virtual vector<scoped_refptr<RTCMediaStream>> remote_streams() override {
    return remote_streams_;
  }

  virtual scoped_refptr<RTCDataChannel> CreateDataChannel(
      const string label,
      RTCDataChannelInit* dataChannelDict) override;

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

  virtual void OnAddTrack(
      rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
      const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&
          streams) override;

  virtual void OnTrack(
      rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) override;

  virtual void OnRemoveTrack(
      rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;

  virtual void OnAddStream(
      rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;

  virtual void OnRemoveStream(
      rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;

  virtual void OnDataChannel(
      rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;

  virtual void OnRenegotiationNeeded() override;

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
  std::vector<scoped_refptr<RTCMediaStream>> local_streams_;
  std::vector<scoped_refptr<RTCMediaStream>> remote_streams_;
  scoped_refptr<RTCDataChannel> data_channel_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_MEDIA_SESSION_IMPL_HXX
