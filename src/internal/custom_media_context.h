#ifndef INTERNAL_CUSTOM_MEDIA_BRIDGE_FACTORY_H_
#define INTERNAL_CUSTOM_MEDIA_BRIDGE_FACTORY_H_

#include <memory>

#include "api/audio/audio_mixer.h"
#include "api/audio_codecs/audio_decoder_factory.h"
#include "api/audio_codecs/audio_encoder_factory.h"
#include "api/peer_connection_interface.h"
#include "api/ref_count.h"
#include "api/scoped_refptr.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "call/audio_state.h"
#include "pc/media_factory.h"
#include "rtc_base/thread.h"
#include "src/internal/custom_webrtc_voice_engine.h"
#include "src/internal/local_audio_track.h"

namespace webrtc {

class CustomAudioTransportImpl;

class CustomMediaContext : public webrtc::CreateAudioStateFactory,
                           public webrtc::RefCountInterface {
 public:
  CustomMediaContext(rtc::Thread* signaling_thread) {
    RTC_DCHECK(signaling_thread);
    signaling_thread_ = signaling_thread;
  }
  rtc::scoped_refptr<libwebrtc::LocalAudioSource> CreateAudioSource(
      cricket::AudioOptions* options, bool is_custom_source = false);

  rtc::scoped_refptr<webrtc::AudioState> CreateAudioState(
      const webrtc::AudioState::Config& config) override;

  rtc::scoped_refptr<PeerConnectionFactoryInterface>
  CreatePeerConnectionFactory(
      rtc::Thread* network_thread, rtc::Thread* worker_thread,
      rtc::Thread* signaling_thread,
      rtc::scoped_refptr<AudioDeviceModule> default_adm,
      rtc::scoped_refptr<AudioEncoderFactory> audio_encoder_factory,
      rtc::scoped_refptr<AudioDecoderFactory> audio_decoder_factory,
      std::unique_ptr<VideoEncoderFactory> video_encoder_factory,
      std::unique_ptr<VideoDecoderFactory> video_decoder_factory,
      rtc::scoped_refptr<AudioMixer> audio_mixer,
      rtc::scoped_refptr<AudioProcessing> audio_processing,
      std::unique_ptr<AudioFrameProcessor> audio_frame_processor = nullptr,
      std::unique_ptr<FieldTrialsView> field_trials = nullptr,
      MediaFactory* media_factory = nullptr);

 private:
  rtc::Thread* signaling_thread_;
  webrtc::CustomAudioTransportImpl* audio_transport_;
};

}  // namespace webrtc

#endif  // INTERNAL_CUSTOM_MEDIA_BRIDGE_FACTORY_H_
