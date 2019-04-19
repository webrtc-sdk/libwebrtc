#ifndef LIB_WEBRTC_AUDIO_SOURCE_IMPL_HXX
#define LIB_WEBRTC_AUDIO_SOURCE_IMPL_HXX

#include "rtc_audio_source.h"

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

};  // namespace libwebrtc

#endif  // LIB_WEBRTC_AUDIO_SOURCE_IMPL_HXX
