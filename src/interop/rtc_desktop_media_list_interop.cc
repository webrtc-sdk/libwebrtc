#include "rtc_desktop_media_list.h"
#include "interop_api.h"
#include "src/rtc_desktop_media_list_impl.h"

using namespace libwebrtc;

#ifdef RTC_DESKTOP_DEVICE

rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopMediaList_RegisterMediaListObserver(
    rtcDesktopMediaListHandle hMediaList,
    rtcMediaListObserverCallbacks* callbacks
) noexcept
{
    CHECK_NATIVE_HANDLE(hMediaList);
    CHECK_POINTER_EX(callbacks, rtcResultU4::kInvalidParameter);
    
    MediaListObserver* pObserver = static_cast<MediaListObserver*>(new MediaListObserverImpl(callbacks));
    scoped_refptr<RTCDesktopMediaList> pMediaList = static_cast<RTCDesktopMediaList*>(hMediaList);
    pMediaList->RegisterMediaListObserver(pObserver);
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopMediaList_DeRegisterMediaListObserver
(
    rtcDesktopMediaListHandle hMediaList
) noexcept
{
    CHECK_NATIVE_HANDLE(hMediaList);

    scoped_refptr<RTCDesktopMediaListImpl> pMediaList = static_cast<RTCDesktopMediaListImpl*>(hMediaList);
    MediaListObserverImpl* pObserverImpl = static_cast<MediaListObserverImpl*>(pMediaList->GetObserver());
    pMediaList->DeRegisterMediaListObserver();    
    if (pObserverImpl) {
        delete pObserverImpl;
    }
    return rtcResultU4::kSuccess;
}

rtcDesktopType LIB_WEBRTC_CALL
RTCDesktopMediaList_GetType(
    rtcDesktopMediaListHandle hMediaList
) noexcept
{
    CHECK_POINTER_EX(hMediaList, static_cast<rtcDesktopType>(-1));

    scoped_refptr<RTCDesktopMediaListImpl> pMediaList = static_cast<RTCDesktopMediaListImpl*>(hMediaList);
    rtcDesktopType desktopType = static_cast<rtcDesktopType>(pMediaList->type());
    return desktopType;
}

int LIB_WEBRTC_CALL
RTCDesktopMediaList_UpdateSourceList(
    rtcDesktopMediaListHandle hMediaList,
    rtcBool32 force_reload /*= rtcBool32::kFalse*/,
    rtcBool32 get_thumbnail /*= rtcBool32::kTrue*/
) noexcept
{
    CHECK_POINTER_EX(hMediaList, -1);
    
    scoped_refptr<RTCDesktopMediaListImpl> pMediaList = static_cast<RTCDesktopMediaListImpl*>(hMediaList);
    int result = (int)pMediaList->UpdateSourceList(
        force_reload != rtcBool32::kFalse,
        get_thumbnail != rtcBool32::kFalse
    );
    return result;
}

int LIB_WEBRTC_CALL
RTCDesktopMediaList_GetSourceCount(
    rtcDesktopMediaListHandle hMediaList
) noexcept
{
    CHECK_POINTER_EX(hMediaList, -1);

    scoped_refptr<RTCDesktopMediaListImpl> pMediaList = static_cast<RTCDesktopMediaListImpl*>(hMediaList);
    return pMediaList->GetSourceCount();
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDesktopMediaList_GetSource(
    rtcDesktopMediaListHandle hMediaList,
    int index,
    rtcDesktopMediaSourceHandle* pOutRetVal
) noexcept
{
    CHECK_NATIVE_HANDLE(hMediaList);

    scoped_refptr<RTCDesktopMediaListImpl> pMediaList = static_cast<RTCDesktopMediaListImpl*>(hMediaList);
    scoped_refptr<MediaSource> source = pMediaList->GetSource(index);
    *pOutRetVal = static_cast<rtcDesktopMediaSourceHandle>(source.release());
    return rtcResultU4::kSuccess;
}

rtcBool32 LIB_WEBRTC_CALL
RTCDesktopMediaList_GetThumbnail(
    rtcDesktopMediaListHandle hMediaList,
    rtcDesktopMediaSourceHandle hSource,
    rtcBool32 notify /*= rtcBool32::kFalse*/
) noexcept
{
    CHECK_POINTER_EX(hMediaList, rtcBool32::kFalse);
    CHECK_POINTER_EX(hSource, rtcBool32::kFalse);

    scoped_refptr<RTCDesktopMediaListImpl> pMediaList = static_cast<RTCDesktopMediaListImpl*>(hMediaList);
    scoped_refptr<MediaSource> pSource = static_cast<MediaSource*>(hSource);
    return pMediaList->GetThumbnail(pSource, notify != rtcBool32::kFalse)
        ? rtcBool32::kTrue
        : rtcBool32::kFalse;
}

#endif // RTC_DESKTOP_DEVICE
