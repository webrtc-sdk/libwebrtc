#include "rtc_rtp_receive_imp.h"

#include "base/refcountedobject.h"
#include "rtc_audio_track_impl.h"
#include "rtc_dtls_transport_impl.h"
#include "rtc_media_stream_impl.h"
#include "rtc_rtp_parameters_impl.h"
#include "rtc_video_track_impl.h"

namespace libwebrtc {
RTCRtpReceiverImpl::RTCRtpReceiverImpl(
    rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver)
    : rtp_receiver_(rtp_receiver), observer_(nullptr) {}

rtc::scoped_refptr<webrtc::RtpReceiverInterface>
RTCRtpReceiverImpl::rtp_receiver() {
  return rtp_receiver_;
}

void RTCRtpReceiverImpl::OnFirstPacketReceived(cricket::MediaType media_type) {
  if (nullptr != observer_) {
    observer_->OnFirstPacketReceived(static_cast<RTCMediaType>(media_type));
  }
}

scoped_refptr<RTCMediaTrack> RTCRtpReceiverImpl::Track() const {
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track =
      rtp_receiver_->track();

  if (track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind) {
    return new RefCountedObject<VideoTrackImpl>(
        static_cast<webrtc::VideoTrackInterface*>(track.get()));
  } else if (track->kind() == webrtc::MediaStreamTrackInterface::kAudioKind) {
    return new RefCountedObject<AudioTrackImpl>(
        static_cast<webrtc::AudioTrackInterface*>(track.get()));
  }
  return scoped_refptr<RTCMediaTrack>();
}
scoped_refptr<RTCDtlsTransport> RTCRtpReceiverImpl::DtlsTransport() const {
  return new RefCountedObject<RTCDtlsTransportImpl>(
      rtp_receiver_->dtls_transport());
}
void RTCRtpReceiverImpl::StreamIds(OnString on) const {
  for (auto item : rtp_receiver_->stream_ids()) {
    on((char*)item.c_str(), item.size());
  }
}
Vector<scoped_refptr<RTCMediaStream>> RTCRtpReceiverImpl::Streams() const {
  Vector<scoped_refptr<RTCMediaStream>> ret;
  for (auto item : rtp_receiver_->streams()) {
    ret.push_back(new RefCountedObject<MediaStreamImpl>(item));
  }
  return ret;
}
RTCMediaType RTCRtpReceiverImpl::MediaType() const {
  return static_cast<RTCMediaType>(rtp_receiver_->media_type());
}
void RTCRtpReceiverImpl::Id(OnString on) const {
  auto temp = rtp_receiver_->id();
  on((char*)temp.c_str(), temp.size());
}
scoped_refptr<RTCRtpParameters> RTCRtpReceiverImpl::GetParameters() const {
  return new RefCountedObject<RTCRtpParametersImpl>(
      rtp_receiver_->GetParameters());
}
bool RTCRtpReceiverImpl::SetParameters(
    scoped_refptr<RTCRtpParameters> parameters) {
  return rtp_receiver_->SetParameters(
      static_cast<RTCRtpParametersImpl*>(parameters.get())->rtp_parameters());
}
void RTCRtpReceiverImpl::SetObserver(RTCRtpReceiverObserver* observer) {
  observer_ = observer;
  if (nullptr == observer) {
    rtp_receiver_->SetObserver(nullptr);
  } else {
    rtp_receiver_->SetObserver(this);
  }
}

void RTCRtpReceiverImpl::SetJitterBufferMinimumDelay(double delay_seconds) {
  rtp_receiver_->SetJitterBufferMinimumDelay(delay_seconds);
}

}  // namespace libwebrtc
