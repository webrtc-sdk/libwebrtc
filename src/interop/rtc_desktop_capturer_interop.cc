/**
 * File provided for Reference Use Only by isoft (c) 2025.
 * Copyright (c) isoft. All rights reserved.
 * 
 */

#include "rtc_desktop_capturer.h"
#include "src/rtc_desktop_capturer_impl.h"
#include "interop_api.h"

using namespace libwebrtc;

#ifdef RTC_DESKTOP_DEVICE

rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopCapturer_RegisterObserver(
    rtcDesktopCapturerHandle handle,
    rtcDesktopCapturerObserverCallbacks* callbacks
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER_EX(callbacks, rtcResultU4::kInvalidParameter);

    // unregistered
    RTCDesktopCapturer_UnregisterObserver(handle);
    
    DesktopCapturerObserver* pObserver = static_cast<DesktopCapturerObserver*>(new DesktopCapturerObserverImpl(callbacks));
    scoped_refptr<RTCDesktopCapturer> p = static_cast<RTCDesktopCapturer*>(handle);
    p->RegisterDesktopCapturerObserver(pObserver);
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopCapturer_UnregisterObserver(
    rtcDesktopCapturerHandle handle
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCDesktopCapturer> p = static_cast<RTCDesktopCapturer*>(handle);
    DesktopCapturerObserverImpl* pObserverImpl = static_cast<DesktopCapturerObserverImpl*>(p->GetObserver());
    p->DeRegisterDesktopCapturerObserver();    
    if (pObserverImpl) {
        delete pObserverImpl;
    }
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopCapturer_Start1(
    rtcDesktopCapturerHandle handle,
    unsigned int fps,
    rtcCaptureState* pOutRetVal
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER(pOutRetVal);
    *pOutRetVal = rtcCaptureState::CS_FAILED;

    scoped_refptr<RTCDesktopCapturer> p = static_cast<RTCDesktopCapturer*>(handle);
    *pOutRetVal = static_cast<rtcCaptureState>(p->Start(static_cast<uint32_t>(fps)));
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopCapturer_Start2(
    rtcDesktopCapturerHandle handle,
    unsigned int fps,
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height,
    rtcCaptureState* pOutRetVal
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER(pOutRetVal);
    *pOutRetVal = rtcCaptureState::CS_FAILED;

    scoped_refptr<RTCDesktopCapturer> p = static_cast<RTCDesktopCapturer*>(handle);
    *pOutRetVal = static_cast<rtcCaptureState>(p->Start(
        static_cast<uint32_t>(fps),
        static_cast<uint32_t>(x),
        static_cast<uint32_t>(y),
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    ));
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopCapturer_Stop(
    rtcDesktopCapturerHandle handle
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);
    scoped_refptr<RTCDesktopCapturer> p = static_cast<RTCDesktopCapturer*>(handle);
    p->Stop();
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopCapturer_IsRunning(
    rtcDesktopCapturerHandle handle,
    rtcBool32* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcBool32, RTCDesktopCapturer, IsRunning);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopCapturer_GetSource(
    rtcDesktopCapturerHandle handle,
    rtcDesktopMediaSourceHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcDesktopMediaSourceHandle, RTCDesktopCapturer, source().release);
}

#endif // RTC_DESKTOP_DEVICE
