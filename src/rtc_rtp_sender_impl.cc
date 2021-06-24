#include "rtc_rtp_sender_impl.h"
#include <src/rtc_audio_track_impl.h>
#include <src/rtc_dtls_transport_impl.h>
#include <src/rtc_dtmf_sender_impl.h>
#include <src/rtc_rtp_parameters_impl.h>
#include <src/rtc_video_track_impl.h>

namespace libwebrtc {
RTCRtpSenderImpl::RTCRtpSenderImpl(
    rtc::scoped_refptr<webrtc::RtpSenderInterface> rtp_sender)
    : rtp_sender_(rtp_sender) {}

rtc::scoped_refptr<webrtc::RtpSenderInterface>
libwebrtc::RTCRtpSenderImpl::rtp_sender() {
  return rtp_sender_;
}

bool RTCRtpSenderImpl::SetTrack(scoped_refptr<RTCMediaTrack> track) {
  if (track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind) {
    VideoTrackImpl* impl = static_cast<VideoTrackImpl*>(track.get());
    return rtp_sender_->SetTrack(impl->rtc_track());

  } else if (track->kind() == webrtc::MediaStreamTrackInterface::kAudioKind) {
    AudioTrackImpl* impl = static_cast<AudioTrackImpl*>(track.get());
    return rtp_sender_->SetTrack(impl->rtc_track());
  }
  return false;
}

scoped_refptr<RTCMediaTrack> RTCRtpSenderImpl::GetTrack() const {
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track =
      rtp_sender_->track();

  if (nullptr == track.get()) {
    return scoped_refptr<RTCMediaTrack>();
  }

  if (track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind) {
    return new RefCountedObject<VideoTrackImpl>(
        static_cast<webrtc::VideoTrackInterface*>(track.get()));
  } else if (track->kind() == webrtc::MediaStreamTrackInterface::kAudioKind) {
    return new RefCountedObject<AudioTrackImpl>(
        static_cast<webrtc::AudioTrackInterface*>(track.get()));
  }
  return scoped_refptr<RTCMediaTrack>();
}

scoped_refptr<RTCDtlsTransport> RTCRtpSenderImpl::GetDtlsTransport() const {
  if (nullptr == rtp_sender_->dtls_transport().get()) {
    return scoped_refptr<RTCDtlsTransport>();
  }
  return new RefCountedObject<RTCDtlsTransportImpl>(
      rtp_sender_->dtls_transport());
}

uint32_t RTCRtpSenderImpl::GetSsrc() const {
  return rtp_sender_->ssrc();
}

RTCMediaType RTCRtpSenderImpl::GetMediaType() const {
  return static_cast<RTCMediaType>(rtp_sender_->media_type());
}

void RTCRtpSenderImpl::Id(OnString on) const {
  auto id = rtp_sender_->id();
  on((char*)id.c_str(), id.size());
}

void RTCRtpSenderImpl::GetStreamIds(OnString on) const {
  for (std::string item : rtp_sender_->stream_ids()) {
    on((char*)item.c_str(), item.size());
  }
}

void RTCRtpSenderImpl::SetStreams(OnVectorString on) const {
  std::vector<std::string> list;
  on([&](char* p, size_t size) {
    std::string id(p, size);
    list.push_back(id);
  });
  rtp_sender_->SetStreams(list);
}

void RTCRtpSenderImpl::InitSendEncodings(OnRTCRtpEncodingParameters on) const {
  for (webrtc::RtpEncodingParameters item :
       rtp_sender_->init_send_encodings()) {
    on(new RefCountedObject<RTCRtpEncodingParametersImpl>(item));
  }
}

scoped_refptr<RTCRtpParameters> RTCRtpSenderImpl::GetParameters() const {
  return new RefCountedObject<RTCRtpParametersImpl>(
      rtp_sender_->GetParameters());
}

bool RTCRtpSenderImpl::SetParameters(
    const scoped_refptr<RTCRtpParameters> parameters) {
  RTCRtpParametersImpl* impl =
      static_cast<RTCRtpParametersImpl*>(parameters.get());
  return rtp_sender_->SetParameters(impl->rtp_parameters()).ok();
}

scoped_refptr<RTCDtmfSender> RTCRtpSenderImpl::GetDtmfSender() const {
 
  if (nullptr == rtp_sender_->GetDtmfSender().get()){
    return scoped_refptr<RTCDtmfSender>();
  }
  return new RefCountedObject<RTCDtmfSenderImpl>(rtp_sender_->GetDtmfSender());
}


}  // namespace libwebrtc
