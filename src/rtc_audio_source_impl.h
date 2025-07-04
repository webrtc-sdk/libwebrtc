#ifndef LIB_WEBRTC_AUDIO_SOURCE_IMPL_HXX
#define LIB_WEBRTC_AUDIO_SOURCE_IMPL_HXX

#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include "common_audio/resampler/include/push_resampler.h"
#include "common_audio/vad/include/webrtc_vad.h"
#include "media/engine/webrtc_video_engine.h"
#include "media/engine/webrtc_voice_engine.h"
#include "pc/media_session.h"
#include "rtc_audio_source.h"
#include "rtc_base/logging.h"
#include "rtc_base/synchronization/mutex.h"
#include "src/internal/local_audio_track.h"

namespace libwebrtc {

class RTCAudioSourceImpl : public RTCAudioSource {
 public:
  RTCAudioSourceImpl(
      rtc::scoped_refptr<libwebrtc::LocalAudioSource> rtc_audio_source,
      SourceType source_type);

  void CaptureFrame(const void* audio_data, int bits_per_sample,
                    int sample_rate, size_t number_of_channels,
                    size_t number_of_frames) override {
    RTC_DCHECK(rtc_audio_source_);
    RTC_DCHECK(audio_data);
    rtc_audio_source_->OnData(audio_data, bits_per_sample, sample_rate,
                              number_of_channels, number_of_frames);
  }

  SourceType GetSourceType() const override { return source_type_; }

  virtual ~RTCAudioSourceImpl();

  rtc::scoped_refptr<webrtc::AudioSourceInterface> rtc_audio_source() {
    return rtc_audio_source_;
  }

 private:
  rtc::scoped_refptr<libwebrtc::LocalAudioSource> rtc_audio_source_;
  SourceType source_type_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_AUDIO_SOURCE_IMPL_HXX
