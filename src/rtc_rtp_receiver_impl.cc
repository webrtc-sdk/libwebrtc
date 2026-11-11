#include "rtc_rtp_receiver_impl.h"
#include "interop_api.h"

#include "base/refcountedobject.h"
#include "rtc_audio_track_impl.h"
#include "rtc_dtls_transport_impl.h"
#include "rtc_media_stream_impl.h"
#include "rtc_rtp_parameters_impl.h"
#include "rtc_video_track_impl.h"

namespace libwebrtc {

/**
 * class RTCRtpReceiverObserverImpl 
 */

RTCRtpReceiverObserverImpl::RTCRtpReceiverObserverImpl(
  void* callbacks /* rtcRtpReceiverObserverCallbacks* */)
  : callbacks_(nullptr)
{
  if (callbacks) {
    size_t nSize = sizeof(rtcRtpReceiverObserverCallbacks);
    callbacks_ = malloc(nSize);
    memcpy(callbacks_, (const void*)callbacks, nSize);
  }
}

RTCRtpReceiverObserverImpl::~RTCRtpReceiverObserverImpl()
{
  if (callbacks_) {
    free(callbacks_);
  }
  callbacks_ = nullptr;
}

void RTCRtpReceiverObserverImpl::OnFirstPacketReceived(RTCMediaType media_type)
{
  if (callbacks_) {
    rtcRtpReceiverObserverCallbacks* pCallbacks = reinterpret_cast<rtcRtpReceiverObserverCallbacks*>(callbacks_);
    pCallbacks->FirstPacketReceived(
      pCallbacks->UserData,
      static_cast<rtcMediaType>(media_type));
  }
}

/**
 * class RTCRtpReceiverImpl 
 */

RTCRtpReceiverImpl::RTCRtpReceiverImpl(
    webrtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver)
    : rtp_receiver_(rtp_receiver), observer_(nullptr) {}

webrtc::scoped_refptr<webrtc::RtpReceiverInterface>
RTCRtpReceiverImpl::rtp_receiver() {
  return rtp_receiver_;
}

void RTCRtpReceiverImpl::OnFirstPacketReceived(webrtc::MediaType media_type) {
  if (nullptr != observer_) {
    observer_->OnFirstPacketReceived(static_cast<RTCMediaType>(media_type));
  }
}

scoped_refptr<RTCMediaTrack> RTCRtpReceiverImpl::track() const {
  webrtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track =
      rtp_receiver_->track();
  if (nullptr == track.get()) {
    return scoped_refptr<RTCMediaTrack>();
  }
  if (track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind) {
    return scoped_refptr<RTCMediaTrack>(new RefCountedObject<VideoTrackImpl>(
        webrtc::scoped_refptr<webrtc::VideoTrackInterface>(
            static_cast<webrtc::VideoTrackInterface*>(track.get()))));
  } else if (track->kind() == webrtc::MediaStreamTrackInterface::kAudioKind) {
    return scoped_refptr<RTCMediaTrack>(new RefCountedObject<AudioTrackImpl>(
        webrtc::scoped_refptr<webrtc::AudioTrackInterface>(
            static_cast<webrtc::AudioTrackInterface*>(track.get()))));
  }
  return scoped_refptr<RTCMediaTrack>();
}
scoped_refptr<RTCDtlsTransport> RTCRtpReceiverImpl::dtls_transport() const {
  if (nullptr == rtp_receiver_->dtls_transport().get()) {
    return scoped_refptr<RTCDtlsTransport>();
  }

  return new RefCountedObject<RTCDtlsTransportImpl>(
      rtp_receiver_->dtls_transport());
}

const vector<string> RTCRtpReceiverImpl::stream_ids() const {
  std::vector<string> vec;
  for (auto item : rtp_receiver_->stream_ids()) {
    vec.push_back(item);
  }
  return vec;
}

vector<scoped_refptr<RTCMediaStream>> RTCRtpReceiverImpl::streams() const {
  std::vector<scoped_refptr<RTCMediaStream>> streams;
  for (auto item : rtp_receiver_->streams()) {
    streams.push_back(new RefCountedObject<MediaStreamImpl>(item));
  }
  return streams;
}

RTCMediaType RTCRtpReceiverImpl::media_type() const {
  return static_cast<RTCMediaType>(rtp_receiver_->media_type());
}

const string RTCRtpReceiverImpl::id() const { return rtp_receiver_->id(); }
scoped_refptr<RTCRtpParameters> RTCRtpReceiverImpl::parameters() const {
  return new RefCountedObject<RTCRtpParametersImpl>(
      rtp_receiver_->GetParameters());
}
bool RTCRtpReceiverImpl::set_parameters(
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

scoped_refptr<RTCRtpReceiverList> RTCRtpReceiverList::Create(
    const vector<scoped_refptr<RTCRtpReceiver>>& source) {
  return new RefCountedObject<RTCRtpReceiverListImpl>(source);
}

RTCRtpReceiverListImpl::RTCRtpReceiverListImpl(
    const vector<scoped_refptr<RTCRtpReceiver>>& source)
    : RTCRtpReceiverList(source) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor ";
}

RTCRtpReceiverListImpl::~RTCRtpReceiverListImpl() {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
}

}  // namespace libwebrtc
