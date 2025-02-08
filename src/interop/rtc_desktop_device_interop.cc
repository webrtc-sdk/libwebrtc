/**
 * File provided for Reference Use Only by isoft (c) 2025.
 * Copyright (c) isoft. All rights reserved.
 * 
 */

#include "rtc_desktop_media_list.h"
#include "rtc_desktop_capturer.h"
#include "rtc_desktop_device.h"
#include "interop_api.h"

using namespace libwebrtc;

#ifdef RTC_DESKTOP_DEVICE

rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopDevice_CreateDesktopCapturer(
    rtcDesktopDeviceHandle desktopDevice,
    rtcDesktopMediaSourceHandle source,
    rtcDesktopCapturerHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_NATIVE_HANDLE(desktopDevice);
    CHECK_POINTER_EX(source, rtcResultU4::kInvalidParameter);

    scoped_refptr<RTCDesktopDevice> pDesktopDevice = static_cast<RTCDesktopDevice*>(desktopDevice);
    scoped_refptr<MediaSource> pSource = static_cast<MediaSource*>(source);
    scoped_refptr<RTCDesktopCapturer> pDesktopCapturer = pDesktopDevice->CreateDesktopCapturer(pSource);
    if (pDesktopCapturer == nullptr) {
        return rtcResultU4::kUnknownError;
    }
    *pOutRetVal = static_cast<rtcDesktopCapturerHandle>(pDesktopCapturer.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopDevice_GetDesktopMediaList(
    rtcDesktopDeviceHandle desktopDevice,
    DesktopType type,
    rtcDesktopMediaListHandle* pOutRetVal
) noexcept
{
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_NATIVE_HANDLE(desktopDevice);

    scoped_refptr<RTCDesktopDevice> pDesktopDevice = static_cast<RTCDesktopDevice*>(desktopDevice);
    scoped_refptr<RTCDesktopMediaList> pMediaList = pDesktopDevice->GetDesktopMediaList(type);
    if (pMediaList == nullptr) {
        return rtcResultU4::kUnknownError;
    }
    *pOutRetVal = static_cast<rtcDesktopMediaListHandle>(pMediaList.release());
    return rtcResultU4::kSuccess;
}

#endif // RTC_DESKTOP_DEVICE
