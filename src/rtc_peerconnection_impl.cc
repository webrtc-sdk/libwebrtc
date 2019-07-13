#include "rtc_peerconnection_impl.h"
#include "rtc_data_channel_impl.h"
#include "rtc_ice_candidate_impl.h"
#include "rtc_media_stream_impl.h"
#include "rtc_mediaconstraints_impl.h"
#include "api/data_channel_interface.h"
#include "base/callback_forward.h"
#include "pc/media_session.h"
#include "rtc_base/bind.h"
#include "rtc_base/callback.h"
#include "rtc_base/logging.h"

#include <functional>
#include <utility>
#include <vector>

using rtc::Thread;

static std::map<libwebrtc::RtcpMuxPolicy,
                webrtc::PeerConnectionInterface::RtcpMuxPolicy>
    rtcp_mux_policy_map = {
        {libwebrtc::kRtcpMuxPolicyNegotiate,
         webrtc::PeerConnectionInterface::kRtcpMuxPolicyNegotiate},
        {libwebrtc::kRtcpMuxPolicyRequire,
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
  virtual void OnFailure(const std::string& error) {
    RTC_LOG(INFO) << __FUNCTION__ << " " << error;
    failure_callback_(error.c_str());
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

  virtual void OnFailure(const std::string& error) {
    failure_callback_(error.c_str());
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
      callback_crt_sec_(new rtc::CriticalSection()) {
  RTC_LOG(INFO) << __FUNCTION__ << ": ctor";
  Initialize();
}

RTCPeerConnectionImpl::~RTCPeerConnectionImpl() {
  Close();
  RTC_LOG(INFO) << __FUNCTION__ << ": dtor";
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

    for (auto track : remote_stream->GetAudioTracks()) {
      observer_->OnAddTrack(remote_stream, track);
    }

    for (auto track : remote_stream->GetVideoTracks()) {
      observer_->OnAddTrack(remote_stream, track);
    }
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

    for (auto track : recv_stream->GetVideoTracks()) {
      observer_->OnRemoveTrack(recv_stream, track);
    }
    for (auto track : recv_stream->GetVideoTracks()) {
      observer_->OnRemoveTrack(recv_stream, track);
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

void RTCPeerConnectionImpl::AddCandidate(const char* mid,
                                         int midx,
                                         const char* candiate) {
  webrtc::SdpParseError error;
  webrtc::IceCandidateInterface* candidate =
      webrtc::CreateIceCandidate(mid, midx, candiate, &error);
  if (!candidate)
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
        CreateRTCIceCandidate(cand_sdp.c_str(), candidate->sdp_mid().c_str(),
                              candidate->sdp_mline_index(), &error);
    observer_->OnIceCandidate(cand);
  }

  RTC_LOG(INFO) << __FUNCTION__ << ", mid " << candidate->sdp_mid()
                << ", mline " << candidate->sdp_mline_index() << ", sdp"
                << cand_sdp;
}

void RTCPeerConnectionImpl::RegisterRTCPeerConnectionObserver(
    RTCPeerConnectionObserver* observer) {
  rtc::CritScope cs(callback_crt_sec_.get());
  observer_ = observer;
}

void RTCPeerConnectionImpl::DeRegisterRTCPeerConnectionObserver() {
  rtc::CritScope cs(callback_crt_sec_.get());
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
    if (strlen(ice_server.uri) > 0) {
      webrtc::PeerConnectionInterface::IceServer server;
      server.uri = ice_server.uri;
      server.username = ice_server.username;
      server.password = ice_server.password;
      config.servers.push_back(server);
    }
  }

  config.enable_dtls_srtp = configuration_.srtp_type == kDTLS_SRTP;

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
  options.disable_encryption = (configuration_.srtp_type == kSRTP_None);
  // options.network_ignore_mask |= ADAPTER_TYPE_CELLULAR;
  rtc_peerconnection_factory_->SetOptions(options);

  rtc_peerconnection_ = rtc_peerconnection_factory_->CreatePeerConnection(
      config, nullptr, nullptr, this);

  if (rtc_peerconnection_.get() == nullptr) {
    RTC_LOG(WARNING) << "CreatePeerConnection failed";
    Close();
    return false;
  }
  return true;
}

scoped_refptr<RTCDataChannel> RTCPeerConnectionImpl::CreateDataChannel(
    const char* label,
    const RTCDataChannelInit* dataChannelDict) {
  webrtc::DataChannelInit init;
  init.id = dataChannelDict->id;
  init.maxRetransmits = dataChannelDict->maxRetransmits;
  init.maxRetransmitTime = dataChannelDict->maxRetransmitTime;
  init.negotiated = dataChannelDict->negotiated;
  init.ordered = dataChannelDict->ordered;
  init.protocol = dataChannelDict->protocol;
  init.reliable = dataChannelDict->reliable;

  rtc::scoped_refptr<webrtc::DataChannelInterface> rtc_data_channel =
      rtc_peerconnection_->CreateDataChannel(label, &init);

  data_channel_ = scoped_refptr<RTCDataChannelImpl>(
      new RefCountedObject<RTCDataChannelImpl>(rtc_data_channel));

  return data_channel_;
}

void RTCPeerConnectionImpl::SetLocalDescription(const char* sdp,
                                                const char* type,
                                                OnSetSdpSuccess success,
                                                OnSetSdpFailure failure) {
  webrtc::SdpParseError error;
  webrtc::SessionDescriptionInterface* session_description(
      webrtc::CreateSessionDescription(type, sdp, &error));

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

void RTCPeerConnectionImpl::SetRemoteDescription(const char* sdp,
                                                 const char* type,
                                                 OnSetSdpSuccess success,
                                                 OnSetSdpFailure failure) {
  RTC_LOG(INFO) << " Received session description :" << sdp;
  webrtc::SdpParseError error;
  webrtc::SessionDescriptionInterface* session_description(
      webrtc::CreateSessionDescription(type, sdp, &error));

  if (!session_description) {
    std::string error = "Can't parse received session description message.";
    RTC_LOG(WARNING) << error;
    failure(error.c_str());
    return;
  }

  cricket::ContentDescription* content_desc =
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

void RTCPeerConnectionImpl::CreateOffer(
    OnSdpCreateSuccess success,
    OnSdpCreateFailure failure,
    scoped_refptr<RTCMediaConstraints> constraints) {
  if (!rtc_peerconnection_.get() || !rtc_peerconnection_factory_.get()) {
    rtc::CritScope cs(callback_crt_sec_.get());
    failure("Failed to initialize PeerConnection");
    return;
  }

  RTCMediaConstraintsImpl* media_constraints =
      static_cast<RTCMediaConstraintsImpl*>(constraints.get());
  webrtc::PeerConnectionInterface::RTCOfferAnswerOptions offer_answer_options;
  if (CopyConstraintsIntoOfferAnswerOptions(
          media_constraints, &offer_answer_options) ==
      false) {
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
    rtc::CritScope cs(callback_crt_sec_.get());
    failure("Failed to initialize PeerConnection");
    return;
  }
  RTCMediaConstraintsImpl* media_constraints =
      static_cast<RTCMediaConstraintsImpl*>(constraints.get());
  webrtc::PeerConnectionInterface::RTCOfferAnswerOptions offer_answer_options;
  if (CopyConstraintsIntoOfferAnswerOptions(
          media_constraints, &offer_answer_options) ==
      false) {
    offer_answer_options = offer_answer_options_;
  }
  rtc_peerconnection_->CreateAnswer(
      CreateSessionDescriptionObserverProxy::Create(success, failure),
      offer_answer_options);
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
      for (auto track : stream->GetAudioTracks()) {
        observer_->OnRemoveTrack(stream, track);
      }
      for (auto track : stream->GetVideoTracks()) {
        observer_->OnRemoveTrack(stream, track);
      }
    }
    remote_streams_.clear();
  }
}

MediaStreamVector RTCPeerConnectionImpl::local_streams() {
  return local_streams_;
}

MediaStreamVector RTCPeerConnectionImpl::remote_streams() {
  return remote_streams_;
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
      strncpy(stats.msid, kv->static_string_val(), kMaxStringLength);
    }

    kv = report->FindValue(webrtc::StatsReport::kStatsValueNameMediaType);
    if (kv) {
      strncpy(stats.kind, kv->static_string_val(), kMaxStringLength);
    }

    strncpy(stats.direction, direction_.c_str(), kMaxStringLength);
  }

  if (observer_)
    observer_->OnComplete(stats);

  observer_ = nullptr;

  this->Release();
}

} // namespace libwebrtc
