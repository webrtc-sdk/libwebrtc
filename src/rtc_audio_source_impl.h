#ifndef LIB_WEBRTC_AUDIO_SOURCE_IMPL_HXX
#define LIB_WEBRTC_AUDIO_SOURCE_IMPL_HXX

#include "rtc_audio_source.h"

#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include "common_audio/resampler/include/push_resampler.h"
#include "common_audio/vad/include/webrtc_vad.h"
#include "media/engine/webrtc_video_engine.h"
#include "media/engine/webrtc_voice_engine.h"
#include "pc/media_session.h"
#include "rtc_base/synchronization/mutex.h"
#include "rtc_base/logging.h"

namespace libwebrtc {

class RTCAudioSourceImpl : public RTCAudioSource {
 public:
  RTCAudioSourceImpl(
      rtc::scoped_refptr<webrtc::AudioSourceInterface> rtc_audio_source);

  virtual ~RTCAudioSourceImpl();

  rtc::scoped_refptr<webrtc::AudioSourceInterface> rtc_audio_source() {
    return rtc_audio_source_;
  }

 private:
  rtc::scoped_refptr<webrtc::AudioSourceInterface> rtc_audio_source_;
};

} // namespace libwebrtc

#endif  // LIB_WEBRTC_AUDIO_SOURCE_IMPL_HXX
