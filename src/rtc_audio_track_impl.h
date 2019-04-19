#ifndef LIB_WEBRTC_AUDIO_TRACK_IMPL_HXX
#define LIB_WEBRTC_AUDIO_TRACK_IMPL_HXX

#include "rtc_audio_track.h"

#include "api/mediastreaminterface.h"
#include "api/peerconnectioninterface.h"
#include "api/peerconnectionproxy.h"
#include "common_audio/resampler/include/push_resampler.h"
#include "common_audio/vad/include/webrtc_vad.h"
#include "media/engine/webrtcvideoengine.h"
#include "media/engine/webrtcvoiceengine.h"
#include "pc/mediasession.h"
#include "rtc_base/criticalsection.h"
#include "rtc_base/logging.h"

namespace libwebrtc {

class AudioTrackImpl : public RTCAudioTrack {
 public:
  AudioTrackImpl(rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track);

  virtual ~AudioTrackImpl();

  virtual const char* kind() const override { return rtc_track_->kind().c_str(); }

  virtual const char* id() const override { return rtc_track_->id().c_str(); }

  virtual bool enabled() const override { return rtc_track_->enabled(); }

  virtual bool set_enabled(bool enable) override {
    return rtc_track_->set_enabled(enable);
  }

  rtc::scoped_refptr<webrtc::AudioTrackInterface> rtc_track() {
    return rtc_track_;
  }

 private:
  rtc::scoped_refptr<webrtc::AudioTrackInterface> rtc_track_;
};

};  // namespace libwebrtc

#endif  // LIB_WEBRTC_AUDIO_TRACK_IMPL_HXX
