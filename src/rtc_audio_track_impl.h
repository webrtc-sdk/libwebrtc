#ifndef LIB_WEBRTC_AUDIO_TRACK_IMPL_HXX
#define LIB_WEBRTC_AUDIO_TRACK_IMPL_HXX

#include "rtc_audio_track.h"

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

class AudioTrackImpl : public RTCAudioTrack {
 public:
  AudioTrackImpl(rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track);

  virtual ~AudioTrackImpl();

  virtual const char* kind() const override { return kind_; }

  virtual const char* id() const override { return id_; }

  virtual bool enabled() const override { return rtc_track_->enabled(); }

  virtual bool set_enabled(bool enable) override {
    return rtc_track_->set_enabled(enable);
  }

  rtc::scoped_refptr<webrtc::AudioTrackInterface> rtc_track() {
    return rtc_track_;
  }

  virtual RTCTrackState state() const override {
    return static_cast<RTCTrackState>(rtc_track_->state());
  }

 private:
  rtc::scoped_refptr<webrtc::AudioTrackInterface> rtc_track_;
  char id_[kMaxStringLength], kind_[kShortStringLength];
};

} // namespace libwebrtc

#endif  // LIB_WEBRTC_AUDIO_TRACK_IMPL_HXX
