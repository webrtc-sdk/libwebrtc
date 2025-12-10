#include "rtc_video_device.h"
#include "interop_api.h"

using namespace libwebrtc;

/*
 * ---------------------------------------------------------------------- 
 * RTCVideoDevice interop methods
 * ---------------------------------------------------------------------- 
 */

int LIB_WEBRTC_CALL
RTCVideoDevice_NumberOfDevices(
    rtcVideoDeviceHandle videoDevice
) noexcept
{
    CHECK_POINTER_EX(videoDevice, 0);
    scoped_refptr<RTCVideoDevice> pVideoDevice = static_cast<RTCVideoDevice*>(videoDevice);
    return (int)pVideoDevice->NumberOfDevices();
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoDevice_GetDeviceName(
    rtcVideoDeviceHandle videoDevice,
    int index,
    char* pOutDeviceNameUTF8,
    int cchOutDeviceNameUTF8,
    char* pOutDeviceUniqueIdUTF8,
    int cchOutDeviceUniqueIdUTF8,
    char* pOutProductUniqueIdUTF8,
    int cchOutProductUniqueIdUTF8
) noexcept
{
    CHECK_NATIVE_HANDLE(videoDevice);
    ZERO_MEMORY(pOutDeviceNameUTF8, cchOutDeviceNameUTF8);
    ZERO_MEMORY(pOutDeviceUniqueIdUTF8, cchOutDeviceUniqueIdUTF8);
    ZERO_MEMORY(pOutProductUniqueIdUTF8, cchOutProductUniqueIdUTF8);

    scoped_refptr<RTCVideoDevice> pVideoDevice = static_cast<RTCVideoDevice*>(videoDevice);
    rtcResultU4 result = static_cast<rtcResultU4>(pVideoDevice->GetDeviceName(
        static_cast<uint32_t>(index),
        pOutDeviceNameUTF8, static_cast<uint32_t>(cchOutDeviceNameUTF8),
        pOutDeviceUniqueIdUTF8, static_cast<uint32_t>(cchOutDeviceUniqueIdUTF8),
        pOutProductUniqueIdUTF8, static_cast<uint32_t>(cchOutProductUniqueIdUTF8)
    ));
    return result;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoDevice_CreateVideoCapturer(
    rtcVideoDeviceHandle videoDevice,
    const char* name,
    int index,
    int width,
    int height,
    int target_fps,
    rtcVideoCapturerHandle* pOutRetVal
) noexcept
{
    CHECK_NATIVE_HANDLE(videoDevice);
    scoped_refptr<RTCVideoDevice> pVideoDevice = static_cast<RTCVideoDevice*>(videoDevice);
    scoped_refptr<RTCVideoCapturer> pVideoCapturer = pVideoDevice->Create(
        name,
        static_cast<uint32_t>(index),
        static_cast<size_t>(width),
        static_cast<size_t>(height),
        static_cast<size_t>(target_fps)
    );
    if (pVideoCapturer == nullptr) {
        return rtcResultU4::kUnknownError;
    }
    *pOutRetVal = static_cast<rtcVideoCapturerHandle>(pVideoCapturer.release());
    return rtcResultU4::kSuccess;
}

/*
 * ---------------------------------------------------------------------- 
 * RTCVideoCapturer interop methods
 * ---------------------------------------------------------------------- 
 */

rtcBool32 LIB_WEBRTC_CALL
RTCVideoCapturer_StartCapture(
    rtcVideoCapturerHandle videoCapturer
) noexcept
{
    CHECK_POINTER_EX(videoCapturer, rtcBool32::kFalse);
    scoped_refptr<RTCVideoCapturer> pVideoCapturer = static_cast<RTCVideoCapturer*>(videoCapturer);
    return pVideoCapturer->StartCapture()
        ? rtcBool32::kTrue
        : rtcBool32::kFalse;
}

rtcBool32 LIB_WEBRTC_CALL
RTCVideoCapturer_CaptureStarted(
    rtcVideoCapturerHandle videoCapturer
) noexcept
{
    CHECK_POINTER_EX(videoCapturer, rtcBool32::kFalse);
    scoped_refptr<RTCVideoCapturer> pVideoCapturer = static_cast<RTCVideoCapturer*>(videoCapturer);
    return pVideoCapturer->CaptureStarted()
        ? rtcBool32::kTrue
        : rtcBool32::kFalse;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCVideoCapturer_StopCapture(
    rtcVideoCapturerHandle videoCapturer
) noexcept
{
    CHECK_NATIVE_HANDLE(videoCapturer);
    scoped_refptr<RTCVideoCapturer> pVideoCapturer = static_cast<RTCVideoCapturer*>(videoCapturer);
    pVideoCapturer->StopCapture();
    return rtcResultU4::kSuccess;
}
