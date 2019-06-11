#include "rtc_peerconnection_factory_impl.h"
#include "rtc_audio_source_impl.h"
#include "rtc_media_stream_impl.h"
#include "rtc_mediaconstraints_impl.h"
#include "rtc_peerconnection_impl.h"
#include "rtc_video_device_impl.h"
#include "rtc_video_source_impl.h"

#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/create_peerconnection_factory.h"
#include "api/media_stream_interface.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "modules/audio_device/audio_device_impl.h"
#include "rtc_base/bind.h"

#if defined(WEBRTC_IOS)
#include "engine/sdk/objc/Framework/Classes/videotoolboxvideocodecfactory.h"
#endif

namespace libwebrtc {

RTCPeerConnectionFactoryImpl::RTCPeerConnectionFactoryImpl(
    rtc::Thread* worker_thread,
    rtc::Thread* signaling_thread,
    rtc::Thread* network_thread)
    : worker_thread_(worker_thread),
      signaling_thread_(signaling_thread),
      network_thread_(network_thread) {}

RTCPeerConnectionFactoryImpl::~RTCPeerConnectionFactoryImpl() {}

bool RTCPeerConnectionFactoryImpl::Initialize() {
  if (!audio_device_module_) {
    worker_thread_->Invoke<void>(
        RTC_FROM_HERE,
        rtc::Bind(&RTCPeerConnectionFactoryImpl::CreateAudioDeviceModule_w,
                  this));
  }

  if (!rtc_peerconnection_factory_) {
    rtc_peerconnection_factory_ = webrtc::CreatePeerConnectionFactory(
        network_thread_, worker_thread_, signaling_thread_,
        audio_device_module_.get(), webrtc::CreateBuiltinAudioEncoderFactory(),
        webrtc::CreateBuiltinAudioDecoderFactory(),
        webrtc::CreateBuiltinVideoEncoderFactory(),
        webrtc::CreateBuiltinVideoDecoderFactory(), nullptr, nullptr);
  }

  if (!rtc_peerconnection_factory_.get()) {
    Terminate();
    return false;
  }

  return true;
}

bool RTCPeerConnectionFactoryImpl::Terminate() {
  audio_device_impl_ = nullptr;
  video_device_impl_ = nullptr;
  rtc_peerconnection_factory_ = NULL;
  if (audio_device_module_) {
    worker_thread_->Invoke<void>(
        RTC_FROM_HERE,
        rtc::Bind(&RTCPeerConnectionFactoryImpl::DestroyAudioDeviceModule_w,
                  this));
  }

  return true;
}

void RTCPeerConnectionFactoryImpl::CreateAudioDeviceModule_w() {
  if (!audio_device_module_)
    audio_device_module_ = webrtc::AudioDeviceModule::Create(webrtc::AudioDeviceModule::kPlatformDefaultAudio);
}

void RTCPeerConnectionFactoryImpl::DestroyAudioDeviceModule_w() {
  if (audio_device_module_)
    audio_device_module_ = nullptr;
}

scoped_refptr<RTCPeerConnection> RTCPeerConnectionFactoryImpl::Create(
    const RTCConfiguration& configuration,
    scoped_refptr<RTCMediaConstraints> constraints) {
  scoped_refptr<RTCPeerConnection> peerconnection =
      scoped_refptr<RTCPeerConnectionImpl>(
          new RefCountedObject<RTCPeerConnectionImpl>(
              configuration, constraints, rtc_peerconnection_factory_));
  peerconnections_.push_back(peerconnection);
  return peerconnection;
}

void RTCPeerConnectionFactoryImpl::Delete(
    scoped_refptr<RTCPeerConnection> peerconnection) {
  peerconnections_.erase(
      std::remove_if(
          peerconnections_.begin(), peerconnections_.end(),
          [peerconnection](const scoped_refptr<RTCPeerConnection> pc_) {
            return pc_ == peerconnection;
          }),
      peerconnections_.end());
}

scoped_refptr<RTCAudioDevice> RTCPeerConnectionFactoryImpl::GetAudioDevice() {
  if (!audio_device_module_) {
    worker_thread_->Invoke<void>(
        RTC_FROM_HERE,
        rtc::Bind(&RTCPeerConnectionFactoryImpl::CreateAudioDeviceModule_w,
                  this));
  }

  if (!audio_device_impl_)
    audio_device_impl_ = scoped_refptr<AudioDeviceImpl>(
        new RefCountedObject<AudioDeviceImpl>(audio_device_module_));

  return audio_device_impl_;
}

scoped_refptr<RTCVideoDevice> RTCPeerConnectionFactoryImpl::GetVideoDevice() {
  if (!video_device_impl_)
    video_device_impl_ = scoped_refptr<RTCVideoDeviceImpl>(
        new RefCountedObject<RTCVideoDeviceImpl>());

  return video_device_impl_;
}

scoped_refptr<RTCAudioSource> RTCPeerConnectionFactoryImpl::CreateAudioSource(
    const char* audio_source_label) {
  rtc::scoped_refptr<webrtc::AudioSourceInterface> rtc_source_track =
      rtc_peerconnection_factory_->CreateAudioSource(cricket::AudioOptions());

  scoped_refptr<RTCAudioSourceImpl> source = scoped_refptr<RTCAudioSourceImpl>(
      new RefCountedObject<RTCAudioSourceImpl>(rtc_source_track));
  return source;
}

scoped_refptr<RTCVideoSource> RTCPeerConnectionFactoryImpl::CreateVideoSource(
    scoped_refptr<RTCVideoCapturer> capturer,
    const char* video_source_label,
    scoped_refptr<RTCMediaConstraints> constraints) {
  if (rtc::Thread::Current() != signaling_thread_) {
    scoped_refptr<RTCVideoSource> source =
        signaling_thread_->Invoke<scoped_refptr<RTCVideoSource>>(
            RTC_FROM_HERE,
            rtc::Bind(&RTCPeerConnectionFactoryImpl::CreateVideoSource_s, this,
                      capturer, video_source_label, constraints));
    return source;
  }

  return CreateVideoSource_s(capturer, video_source_label, constraints);
}

scoped_refptr<RTCVideoSource> RTCPeerConnectionFactoryImpl::CreateVideoSource_s(
    scoped_refptr<RTCVideoCapturer> capturer,
    const char* video_source_label,
    scoped_refptr<RTCMediaConstraints> constraints) {
  RTCVideoCapturerImpl* capturer_impl =
      static_cast<RTCVideoCapturerImpl*>(capturer.get());
  /*RTCMediaConstraintsImpl* media_constraints =
          static_cast<RTCMediaConstraintsImpl*>(constraints.get());*/
  rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> rtc_source_track =
  new rtc::RefCountedObject<webrtc::internal::CapturerTrackSource>(capturer_impl->video_capturer());
  scoped_refptr<RTCVideoSourceImpl> source = scoped_refptr<RTCVideoSourceImpl>(
      new RefCountedObject<RTCVideoSourceImpl>(rtc_source_track));
  return source;
}

scoped_refptr<RTCMediaStream> RTCPeerConnectionFactoryImpl::CreateStream(
    const char* stream_id) {
  rtc::scoped_refptr<webrtc::MediaStreamInterface> rtc_stream =
      rtc_peerconnection_factory_->CreateLocalMediaStream(stream_id);

  scoped_refptr<MediaStreamImpl> stream = scoped_refptr<MediaStreamImpl>(
      new RefCountedObject<MediaStreamImpl>(rtc_stream));

  return stream;
}

scoped_refptr<RTCVideoTrack> RTCPeerConnectionFactoryImpl::CreateVideoTrack(
    scoped_refptr<RTCVideoSource> source,
    const char* track_id) {
  scoped_refptr<RTCVideoSourceImpl> source_adapter(
      static_cast<RTCVideoSourceImpl*>(source.get()));

  rtc::scoped_refptr<webrtc::VideoTrackInterface> rtc_video_track =
      rtc_peerconnection_factory_->CreateVideoTrack(
          track_id, source_adapter->rtc_source_track());

  scoped_refptr<VideoTrackImpl> video_track = scoped_refptr<VideoTrackImpl>(
      new RefCountedObject<VideoTrackImpl>(rtc_video_track));

  // 	webrtc::VideoTrackProxyWithInternal<webrtc::VideoTrackInterface>
  // *track_proxy =
  // dynamic_cast<webrtc::VideoTrackProxyWithInternal<webrtc::VideoTrackInterface>
  // *>(video_track.get()); 	if (track_proxy) {
  // 		webrtc::MediaStreamTrack<VideoTrackInterface> *track =
  // dynamic_cast<webrtc::MediaStreamTrack<VideoTrackInterface>*>(track_proxy->internal());
  // 		LOG(INFO) << "VideoTrackInterface: " << track->id();
  // 	}

  return video_track;
}

scoped_refptr<RTCAudioTrack> RTCPeerConnectionFactoryImpl::CreateAudioTrack(
    scoped_refptr<RTCAudioSource> source,
    const char* track_id) {
  RTCAudioSourceImpl* source_impl =
      static_cast<RTCAudioSourceImpl*>(source.get());

  rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
      rtc_peerconnection_factory_->CreateAudioTrack(
          track_id, source_impl->rtc_audio_source()));

  scoped_refptr<AudioTrackImpl> track = scoped_refptr<AudioTrackImpl>(
      new RefCountedObject<AudioTrackImpl>(audio_track));
  return track;
}

} // namespace libwebrtc
