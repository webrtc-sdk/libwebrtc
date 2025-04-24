#include "src/internal/local_audio_track.h"

using webrtc::MediaSourceInterface;

namespace libwebrtc {

rtc::scoped_refptr<LocalAudioSource> LocalAudioSource::Create(
    const cricket::AudioOptions* audio_options,
    webrtc::CustomAudioTransportImpl* audio_transport) {
  auto source = rtc::make_ref_counted<LocalAudioSource>(audio_transport);
  source->Initialize(audio_options);
  return source;
}

void LocalAudioSource::Initialize(const cricket::AudioOptions* audio_options) {
  if (!audio_options) return;

  options_ = *audio_options;
}

}  // namespace libwebrtc
