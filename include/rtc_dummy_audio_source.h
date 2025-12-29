#ifndef LIB_WEBRTC_RTC_DUMMY_AUDIO_SOURCE_HXX
#define LIB_WEBRTC_RTC_DUMMY_AUDIO_SOURCE_HXX

#include "rtc_types.h"
#include "rtc_audio_data.h"

namespace libwebrtc {

/**
 * class DummyAudioSourceObserver
 */
class DummyAudioSourceObserver {
 public:
  virtual void OnStart() = 0;

  virtual void OnPaused() = 0;

  virtual void OnStop() = 0;

  virtual void OnError() = 0;

  virtual void OnFillBuffer(scoped_refptr<RTCAudioData> data) = 0;

 protected:
  virtual ~DummyAudioSourceObserver() {}
}; // class DummyAudioSourceObserver

class RTCDummyAudioSource : public RefCountInterface {
 public:
  virtual void RegisterAudioDataObserver(DummyAudioSourceObserver* observer) = 0;
  virtual void DeRegisterAudioDataObserver() = 0;

  virtual int bits_per_sample() const = 0;
  virtual int sample_rate_hz() const = 0;
  virtual size_t number_of_channels() = 0;
  virtual size_t number_of_frames() = 0;

  virtual RTCCaptureState Start() = 0;
  virtual void Stop() = 0;
  virtual RTCCaptureState CaptureState() = 0;
  virtual bool IsRunning() const = 0;

  virtual DummyAudioSourceObserver* GetObserver() = 0;

 protected:
  virtual ~RTCDummyAudioSource() {}
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_DUMMY_AUDIO_SOURCE_HXX
