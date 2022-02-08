#include "rtc_peerconnection_impl.h"

#include <functional>
#include <utility>
#include <vector>

#include "api/data_channel_interface.h"
#include "base/callback_forward.h"
#include "pc/media_session.h"
#include "rtc_base/logging.h"
#include "rtc_data_channel_impl.h"
#include "rtc_ice_candidate_impl.h"
#include "rtc_media_stream_impl.h"
#include "rtc_mediaconstraints_impl.h"
#include "rtc_rtp_receive_imp.h"
#include "rtc_rtp_sender_impl.h"
#include "rtc_rtp_transceiver_impl.h"

using rtc::Thread;

static std::map<libwebrtc::RtcpMuxPolicy,
                webrtc::PeerConnectionInterface::RtcpMuxPolicy>
    rtcp_mux_policy_map = {
        {libwebrtc::RtcpMuxPolicy::kRtcpMuxPolicyNegotiate,
         webrtc::PeerConnectionInterface::kRtcpMuxPolicyNegotiate},
        {libwebrtc::RtcpMuxPolicy::kRtcpMuxPolicyRequire,
         webrtc::PeerConnectionInterface::kRtcpMuxPolicyRequire}};

static std::map<libwebrtc::SdpSemantics, webrtc::SdpSemantics>
    sdp_semantics_map = {
        {libwebrtc::SdpSemantics::kPlanB, webrtc::SdpSemantics::kPlanB},
        {libwebrtc::SdpSemantics::kUnifiedPlan,
         webrtc::SdpSemantics::kUnifiedPlan}};

static std::map<libwebrtc::CandidateNetworkPolicy,
                webrtc::PeerConnectionInterface::CandidateNetworkPolicy>
    candidate_network_policy_map = {
        {libwebrtc::CandidateNetworkPolicy::kCandidateNetworkPolicyAll,
         webrtc::PeerConnectionInterface::kCandidateNetworkPolicyAll},
        {libwebrtc::CandidateNetworkPolicy::kCandidateNetworkPolicyLowCost,
         webrtc::PeerConnectionInterface::kCandidateNetworkPolicyLowCost}};

static std::map<libwebrtc::IceTransportsType,
                webrtc::PeerConnectionInterface::IceTransportsType>
    ice_transport_type_map = {{libwebrtc::IceTransportsType::kAll,
                               webrtc::PeerConnectionInterface::kAll},
                              {libwebrtc::IceTransportsType::kNoHost,
                               webrtc::PeerConnectionInterface::kNoHost},
                              {libwebrtc::IceTransportsType::kNone,
                               webrtc::PeerConnectionInterface::kNone},
                              {libwebrtc::IceTransportsType::kRelay,
                               webrtc::PeerConnectionInterface::kRelay}};

static std::map<libwebrtc::TcpCandidatePolicy,
                webrtc::PeerConnectionInterface::TcpCandidatePolicy>
    tcp_candidate_policy_map = {
        {libwebrtc::TcpCandidatePolicy::kTcpCandidatePolicyDisabled,
         webrtc::PeerConnectionInterface::kTcpCandidatePolicyDisabled},
        {libwebrtc::TcpCandidatePolicy::kTcpCandidatePolicyEnabled,
         webrtc::PeerConnectionInterface::kTcpCandidatePolicyDisabled}};

static std::map<webrtc::PeerConnectionInterface::IceGatheringState,
                libwebrtc::RTCIceGatheringState>
    ice_gathering_state_map = {
        {webrtc::PeerConnectionInterface::kIceGatheringNew,
         libwebrtc::RTCIceGatheringState::RTCIceGatheringStateNew},
        {webrtc::PeerConnectionInterface::kIceGatheringGathering,
         libwebrtc::RTCIceGatheringState::RTCIceGatheringStateGathering},
        {webrtc::PeerConnectionInterface::kIceGatheringComplete,
         libwebrtc::RTCIceGatheringState::RTCIceGatheringStateComplete}};

static std::map<webrtc::PeerConnectionInterface::IceConnectionState,
                libwebrtc::RTCIceConnectionState>
    ice_connection_state_map = {
        {webrtc::PeerConnectionInterface::kIceConnectionNew,
         libwebrtc::RTCIceConnectionState::RTCIceConnectionStateNew},
        {webrtc::PeerConnectionInterface::kIceConnectionChecking,
         libwebrtc::RTCIceConnectionState::RTCIceConnectionStateChecking},
        {webrtc::PeerConnectionInterface::kIceConnectionCompleted,
         libwebrtc::RTCIceConnectionState::RTCIceConnectionStateCompleted},
        {webrtc::PeerConnectionInterface::kIceConnectionConnected,
         libwebrtc::RTCIceConnectionState::RTCIceConnectionStateConnected},
        {webrtc::PeerConnectionInterface::kIceConnectionDisconnected,
         libwebrtc::RTCIceConnectionState::RTCIceConnectionStateDisconnected},
        {webrtc::PeerConnectionInterface::kIceConnectionFailed,
         libwebrtc::RTCIceConnectionState::RTCIceConnectionStateFailed},
        {webrtc::PeerConnectionInterface::kIceConnectionClosed,
         libwebrtc::RTCIceConnectionState::RTCIceConnectionStateClosed},
        {webrtc::PeerConnectionInterface::kIceConnectionMax,
         libwebrtc::RTCIceConnectionState::RTCIceConnectionStateMax}};

static std::map<webrtc::PeerConnectionInterface::SignalingState,
                libwebrtc::RTCSignalingState>
    signaling_state_map = {
        {webrtc::PeerConnectionInterface::kStable,
         libwebrtc::RTCSignalingState::RTCSignalingStateStable},
        {webrtc::PeerConnectionInterface::kHaveLocalOffer,
         libwebrtc::RTCSignalingState::RTCSignalingStateHaveLocalOffer},
        {webrtc::PeerConnectionInterface::kHaveLocalPrAnswer,
         libwebrtc::RTCSignalingState::RTCSignalingStateHaveLocalPrAnswer},
        {webrtc::PeerConnectionInterface::kHaveRemoteOffer,
         libwebrtc::RTCSignalingState::RTCSignalingStateHaveRemoteOffer},
        {webrtc::PeerConnectionInterface::kHaveRemotePrAnswer,
         libwebrtc::RTCSignalingState::RTCSignalingStateHaveRemotePrAnswer},
        {webrtc::PeerConnectionInterface::kClosed,
         libwebrtc::RTCSignalingState::RTCSignalingStateClosed}};

namespace libwebrtc {
class SetSessionDescriptionObserverProxy
    : public webrtc::SetSessionDescriptionObserver {
 public:
  static SetSessionDescriptionObserverProxy* Create(
      OnSetSdpSuccess success_callback,
      OnSetSdpFailure failure_callback) {
    return new rtc::RefCountedObject<SetSessionDescriptionObserverProxy>(
        success_callback, failure_callback);
  }
  virtual void OnSuccess() {
    RTC_LOG(INFO) << __FUNCTION__;
    success_callback_();
  }
  virtual void OnFailure(webrtc::RTCError error) {
    RTC_LOG(INFO) << __FUNCTION__ << " " << error.message();
    failure_callback_(error.message());
  }

 protected:
  SetSessionDescriptionObserverProxy(OnSetSdpSuccess success_callback,
                                     OnSetSdpFailure failure_callback)
      : success_callback_(success_callback),
        failure_callback_(failure_callback) {}
  ~SetSessionDescriptionObserverProxy() {}

 private:
  OnSetSdpSuccess success_callback_;
  OnSetSdpFailure failure_callback_;
};

class CreateSessionDescriptionObserverProxy
    : public webrtc::CreateSessionDescriptionObserver {
 public:
  static CreateSessionDescriptionObserverProxy* Create(
      OnSdpCreateSuccess success_callback,
      OnSdpCreateFailure failure_callback) {
    return new rtc::RefCountedObject<CreateSessionDescriptionObserverProxy>(
        success_callback, failure_callback);
  }

  CreateSessionDescriptionObserverProxy(OnSdpCreateSuccess success_callback,
                                        OnSdpCreateFailure failure_callback)
      : success_callback_(success_callback),
        failure_callback_(failure_callback) {}

 public:
  virtual void OnSuccess(webrtc::SessionDescriptionInterface* desc) {
    std::string sdp;
    desc->ToString(&sdp);
    std::string type = desc->type();
    success_callback_(sdp.c_str(), type.c_str());
  }

  virtual void OnFailure(webrtc::RTCError error) {
    failure_callback_(error.message());
  }

 private:
  OnSdpCreateSuccess success_callback_;
  OnSdpCreateFailure failure_callback_;
};

RTCPeerConnectionImpl::RTCPeerConnectionImpl(
    const RTCConfiguration& configuration,
    scoped_refptr<RTCMediaConstraints> constraints,
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
        peer_connection_factory)
    : rtc_peerconnection_factory_(peer_connection_factory),
      configuration_(configuration),
      constraints_(constraints),
      callback_crt_sec_(new webrtc::Mutex()) {
  RTC_LOG(INFO) << __FUNCTION__ << ": ctor";
  Initialize();
}

RTCPeerConnectionImpl::~RTCPeerConnectionImpl() {
  Close();
  RTC_LOG(INFO) << __FUNCTION__ << ": dtor";
}

void RTCPeerConnectionImpl::OnAddTrack(
    rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
    const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&
        streams) {
  if (nullptr != observer_) {
    std::vector<scoped_refptr<RTCMediaStream>> out_streams;
    for (auto item : streams) {
      out_streams.push_back(new RefCountedObject<MediaStreamImpl>(item));
    }
    scoped_refptr<RTCRtpReceiver> rtc_receiver =
        new RefCountedObject<RTCRtpReceiverImpl>(receiver);
    observer_->OnAddTrack(out_streams, rtc_receiver);
  }
}

void RTCPeerConnectionImpl::OnTrack(
    rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) {
  if (nullptr != observer_) {
    observer_->OnTrack(
        new RefCountedObject<RTCRtpTransceiverImpl>(transceiver));
  }
}

void RTCPeerConnectionImpl::OnRemoveTrack(
    rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) {
  if (nullptr != observer_) {
    observer_->OnRemoveTrack(
        new RefCountedObject<RTCRtpReceiverImpl>(receiver));
  }
}

// Called when a remote stream is added
void RTCPeerConnectionImpl::OnAddStream(
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
  RTC_LOG(INFO) << __FUNCTION__ << " " << stream->id();

  scoped_refptr<MediaStreamImpl> remote_stream = scoped_refptr<MediaStreamImpl>(
      new RefCountedObject<MediaStreamImpl>(stream));

  remote_stream->RegisterRTCPeerConnectionObserver(observer_);

  remote_streams_.push_back(remote_stream);

  if (observer_) {
    observer_->OnAddStream(remote_stream);
  }
}

void RTCPeerConnectionImpl::OnRemoveStream(
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
  RTC_LOG(INFO) << __FUNCTION__ << " " << stream->id();

  MediaStreamImpl* recv_stream = nullptr;

  for (auto kv : remote_streams_) {
    MediaStreamImpl* recv_st = static_cast<MediaStreamImpl*>(kv.get());
    if (recv_st->rtc_media_stream() == stream) {
      recv_stream = recv_st;
    }
  }

  if (nullptr != recv_stream) {
    if (observer_) {
      observer_->OnRemoveStream(recv_stream);
    }

    remote_streams_.erase(
        std::find(remote_streams_.begin(), remote_streams_.end(), recv_stream));
  }
}

void RTCPeerConnectionImpl::OnDataChannel(
    rtc::scoped_refptr<webrtc::DataChannelInterface> rtc_data_channel) {
  data_channel_ = scoped_refptr<RTCDataChannelImpl>(
      new RefCountedObject<RTCDataChannelImpl>(rtc_data_channel));

  if (observer_)
    observer_->OnDataChannel(data_channel_);
}

void RTCPeerConnectionImpl::OnRenegotiationNeeded() {
  if (observer_) {
    observer_->OnRenegotiationNeeded();
  }
}

void RTCPeerConnectionImpl::OnIceGatheringChange(
    webrtc::PeerConnectionInterface::IceGatheringState new_state) {
  if (observer_)
    observer_->OnIceGatheringState(ice_gathering_state_map[new_state]);
}

void RTCPeerConnectionImpl::OnIceConnectionChange(
    webrtc::PeerConnectionInterface::IceConnectionState new_state) {
  if (observer_)
    observer_->OnIceConnectionState(ice_connection_state_map[new_state]);
}

void RTCPeerConnectionImpl::OnSignalingChange(
    webrtc::PeerConnectionInterface::SignalingState new_state) {
  if (observer_)
    observer_->OnSignalingState(signaling_state_map[new_state]);
}

void RTCPeerConnectionImpl::AddCandidate(const string mid,
                                         int mid_mline_index,
                                         const string cand_sdp) {
  webrtc::SdpParseError error;
  webrtc::IceCandidateInterface* candidate = webrtc::CreateIceCandidate(
      to_std_string(mid), mid_mline_index, to_std_string(cand_sdp), &error);
  if (candidate != nullptr)
    rtc_peerconnection_->AddIceCandidate(candidate);
}

void RTCPeerConnectionImpl::OnIceCandidate(
    const webrtc::IceCandidateInterface* candidate) {
  if (!rtc_peerconnection_)
    return;

#if 0
    if (candidate->candidate().protocol() != "tcp")
        return;


  // For loopback test. To save some connecting delay.
  if (type_ == kLoopBack) {
    if (!rtc_peerconnection_->AddIceCandidate(candidate)) {
      RTC_LOG(WARNING) << "Failed to apply the received candidate";
    }
    return;
  }
#endif

  std::string cand_sdp;
  if (observer_ && candidate->ToString(&cand_sdp)) {
    SdpParseError error;
    scoped_refptr<RTCIceCandidate> cand =
        RTCIceCandidate::Create(cand_sdp.c_str(), candidate->sdp_mid().c_str(),
                                candidate->sdp_mline_index(), &error);
    observer_->OnIceCandidate(cand);
  }

  RTC_LOG(INFO) << __FUNCTION__ << ", mid " << candidate->sdp_mid()
                << ", mline " << candidate->sdp_mline_index() << ", sdp"
                << cand_sdp;
}

void RTCPeerConnectionImpl::RegisterRTCPeerConnectionObserver(
    RTCPeerConnectionObserver* observer) {
  webrtc::MutexLock cs(callback_crt_sec_.get());
  observer_ = observer;
}

void RTCPeerConnectionImpl::DeRegisterRTCPeerConnectionObserver() {
  webrtc::MutexLock cs(callback_crt_sec_.get());
  observer_ = nullptr;
}

bool RTCPeerConnectionImpl::Initialize() {
  RTC_DCHECK(rtc_peerconnection_factory_.get() != nullptr);
  RTC_DCHECK(rtc_peerconnection_.get() == nullptr);

  webrtc::PeerConnectionInterface::RTCConfiguration config;
  webrtc::PeerConnectionInterface::IceServers servers;

  config.rtcp_mux_policy =
      webrtc::PeerConnectionInterface::kRtcpMuxPolicyNegotiate;

  config.candidate_network_policy =
      webrtc::PeerConnectionInterface::kCandidateNetworkPolicyAll;

  for (int i = 0; i < kMaxIceServerSize; i++) {
    IceServer ice_server = configuration_.ice_servers[i];
    if (ice_server.uri.size() > 0) {
      webrtc::PeerConnectionInterface::IceServer server;
      server.uri = to_std_string(ice_server.uri);
      server.username = to_std_string(ice_server.username);
      server.password = to_std_string(ice_server.password);
      config.servers.push_back(server);
    }
  }

  config.enable_dtls_srtp =
      configuration_.srtp_type == MediaSecurityType::kDTLS_SRTP;

  config.sdp_semantics = sdp_semantics_map[configuration_.sdp_semantics];
  config.candidate_network_policy =
      candidate_network_policy_map[configuration_.candidate_network_policy];
  config.tcp_candidate_policy =
      tcp_candidate_policy_map[configuration_.tcp_candidate_policy];
  config.type = ice_transport_type_map[configuration_.type];
  config.rtcp_mux_policy = rtcp_mux_policy_map[configuration_.rtcp_mux_policy];

  offer_answer_options_.offer_to_receive_audio =
      configuration_.offer_to_receive_audio;
  offer_answer_options_.offer_to_receive_video =
      configuration_.offer_to_receive_video;

  offer_answer_options_.use_rtp_mux = configuration_.use_rtp_mux;

  RTCMediaConstraintsImpl* media_constraints =
      static_cast<RTCMediaConstraintsImpl*>(constraints_.get());
  CopyConstraintsIntoRtcConfiguration(media_constraints, &config);

  webrtc::PeerConnectionFactoryInterface::Options options;
  options.disable_encryption =
      (configuration_.srtp_type == MediaSecurityType::kSRTP_None);
  // options.network_ignore_mask |= ADAPTER_TYPE_CELLULAR;
  rtc_peerconnection_factory_->SetOptions(options);

  webrtc::PeerConnectionDependencies dependencies(this);
  auto result =
      rtc_peerconnection_factory_->CreatePeerConnectionOrError(config, std::move(dependencies));

  if (!result.ok()) {
    RTC_LOG(WARNING) << "CreatePeerConnection failed";
    Close();
    return false;
  }

  rtc_peerconnection_ = result.MoveValue();
  return true;
}

scoped_refptr<RTCDataChannel> RTCPeerConnectionImpl::CreateDataChannel(
    const string label,
    RTCDataChannelInit* dataChannelDict) {
  webrtc::DataChannelInit init;
  init.id = dataChannelDict->id;
  init.maxRetransmits = dataChannelDict->maxRetransmits;
  init.maxRetransmitTime = dataChannelDict->maxRetransmitTime;
  init.negotiated = dataChannelDict->negotiated;
  init.ordered = dataChannelDict->ordered;
  init.protocol = to_std_string(dataChannelDict->protocol);
  init.reliable = dataChannelDict->reliable;

  rtc::scoped_refptr<webrtc::DataChannelInterface> rtc_data_channel =
      rtc_peerconnection_->CreateDataChannel(to_std_string(label), &init);

  data_channel_ = scoped_refptr<RTCDataChannelImpl>(
      new RefCountedObject<RTCDataChannelImpl>(rtc_data_channel));

  dataChannelDict->id = init.id;
  return data_channel_;
}

void RTCPeerConnectionImpl::SetLocalDescription(const string sdp,
                                                const string type,
                                                OnSetSdpSuccess success,
                                                OnSetSdpFailure failure) {
  webrtc::SdpParseError error;
  webrtc::SessionDescriptionInterface* session_description(
      webrtc::CreateSessionDescription(to_std_string(type), to_std_string(sdp), &error));

  if (!session_description) {
    std::string error = "Can't parse received session description message.";
    RTC_LOG(WARNING) << error;
    failure(error.c_str());
    return;
  }

  rtc_peerconnection_->SetLocalDescription(
      SetSessionDescriptionObserverProxy::Create(success, failure),
      session_description);
}

void RTCPeerConnectionImpl::SetRemoteDescription(const string sdp,
                                                 const string type,
                                                 OnSetSdpSuccess success,
                                                 OnSetSdpFailure failure) {
  RTC_LOG(INFO) << " Received session description :" << to_std_string(sdp);
  webrtc::SdpParseError error;
  webrtc::SessionDescriptionInterface* session_description(
      webrtc::CreateSessionDescription(to_std_string(type), to_std_string(sdp), &error));

  if (!session_description) {
    std::string error = "Can't parse received session description message.";
    RTC_LOG(WARNING) << error;
    failure(error.c_str());
    return;
  }

  cricket::MediaContentDescription* content_desc =
      session_description->description()->GetContentDescriptionByName("video");
  cricket::MediaContentDescription* media_content_desc =
      (cricket::MediaContentDescription*)content_desc;

  if (media_content_desc && configuration_.local_video_bandwidth > 0)
    media_content_desc->set_bandwidth(configuration_.local_video_bandwidth *
                                      1000);

  rtc_peerconnection_->SetRemoteDescription(
      SetSessionDescriptionObserverProxy::Create(success, failure),
      session_description);

  return;
}

void RTCPeerConnectionImpl::GetLocalDescription(OnGetSdpSuccess success,
                                                OnGetSdpFailure failure) {
  auto local_description = rtc_peerconnection_->local_description();
  if (!local_description) {
    if (failure) {
      failure("not local description");
    }
    return;
  }

  if (success) {
    std::string dsp;
    local_description->ToString(&dsp);
    success(dsp.c_str(), webrtc::SdpTypeToString(local_description->GetType()));
  }
}

void RTCPeerConnectionImpl::GetRemoteDescription(OnGetSdpSuccess success,
                                                 OnGetSdpFailure failure) {
  auto local_description = rtc_peerconnection_->local_description();
  if (!local_description) {
    if (failure) {
      failure("not remote description");
    }
    return;
  }

  if (success) {
    std::string dsp;
    local_description->ToString(&dsp);
    success(dsp.c_str(), webrtc::SdpTypeToString(local_description->GetType()));
  }
}

void RTCPeerConnectionImpl::CreateOffer(
    OnSdpCreateSuccess success,
    OnSdpCreateFailure failure,
    scoped_refptr<RTCMediaConstraints> constraints) {
  if (!rtc_peerconnection_.get() || !rtc_peerconnection_factory_.get()) {
    webrtc::MutexLock cs(callback_crt_sec_.get());
    failure("Failed to initialize PeerConnection");
    return;
  }

  RTCMediaConstraintsImpl* media_constraints =
      static_cast<RTCMediaConstraintsImpl*>(constraints.get());
  webrtc::PeerConnectionInterface::RTCOfferAnswerOptions offer_answer_options;
  if (CopyConstraintsIntoOfferAnswerOptions(media_constraints,
                                            &offer_answer_options) == false) {
    offer_answer_options = offer_answer_options_;
  }

  rtc_peerconnection_->CreateOffer(
      CreateSessionDescriptionObserverProxy::Create(success, failure),
      offer_answer_options);
}

void RTCPeerConnectionImpl::CreateAnswer(
    OnSdpCreateSuccess success,
    OnSdpCreateFailure failure,
    scoped_refptr<RTCMediaConstraints> constraints) {
  if (!rtc_peerconnection_.get() || !rtc_peerconnection_factory_.get()) {
    webrtc::MutexLock cs(callback_crt_sec_.get());
    failure("Failed to initialize PeerConnection");
    return;
  }
  RTCMediaConstraintsImpl* media_constraints =
      static_cast<RTCMediaConstraintsImpl*>(constraints.get());
  webrtc::PeerConnectionInterface::RTCOfferAnswerOptions offer_answer_options;
  if (CopyConstraintsIntoOfferAnswerOptions(media_constraints,
                                            &offer_answer_options) == false) {
    offer_answer_options = offer_answer_options_;
  }
  rtc_peerconnection_->CreateAnswer(
      CreateSessionDescriptionObserverProxy::Create(success, failure),
      offer_answer_options);
}

void RTCPeerConnectionImpl::RestartIce() {
  RTC_LOG(INFO) << __FUNCTION__;
  if (rtc_peerconnection_.get()) {
    rtc_peerconnection_->RestartIce();
  }
}

void RTCPeerConnectionImpl::Close() {
  RTC_LOG(INFO) << __FUNCTION__;
  if (rtc_peerconnection_.get()) {
    rtc_peerconnection_ = nullptr;
    data_channel_ = nullptr;
    local_streams_.clear();
    for (auto stream : remote_streams_) {
      if (observer_) {
        observer_->OnRemoveStream(stream);
      }
      /*   stream->GetAudioTracks([&](scoped_refptr<RTCMediaTrack> track) {
           observer_->OnRemoveTrack([&](OnRTCMediaStream on) { on(stream); },
                                    track);
         });
         stream->GetVideoTracks([&](scoped_refptr<RTCMediaTrack> track) {
           observer_->OnRemoveTrack([&](OnRTCMediaStream on) { on(stream); },
                                    track);
         });*/
    }
    remote_streams_.clear();
  }
}

int RTCPeerConnectionImpl::AddStream(scoped_refptr<RTCMediaStream> stream) {
  MediaStreamImpl* send_stream = static_cast<MediaStreamImpl*>(stream.get());
  rtc::scoped_refptr<webrtc::MediaStreamInterface> rtc_media_stream =
      send_stream->rtc_media_stream();

  send_stream->RegisterRTCPeerConnectionObserver(observer_);

  if (std::find(local_streams_.begin(), local_streams_.end(), stream) !=
      local_streams_.end())
    return -1;  // Already added.

  if (!rtc_peerconnection_->AddStream(rtc_media_stream)) {
    RTC_LOG(LS_ERROR) << "Adding stream to PeerConnection failed";
  }

  local_streams_.push_back(stream);
  return 0;
}

int RTCPeerConnectionImpl::RemoveStream(scoped_refptr<RTCMediaStream> stream) {
  MediaStreamImpl* send_stream = static_cast<MediaStreamImpl*>(stream.get());

  rtc::scoped_refptr<webrtc::MediaStreamInterface> rtc_media_stream =
      send_stream->rtc_media_stream();

  if (std::find(local_streams_.begin(), local_streams_.end(), stream) ==
      local_streams_.end())
    return -1;  // Not found.

  rtc_peerconnection_->RemoveStream(rtc_media_stream);

  local_streams_.erase(
      std::find(local_streams_.begin(), local_streams_.end(), stream));
  return 0;
}

bool RTCPeerConnectionImpl::GetStats(
    const RTCAudioTrack* track,
    scoped_refptr<TrackStatsObserver> observer) {
  AudioTrackImpl* impl = static_cast<AudioTrackImpl*>((RTCAudioTrack*)track);
  rtc::scoped_refptr<WebRTCStatsObserver> rtc_observer =
      WebRTCStatsObserver::Create(observer, "stats");
  return rtc_peerconnection_->GetStats(
      rtc_observer.get(), impl->rtc_track(),
      webrtc::PeerConnectionInterface::kStatsOutputLevelDebug);
}

bool RTCPeerConnectionImpl::GetStats(
    const RTCVideoTrack* track,
    scoped_refptr<TrackStatsObserver> observer) {
  VideoTrackImpl* impl = static_cast<VideoTrackImpl*>((RTCVideoTrack*)track);
  rtc::scoped_refptr<WebRTCStatsObserver> rtc_observer =
      WebRTCStatsObserver::Create(observer, "recv");
  return rtc_peerconnection_->GetStats(
      rtc_observer.get(), impl->rtc_track(),
      webrtc::PeerConnectionInterface::kStatsOutputLevelDebug);
}

scoped_refptr<RTCRtpTransceiver> RTCPeerConnectionImpl::AddTransceiver(
    scoped_refptr<RTCMediaTrack> track,
    scoped_refptr<RTCRtpTransceiverInit> init) {
  RTCRtpTransceiverInitImpl* initImpl =
      static_cast<RTCRtpTransceiverInitImpl*>(init.get());

  webrtc::RTCErrorOr<rtc::scoped_refptr<webrtc::RtpTransceiverInterface>>
      errorOr;
  std::string kind = to_std_string(track->kind());
  if (0 == kind.compare(webrtc::MediaStreamTrackInterface::kVideoKind)) {
    VideoTrackImpl* impl = static_cast<VideoTrackImpl*>(track.get());
    errorOr = rtc_peerconnection_->AddTransceiver(
        impl->rtc_track(), initImpl->rtp_transceiver_init());
  } else if (0 == kind.compare(webrtc::MediaStreamTrackInterface::kAudioKind)) {
    AudioTrackImpl* impl = static_cast<AudioTrackImpl*>(track.get());
    errorOr = rtc_peerconnection_->AddTransceiver(
        impl->rtc_track(), initImpl->rtp_transceiver_init());
  }

  if (errorOr.ok()) {
    return new RefCountedObject<RTCRtpTransceiverImpl>(errorOr.value());
  }

  return scoped_refptr<RTCRtpTransceiver>();
}


scoped_refptr<RTCRtpTransceiver> RTCPeerConnectionImpl::AddTransceiver(
    scoped_refptr<RTCMediaTrack> track) {
  webrtc::RTCErrorOr<rtc::scoped_refptr<webrtc::RtpTransceiverInterface>>
      errorOr;
  std::string kind = to_std_string(track->kind());
  if (0 == kind.compare(webrtc::MediaStreamTrackInterface::kVideoKind)) {
    VideoTrackImpl* impl = static_cast<VideoTrackImpl*>(track.get());
    errorOr = rtc_peerconnection_->AddTransceiver(impl->rtc_track());
  } else if (0 == kind.compare(webrtc::MediaStreamTrackInterface::kAudioKind)) {
    AudioTrackImpl* impl = static_cast<AudioTrackImpl*>(track.get());
    errorOr = rtc_peerconnection_->AddTransceiver(impl->rtc_track());
  }

  if (errorOr.ok()) {
    return new RefCountedObject<RTCRtpTransceiverImpl>(errorOr.value());
  }
  // onAdd(scoped_refptr<RTCRtpTransceiver>(), errorOr.error().message());
  return scoped_refptr<RTCRtpTransceiver>();
}

 scoped_refptr<RTCRtpTransceiver> RTCPeerConnectionImpl::AddTransceiver(
    RTCMediaType media_type) {
  webrtc::RTCErrorOr<rtc::scoped_refptr<webrtc::RtpTransceiverInterface>>
       errorOr;
  if (media_type == RTCMediaType::AUDIO) {
    errorOr = rtc_peerconnection_->AddTransceiver(cricket::MediaType::MEDIA_TYPE_AUDIO);
  } else if(media_type == RTCMediaType::VIDEO) {
    errorOr = rtc_peerconnection_->AddTransceiver(
        cricket::MediaType::MEDIA_TYPE_VIDEO);
   }
   if (errorOr.ok()) {
     return new RefCountedObject<RTCRtpTransceiverImpl>(errorOr.value());
   }
   // onAdd(scoped_refptr<RTCRtpTransceiver>(), errorOr.error().message());
   return scoped_refptr<RTCRtpTransceiver>();
 }

scoped_refptr<RTCRtpTransceiver> RTCPeerConnectionImpl::AddTransceiver(
     RTCMediaType media_type,
     scoped_refptr<RTCRtpTransceiverInit> init) {
   RTCRtpTransceiverInitImpl* initImpl =
       static_cast<RTCRtpTransceiverInitImpl*>(init.get());
   webrtc::RTCErrorOr<rtc::scoped_refptr<webrtc::RtpTransceiverInterface>>
       errorOr;
   if (media_type == RTCMediaType::AUDIO) {
     errorOr = rtc_peerconnection_->AddTransceiver(
         cricket::MediaType::MEDIA_TYPE_AUDIO,
         initImpl->rtp_transceiver_init());
   } else if (media_type == RTCMediaType::VIDEO) {
     errorOr = rtc_peerconnection_->AddTransceiver(
         cricket::MediaType::MEDIA_TYPE_VIDEO,
         initImpl->rtp_transceiver_init());
   }
   if (errorOr.ok()) {
     return new RefCountedObject<RTCRtpTransceiverImpl>(errorOr.value());
   }
   // onAdd(scoped_refptr<RTCRtpTransceiver>(), errorOr.error().message());
   return scoped_refptr<RTCRtpTransceiver>();
 }

scoped_refptr<RTCRtpSender> RTCPeerConnectionImpl::AddTrack(
    scoped_refptr<RTCMediaTrack> track,
    vector<string> streamIds) {
  webrtc::RTCErrorOr<rtc::scoped_refptr<webrtc::RtpSenderInterface>> errorOr;

  std::vector<std::string> stream_ids;
  for (auto id : streamIds.std_vector()) {
    stream_ids.push_back(to_std_string(id));
  }
  std::string kind = to_std_string(track->kind());
  if (0 == kind.compare(webrtc::MediaStreamTrackInterface::kVideoKind)) {
    VideoTrackImpl* impl = static_cast<VideoTrackImpl*>(track.get());
    errorOr = rtc_peerconnection_->AddTrack(impl->rtc_track(), stream_ids);
  } else if (0 == kind.compare(webrtc::MediaStreamTrackInterface::kAudioKind)) {
    AudioTrackImpl* impl = static_cast<AudioTrackImpl*>(track.get());
    errorOr = rtc_peerconnection_->AddTrack(impl->rtc_track(), stream_ids);
  }

  if (errorOr.ok()) {
    return new RefCountedObject<RTCRtpSenderImpl>(errorOr.value());
  }

  // onAdd(scoped_refptr<RTCRtpSender>(), errorOr.error().message());
  return scoped_refptr<RTCRtpSender>();
}

bool RTCPeerConnectionImpl::RemoveTrack(scoped_refptr<RTCRtpSender> render) {
  RTCRtpSenderImpl* impl = static_cast<RTCRtpSenderImpl*>(render.get());
  return rtc_peerconnection_->RemoveTrack(impl->rtc_rtp_sender());
}

vector<scoped_refptr<RTCRtpSender>> RTCPeerConnectionImpl::senders() {
  std::vector<scoped_refptr<RTCRtpSender>> vec;
  for (auto item : rtc_peerconnection_->GetSenders()) {
    vec.push_back(new RefCountedObject<RTCRtpSenderImpl>(item));
  }
  return vec;
}

vector<scoped_refptr<RTCRtpTransceiver>> RTCPeerConnectionImpl::transceivers() {
  std::vector<scoped_refptr<RTCRtpTransceiver>> vec;
  for (auto item : rtc_peerconnection_->GetTransceivers()) {
    vec.push_back(new RefCountedObject<RTCRtpTransceiverImpl>(item));
  }
  return vec;
}

vector<scoped_refptr<RTCRtpReceiver>> RTCPeerConnectionImpl::receivers() {
  std::vector<scoped_refptr<RTCRtpReceiver>> vec;
  for (auto item : rtc_peerconnection_->GetReceivers()) {
    vec.push_back(new RefCountedObject<RTCRtpReceiverImpl>(item));
  }
  return vec;
}

void WebRTCStatsObserver::OnComplete(const webrtc::StatsReports& reports) {
  MediaTrackStatistics stats;

  for (auto report : reports) {
    // 		for (auto kv : report->values())
    // 		{
    // 			LOG_WARNING(<< kv.second->display_name() << ": "
    // << kv.second->ToString());
    // 		}

    const webrtc::StatsReport::Value* kv =
        report->FindValue(webrtc::StatsReport::kStatsValueNameBytesReceived);
    if (kv) {
      stats.bytes_received = kv->int64_val();
    }
    kv = report->FindValue(webrtc::StatsReport::kStatsValueNameBytesSent);
    if (kv) {
      stats.bytes_sent = kv->int64_val();
    }

    kv = report->FindValue(webrtc::StatsReport::kStatsValueNameRtt);
    if (kv) {
      stats.rtt = kv->int64_val();
    }

    kv = report->FindValue(webrtc::StatsReport::kStatsValueNamePacketsSent);
    if (kv) {
      stats.packets_sent = kv->int_val();
    }

    kv = report->FindValue(webrtc::StatsReport::kStatsValueNameFrameRateSent);
    if (kv) {
      stats.frame_rate_sent = kv->int_val();
    }

    kv = report->FindValue(
        webrtc::StatsReport::kStatsValueNameFrameRateReceived);
    if (kv) {
      stats.frame_rate_received = kv->int_val();
    }

    kv = report->FindValue(webrtc::StatsReport::kStatsValueNamePacketsReceived);
    if (kv) {
      stats.packets_received = kv->int_val();
    }

    kv = report->FindValue(webrtc::StatsReport::kStatsValueNamePacketsLost);
    if (kv) {
      stats.packets_lost = kv->int_val();
    }

    kv = report->FindValue(webrtc::StatsReport::kStatsValueNameSsrc);
    if (kv) {
      stats.ssrc = kv->int64_val();
    }

    kv = report->FindValue(webrtc::StatsReport::kStatsValueNameTrackId);
    if (kv) {
      stats.msid = string(kv->static_string_val());
    }

    kv = report->FindValue(webrtc::StatsReport::kStatsValueNameMediaType);
    if (kv) {
      stats.kind = string(kv->static_string_val());
    }

    stats.direction = direction_;
  }

  if (observer_)
    observer_->OnComplete(stats);

  observer_ = nullptr;

  this->Release();
}

}  // namespace libwebrtc
