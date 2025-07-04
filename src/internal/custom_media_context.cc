#include "src/internal/custom_media_context.h"

#include <pc/channel_interface.h>

#include <memory>
#include <utility>

#include "api/create_peerconnection_factory.h"
#include "api/enable_media.h"
#include "api/environment/environment.h"
#include "api/peer_connection_interface.h"
#include "api/rtc_event_log/rtc_event_log_factory.h"
#include "api/scoped_refptr.h"
#include "api/task_queue/default_task_queue_factory.h"
#include "api/transport/field_trial_based_config.h"
#include "call/create_call.h"
#include "media/base/media_engine.h"
#include "media/engine/webrtc_media_engine.h"
#include "media/engine/webrtc_video_engine.h"
#include "modules/audio_device/include/audio_device.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "pc/media_factory.h"
#include "rtc_base/thread.h"
#include "src/internal/custom_audio_state.h"
#include "src/internal/custom_audio_transport_impl.h"
#include "src/internal/custom_webrtc_voice_engine.h"
#include "src/internal/local_audio_track.h"

namespace webrtc {

class TaskQueueFactory;
class AudioDeviceModule;
using ::cricket::AudioCodec;
using ::cricket::CompositeMediaEngine;
using ::cricket::CustomWebRtcVoiceEngine;
using ::cricket::MediaEngineInterface;
using ::cricket::WebRtcVideoEngine;

class CustomMediaFactoryImpl : public MediaFactory {
 public:
  CustomMediaFactoryImpl(
      webrtc::CreateAudioStateFactory* create_audio_state_factory)
      : create_audio_state_factory_(create_audio_state_factory) {
    RTC_DCHECK(create_audio_state_factory_);
  }
  ~CustomMediaFactoryImpl() override = default;

  std::unique_ptr<Call> CreateCall(const CallConfig& config) override {
    return webrtc::CreateCall(config);
  }

  std::unique_ptr<MediaEngineInterface> CreateMediaEngine(
      const Environment& env,
      PeerConnectionFactoryDependencies& deps) override {
    auto audio_engine = std::make_unique<CustomWebRtcVoiceEngine>(
        create_audio_state_factory_, &env.task_queue_factory(), deps.adm.get(),
        std::move(deps.audio_encoder_factory),
        std::move(deps.audio_decoder_factory), std::move(deps.audio_mixer),
        std::move(deps.audio_processing), std::move(deps.audio_frame_processor),
        env.field_trials());
    auto video_engine = std::make_unique<WebRtcVideoEngine>(
        std::move(deps.video_encoder_factory),
        std::move(deps.video_decoder_factory), env.field_trials());
    return std::make_unique<CompositeMediaEngine>(std::move(audio_engine),
                                                  std::move(video_engine));
  }

 private:
  webrtc::CreateAudioStateFactory* create_audio_state_factory_ = nullptr;
};

rtc::scoped_refptr<libwebrtc::LocalAudioSource>
CustomMediaContext::CreateAudioSource(cricket::AudioOptions* options,
                                      bool is_custom_source) {
  RTC_DCHECK(options);
  // if is_custom_source == true, not using the default audio transport,
  // you can put costom audio frame via LocalAudioSource::CaptureFrame(...)
  // and the audio transport will be null.
  // otherwise, use the default audio transport, audio transport will
  // put audio frame from your platform adm to your
  // LocalAudioSource::SendAudioData(...).
  if (rtc::Thread::Current() != signaling_thread_) {
    return signaling_thread_->BlockingCall([this, options, is_custom_source] {
      return libwebrtc::LocalAudioSource::Create(
          options, is_custom_source ? nullptr : audio_transport_);
    });
  }
  return libwebrtc::LocalAudioSource::Create(
      options, is_custom_source ? nullptr : audio_transport_);
}

rtc::scoped_refptr<webrtc::AudioState> CustomMediaContext::CreateAudioState(
    const webrtc::AudioState::Config& config) {
  auto audio_transport = std::make_unique<CustomAudioTransportImpl>(
      config.audio_mixer.get(), config.audio_processing.get(),
      config.async_audio_processing_factory.get());
  audio_transport_ = audio_transport.get();
  return rtc::make_ref_counted<CustomAudioState>(config,
                                                 std::move(audio_transport));
}

rtc::scoped_refptr<PeerConnectionFactoryInterface>
CustomMediaContext::CreatePeerConnectionFactory(
    rtc::Thread* network_thread, rtc::Thread* worker_thread,
    rtc::Thread* signaling_thread,
    rtc::scoped_refptr<AudioDeviceModule> default_adm,
    rtc::scoped_refptr<AudioEncoderFactory> audio_encoder_factory,
    rtc::scoped_refptr<AudioDecoderFactory> audio_decoder_factory,
    std::unique_ptr<VideoEncoderFactory> video_encoder_factory,
    std::unique_ptr<VideoDecoderFactory> video_decoder_factory,
    rtc::scoped_refptr<AudioMixer> audio_mixer,
    rtc::scoped_refptr<AudioProcessing> audio_processing,
    std::unique_ptr<AudioFrameProcessor> audio_frame_processor,
    std::unique_ptr<FieldTrialsView> field_trials,
    MediaFactory* media_factory) {
  if (!field_trials) {
    field_trials = std::make_unique<webrtc::FieldTrialBasedConfig>();
  }

  PeerConnectionFactoryDependencies dependencies;
  dependencies.network_thread = network_thread;
  dependencies.worker_thread = worker_thread;
  dependencies.signaling_thread = signaling_thread;
  dependencies.task_queue_factory =
      CreateDefaultTaskQueueFactory(field_trials.get());
  dependencies.event_log_factory = std::make_unique<RtcEventLogFactory>();
  dependencies.trials = std::move(field_trials);

  if (network_thread) {
    // TODO(bugs.webrtc.org/13145): Add an rtc::SocketFactory* argument.
    dependencies.socket_factory = network_thread->socketserver();
  }
  dependencies.adm = std::move(default_adm);
  dependencies.audio_encoder_factory = std::move(audio_encoder_factory);
  dependencies.audio_decoder_factory = std::move(audio_decoder_factory);
  dependencies.audio_frame_processor = std::move(audio_frame_processor);
  if (audio_processing) {
    dependencies.audio_processing = std::move(audio_processing);
  } else {
    dependencies.audio_processing = AudioProcessingBuilder().Create();
  }
  dependencies.audio_mixer = std::move(audio_mixer);
  dependencies.video_encoder_factory = std::move(video_encoder_factory);
  dependencies.video_decoder_factory = std::move(video_decoder_factory);

  dependencies.media_factory = std::make_unique<CustomMediaFactoryImpl>(this);

  return CreateModularPeerConnectionFactory(std::move(dependencies));
}

}  // namespace webrtc
