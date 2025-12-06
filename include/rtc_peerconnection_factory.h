#ifndef LIB_WEBRTC_RTC_PEERCONNECTION_FACTORY_HXX
#define LIB_WEBRTC_RTC_PEERCONNECTION_FACTORY_HXX

#include "rtc_audio_source.h"
#include "rtc_audio_track.h"
#include "rtc_types.h"
#ifdef RTC_DESKTOP_DEVICE
#include "rtc_desktop_device.h"
#endif
#include "rtc_media_stream.h"
#include "rtc_mediaconstraints.h"
#include "rtc_video_device.h"
#include "rtc_video_source.h"
#include "rtc_dummy_video_capturer.h"
#include "rtc_dummy_audio_source.h"

namespace libwebrtc {

class RTCPeerConnection;
class RTCAudioDevice;
class RTCAudioProcessing;
class RTCVideoDevice;
class RTCRtpCapabilities;

class RTCPeerConnectionFactory : public RefCountInterface {
 public:
  virtual bool Initialize(bool use_dummy_audio = false) = 0;

  virtual bool Terminate() = 0;

  virtual scoped_refptr<RTCPeerConnection> Create(
      const RTCConfiguration& configuration,
      scoped_refptr<RTCMediaConstraints> constraints) = 0;

  virtual void Delete(scoped_refptr<RTCPeerConnection> peerconnection) = 0;

  virtual scoped_refptr<RTCAudioDevice> GetAudioDevice() = 0;

  virtual scoped_refptr<RTCAudioProcessing> GetAudioProcessing() = 0;

  virtual scoped_refptr<RTCVideoDevice> GetVideoDevice() = 0;
#ifdef RTC_DESKTOP_DEVICE
  virtual scoped_refptr<RTCDesktopDevice> GetDesktopDevice() = 0;
#endif
  virtual scoped_refptr<RTCAudioSource> CreateAudioSource(
      const string audio_source_label,
      RTCAudioSource::SourceType source_type =
          RTCAudioSource::SourceType::kMicrophone) = 0;
          
  virtual scoped_refptr<RTCDummyAudioSource> CreateDummyAudioSource(
      const string audio_source_label,
      int sample_rate_hz = 16000,
      uint32_t num_channels = 2
  ) = 0;

  virtual scoped_refptr<RTCVideoSource> CreateVideoSource(
      scoped_refptr<RTCVideoCapturer> capturer, const string video_source_label,
      scoped_refptr<RTCMediaConstraints> constraints) = 0;
#ifdef RTC_DESKTOP_DEVICE
  virtual scoped_refptr<RTCVideoSource> CreateDesktopSource(
      scoped_refptr<RTCDesktopCapturer> capturer,
      const string video_source_label,
      scoped_refptr<RTCMediaConstraints> constraints) = 0;
#endif
  virtual scoped_refptr<RTCDummyVideoCapturer> CreateDummyVideoCapturer(
      uint32_t fps, uint32_t width, uint32_t height) = 0;
  virtual scoped_refptr<RTCVideoSource> CreateDummyVideoSource(
      scoped_refptr<RTCDummyVideoCapturer> capturer, const string video_source_label) = 0;

  virtual scoped_refptr<RTCAudioTrack> CreateAudioTrack(
      scoped_refptr<RTCAudioSource> source, const string track_id) = 0;

  virtual scoped_refptr<RTCAudioTrack> CreateAudioTrack(
      scoped_refptr<RTCDummyAudioSource> source, const string track_id) = 0;

  virtual scoped_refptr<RTCVideoTrack> CreateVideoTrack(
      scoped_refptr<RTCVideoSource> source, const string track_id) = 0;

  virtual scoped_refptr<RTCMediaStream> CreateStream(
      const string stream_id) = 0;

  virtual scoped_refptr<RTCRtpCapabilities> GetRtpSenderCapabilities(
      RTCMediaType media_type) = 0;

  virtual scoped_refptr<RTCRtpCapabilities> GetRtpReceiverCapabilities(
      RTCMediaType media_type) = 0;

  virtual bool GetUseDummyAudio() = 0;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_PEERCONNECTION_FACTORY_HXX