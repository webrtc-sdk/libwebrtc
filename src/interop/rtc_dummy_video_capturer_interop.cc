#include "src/rtc_dummy_video_capturer_impl.h"
#include "interop_api.h"

using namespace libwebrtc;

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyVideoCapturer_RegisterObserver(
    rtcDummyVideoCapturerHandle handle,
    rtcDummyVideoCapturerObserverCallbacks* callbacks
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(callbacks, rtcResultU4::kInvalidParameter);

  // unregistered
  RTCDummyVideoCapturer_UnregisterObserver(handle);
  
  scoped_refptr<RTCDummyVideoCapturer> p = static_cast<RTCDummyVideoCapturer*>(handle);
  DummyVideoCapturerObserver* pObserver = static_cast<DummyVideoCapturerObserver*>(
    new DummyVideoCapturerObserverImpl(static_cast<void*>(callbacks))
  );
  p->RegisterObserver(pObserver);
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyVideoCapturer_UnregisterObserver(
    rtcDummyVideoCapturerHandle handle
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);

  scoped_refptr<RTCDummyVideoCapturer> p = static_cast<RTCDummyVideoCapturer*>(handle);
  DummyVideoCapturerObserverImpl* pObserverImpl = static_cast<DummyVideoCapturerObserverImpl*>(p->GetObserver());
  p->DeRegisterObserver();    
  if (pObserverImpl) {
    delete pObserverImpl;
  }
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyVideoCapturer_Start1(
    rtcDummyVideoCapturerHandle handle,
    rtcCaptureState* pOutRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pOutRetVal, rtcCaptureState, RTCDummyVideoCapturer, Start);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyVideoCapturer_Start2(
    rtcDummyVideoCapturerHandle handle,
    unsigned int fps,
    rtcCaptureState* pOutRetVal
) noexcept
{
  CHECK_POINTER(pOutRetVal);
  *pOutRetVal = rtcCaptureState::CS_FAILED;
  CHECK_NATIVE_HANDLE(handle);
  if (fps == 0) {
    return rtcResultU4::kInvalidParameter;
  }

  scoped_refptr<RTCDummyVideoCapturer> p = static_cast<RTCDummyVideoCapturer*>(handle);
  *pOutRetVal = static_cast<rtcCaptureState>(p->Start(
    static_cast<uint32_t>(fps)
  ));
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyVideoCapturer_Start3(
    rtcDummyVideoCapturerHandle handle,
    unsigned int fps,
    unsigned int width,
    unsigned int height,
    rtcCaptureState* pOutRetVal
) noexcept
{
  CHECK_POINTER(pOutRetVal);
  *pOutRetVal = rtcCaptureState::CS_FAILED;
  CHECK_NATIVE_HANDLE(handle);
  if (fps == 0 || width == 0 || height == 0) {
    return rtcResultU4::kInvalidParameter;
  }

  scoped_refptr<RTCDummyVideoCapturer> p = static_cast<RTCDummyVideoCapturer*>(handle);
  *pOutRetVal = static_cast<rtcCaptureState>(p->Start(
    static_cast<uint32_t>(fps),
    static_cast<uint32_t>(width),
    static_cast<uint32_t>(height)
  ));
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyVideoCapturer_Stop(
    rtcDummyVideoCapturerHandle handle
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);
    scoped_refptr<RTCDummyVideoCapturer> p = static_cast<RTCDummyVideoCapturer*>(handle);
    p->Stop();
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyVideoCapturer_IsRunning(
    rtcDummyVideoCapturerHandle handle,
    rtcBool32* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcBool32, RTCDummyVideoCapturer, IsRunning);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDummyVideoCapturer_GetState(
    rtcDummyVideoCapturerHandle handle,
    rtcCaptureState* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcCaptureState, RTCDummyVideoCapturer, state);
}
