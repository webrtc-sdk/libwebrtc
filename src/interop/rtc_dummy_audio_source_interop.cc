#include "src/rtc_dummy_audio_source_impl.h"
#include "interop_api.h"

using namespace libwebrtc;

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyAudioSource_RegisterAudioDataObserver(
    rtcDummyAudioSourceHandle handle,
    rtcDummyAudioSourceObserverCallbacks* callbacks
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(callbacks, rtcResultU4::kInvalidParameter);

  // unregistered
  RTCDummyAudioSource_DeRegisterAudioDataObserver(handle);
  
  scoped_refptr<RTCDummyAudioSource> p = static_cast<RTCDummyAudioSource*>(handle);
  DummyAudioSourceObserver* pObserver = static_cast<DummyAudioSourceObserver*>(
    new DummyAudioSourceObserverImpl(static_cast<void*>(callbacks))
  );
  p->RegisterAudioDataObserver(pObserver);
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyAudioSource_DeRegisterAudioDataObserver(
    rtcDummyAudioSourceHandle handle
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);

  scoped_refptr<RTCDummyAudioSource> p = static_cast<RTCDummyAudioSource*>(handle);
  DummyAudioSourceObserverImpl* pObserverImpl = static_cast<DummyAudioSourceObserverImpl*>(p->GetObserver());
  p->DeRegisterAudioDataObserver();    
  if (pObserverImpl) {
    delete pObserverImpl;
  }
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyAudioSource_GetBitsPerSample(
    rtcDummyAudioSourceHandle handle,
    int* pRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pRetVal, int, RTCDummyAudioSource, bits_per_sample);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyAudioSource_GetSampleRateHz(
    rtcDummyAudioSourceHandle handle,
    int* pRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pRetVal, int, RTCDummyAudioSource, sample_rate_hz);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyAudioSource_GetNumChannels(
    rtcDummyAudioSourceHandle handle,
    int* pRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pRetVal, int, RTCDummyAudioSource, number_of_channels);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyAudioSource_GetSamplesPerChannel(
    rtcDummyAudioSourceHandle handle,
    int* pRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pRetVal, int, RTCDummyAudioSource, number_of_frames);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyAudioSource_Start(
    rtcDummyAudioSourceHandle handle,
    rtcCaptureState* pRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pRetVal, rtcCaptureState, RTCDummyAudioSource, Start);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyAudioSource_Stop(
    rtcDummyAudioSourceHandle handle
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);
  scoped_refptr<RTCDummyAudioSource> p = static_cast<RTCDummyAudioSource*>(handle);
  p->Stop();
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyAudioSource_GetCaptureState(
    rtcDummyAudioSourceHandle handle,
    rtcCaptureState* pRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pRetVal, rtcCaptureState, RTCDummyAudioSource, CaptureState);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyAudioSource_IsRunning(
    rtcDummyAudioSourceHandle handle,
    rtcBool32* pRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pRetVal, rtcBool32, RTCDummyAudioSource, IsRunning);
}