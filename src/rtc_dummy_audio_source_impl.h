/**
 * File provided for Reference Use Only by isoft (c) 2025.
 * Copyright (c) isoft. All rights reserved.
 * 
 */

#ifndef LIB_WEBRTC_DUMMY_AUDIO_SOURCE_IMPL_HXX
#define LIB_WEBRTC_DUMMY_AUDIO_SOURCE_IMPL_HXX

#include <list>

#include "rtc_types.h"
#include "rtc_dummy_audio_source.h"
#include "src/internal/dummy_audio_capturer.h"

namespace libwebrtc {

/**
 * class DummyAudioSourceObserverImpl
 */
class DummyAudioSourceObserverImpl : public DummyAudioSourceObserver
{
 public:
   DummyAudioSourceObserverImpl(void* callbacks /* rtcAudioSourceObserverCallbacks* */);
   ~DummyAudioSourceObserverImpl();

  void OnStart() override;
  void OnPaused() override;
  void OnStop() override;
  void OnError() override;
  void OnFillBuffer(scoped_refptr<RTCAudioData> audio_data) override;

 private:
   void* callbacks_ /* rtcAudioSourceObserverCallbacks* */;
}; // end class DummyAudioSourceObserverImpl

/**
 * class RTCDummyAudioSourceImpl
 */
class RTCDummyAudioSourceImpl : public RTCDummyAudioSource {
 public:
  RTCDummyAudioSourceImpl(
      rtc::scoped_refptr<webrtc::internal::DummyAudioCapturer> rtc_audio_source
  );

  virtual ~RTCDummyAudioSourceImpl();

  void RegisterAudioDataObserver(DummyAudioSourceObserver* observer) override;
  void DeRegisterAudioDataObserver() override;

  int bits_per_sample() const override;
  int sample_rate_hz() const override;
  size_t number_of_channels() override;
  size_t number_of_frames() override;

  RTCCaptureState Start() override;
  void Stop() override;
  RTCCaptureState CaptureState() override;
  bool IsRunning() const override;

  DummyAudioSourceObserver* GetObserver() override;

  rtc::scoped_refptr<webrtc::AudioSourceInterface> rtc_audio_source() {
    return rtc::scoped_refptr<webrtc::AudioSourceInterface>(
      static_cast<webrtc::AudioSourceInterface*>(rtc_audio_source_.get())
    );
  }
 
 private:
  rtc::scoped_refptr<webrtc::internal::DummyAudioCapturer> rtc_audio_source_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_DUMMY_AUDIO_SOURCE_IMPL_HXX
