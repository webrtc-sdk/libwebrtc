#ifndef LIB_WEBRTC_MEDIA_SESSION_FACTORY_IMPL_HXX
#define LIB_WEBRTC_MEDIA_SESSION_FACTORY_IMPL_HXX

#include <memory>

#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include "api/task_queue/task_queue_factory.h"
#include "rtc_audio_device_impl.h"
#include "rtc_audio_processing_impl.h"
#include "rtc_base/thread.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_video_device_impl.h"

#ifdef RTC_DESKTOP_DEVICE
#include "rtc_desktop_capturer_impl.h"
#include "rtc_desktop_device_impl.h"
#include "src/internal/desktop_capturer.h"
#endif

#include "src/internal/custom_media_context.h"

namespace libwebrtc {

class RTCPeerConnectionFactoryImpl : public RTCPeerConnectionFactory {
 public:
  RTCPeerConnectionFactoryImpl();

  virtual ~RTCPeerConnectionFactoryImpl();

  bool Initialize() override;

  bool Terminate() override;

  scoped_refptr<RTCPeerConnection> Create(
      const RTCConfiguration& configuration,
      scoped_refptr<RTCMediaConstraints> constraints) override;

  void Delete(scoped_refptr<RTCPeerConnection> peerconnection) override;

  scoped_refptr<RTCAudioDevice> GetAudioDevice() override;

  scoped_refptr<RTCVideoDevice> GetVideoDevice() override;

  scoped_refptr<RTCAudioProcessing> GetAudioProcessing() override;

  virtual scoped_refptr<RTCAudioSource> CreateAudioSource(
      const string audio_source_label,
      RTCAudioSource::SourceType source_type) override;

  virtual scoped_refptr<RTCVideoSource> CreateVideoSource(
      scoped_refptr<RTCVideoCapturer> capturer, const string video_source_label,
      scoped_refptr<RTCMediaConstraints> constraints) override;
#ifdef RTC_DESKTOP_DEVICE
  virtual scoped_refptr<RTCDesktopDevice> GetDesktopDevice() override;
  virtual scoped_refptr<RTCVideoSource> CreateDesktopSource(
      scoped_refptr<RTCDesktopCapturer> capturer,
      const string video_source_label,
      scoped_refptr<RTCMediaConstraints> constraints) override;
#endif
  virtual scoped_refptr<RTCAudioTrack> CreateAudioTrack(
      scoped_refptr<RTCAudioSource> source, const string track_id) override;

  virtual scoped_refptr<RTCVideoTrack> CreateVideoTrack(
      scoped_refptr<RTCVideoSource> source, const string track_id) override;

  virtual scoped_refptr<RTCMediaStream> CreateStream(
      const string stream_id) override;

  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
  peer_connection_factory() {
    return rtc_peerconnection_factory_;
  }

  scoped_refptr<RTCRtpCapabilities> GetRtpSenderCapabilities(
      RTCMediaType media_type) override;

  scoped_refptr<RTCRtpCapabilities> GetRtpReceiverCapabilities(
      RTCMediaType media_type) override;

  rtc::Thread* signaling_thread() { return signaling_thread_.get(); }

 protected:
  void CreateAudioDeviceModule_w();

  void DestroyAudioDeviceModule_w();

  scoped_refptr<RTCVideoSource> CreateVideoSource_s(
      scoped_refptr<RTCVideoCapturer> capturer, const char* video_source_label,
      scoped_refptr<RTCMediaConstraints> constraints);
#ifdef RTC_DESKTOP_DEVICE
  scoped_refptr<RTCVideoSource> CreateDesktopSource_d(
      scoped_refptr<RTCDesktopCapturer> capturer,
      const char* video_source_label,
      scoped_refptr<RTCMediaConstraints> constraints);
#endif
 private:
  std::unique_ptr<rtc::Thread> worker_thread_;
  std::unique_ptr<rtc::Thread> signaling_thread_;
  std::unique_ptr<rtc::Thread> network_thread_;
  rtc::scoped_refptr<webrtc::CustomMediaContext> custom_media_context_;
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
      rtc_peerconnection_factory_;
  rtc::scoped_refptr<webrtc::AudioDeviceModule> audio_device_module_;
  scoped_refptr<AudioDeviceImpl> audio_device_impl_;
  scoped_refptr<RTCAudioProcessingImpl> audio_processing_impl_;
  scoped_refptr<RTCVideoDeviceImpl> video_device_impl_;
#ifdef RTC_DESKTOP_DEVICE
  scoped_refptr<RTCDesktopDeviceImpl> desktop_device_impl_;
#endif
  std::list<scoped_refptr<RTCPeerConnection>> peerconnections_;
  std::unique_ptr<webrtc::TaskQueueFactory> task_queue_factory_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_MEDIA_SESSION_FACTORY_IMPL_HXX
