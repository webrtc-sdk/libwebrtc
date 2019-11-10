#ifndef LIB_WEBRTC_MEDIA_SESSION_FACTORY_IMPL_HXX
#define LIB_WEBRTC_MEDIA_SESSION_FACTORY_IMPL_HXX

#include "rtc_audio_device_impl.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_video_device_impl.h"
#include "rtc_desktop_device_impl.h"

#include "api/mediastreaminterface.h"
#include "api/peerconnectioninterface.h"
#include "rtc_base/thread.h"

namespace libwebrtc {

class RTCPeerConnectionFactoryImpl : public RTCPeerConnectionFactory {
 public:
  RTCPeerConnectionFactoryImpl(rtc::Thread* worker_thread,
                               rtc::Thread* signaling_thread,
                               rtc::Thread* network_thread);

  virtual ~RTCPeerConnectionFactoryImpl();

  bool Initialize() override;

  bool Terminate() override;

  scoped_refptr<RTCPeerConnection> Create(const RTCConfiguration& configuration,scoped_refptr<RTCMediaConstraints> constraints) override;

  void Delete(scoped_refptr<RTCPeerConnection> peerconnection) override;

  scoped_refptr<RTCAudioDevice> GetAudioDevice() override;

  scoped_refptr<RTCVideoDevice> GetVideoDevice() override;

  scoped_refptr<RTCDesktopDevice> GetDesktopDevice() override;

  virtual scoped_refptr<RTCAudioSource> CreateAudioSource(const char* audio_source_label) override;

  //virtual scoped_refptr<RTCVideoSource> CreateDesktopSource(scoped_refptr<RTCVideoCapturer> capturer , scoped_refptr<RTCMediaConstraints> constraints) override;
   
  virtual scoped_refptr<RTCVideoSource> CreateVideoSource(scoped_refptr<RTCVideoCapturer> capturer,const char* video_source_label,scoped_refptr<RTCMediaConstraints> constraints) override;

  virtual scoped_refptr<RTCAudioTrack> CreateAudioTrack(scoped_refptr<RTCAudioSource> source,const char* track_id) override;

  virtual scoped_refptr<RTCVideoTrack> CreateVideoTrack(scoped_refptr<RTCVideoSource> source,const char* track_id) override;

  virtual scoped_refptr<RTCMediaStream> CreateStream(const char* stream_id) override;
 

  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>peer_connection_factory() 
  {
    return rtc_peerconnection_factory_;
  }

 protected:
  void CreateAudioDeviceModule_w();

  void DestroyAudioDeviceModule_w();

  scoped_refptr<RTCVideoSource> CreateVideoSource_s(scoped_refptr<RTCVideoCapturer> capturer,const char* video_source_label,scoped_refptr<RTCMediaConstraints> constraints);
  //scoped_refptr<RTCVideoSource> CreateDesktopSource_s(scoped_refptr<RTCDesktopCapturer> capturer,  scoped_refptr<RTCMediaConstraints> constraints);

 private:
  rtc::Thread* worker_thread_ = nullptr;
  rtc::Thread* signaling_thread_ = nullptr;
  rtc::Thread* network_thread_ = nullptr;
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> rtc_peerconnection_factory_;
  rtc::scoped_refptr<webrtc::AudioDeviceModule> audio_device_module_;
  scoped_refptr<AudioDeviceImpl> audio_device_impl_;
  scoped_refptr<DesktopDeviceImpl> desktop_device_impl_;
  scoped_refptr<RTCVideoDeviceImpl> video_device_impl_;
  std::list<scoped_refptr<RTCPeerConnection>> peerconnections_;
};

};  // namespace libwebrtc

#endif  // LIB_WEBRTC_MEDIA_SESSION_FACTORY_IMPL_HXX
