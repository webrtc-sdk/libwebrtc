#include "rtc_desktop_device_impl.h"
#include "rtc_desktop_media_list.h"
#include "rtc_desktop_capturer.h"
#include "rtc_video_device_impl.h"
#include "rtc_base/thread.h"

namespace libwebrtc {


RTCDesktopDeviceImpl::RTCDesktopDeviceImpl(rtc::Thread* signaling_thread)
    : signaling_thread_(signaling_thread) { }

RTCDesktopDeviceImpl::~RTCDesktopDeviceImpl() {}

scoped_refptr<RTCDesktopCapturer> RTCDesktopDeviceImpl::CreateDesktopCapturer(scoped_refptr<MediaSource> source)  {
  MediaSourceImpl* source_impl = static_cast<MediaSourceImpl*>(source.get());
    return new RefCountedObject<RTCDesktopCapturerImpl>(
            source_impl->type(),
            source_impl->source_id(),
            signaling_thread_,
            source);
}

scoped_refptr<RTCDesktopMediaList> RTCDesktopDeviceImpl::GetDesktopMediaList(DesktopType type) {
    if(desktop_media_lists_.find(type) == desktop_media_lists_.end()) {
    desktop_media_lists_[type] =
        new RefCountedObject<RTCDesktopMediaListImpl>(type, signaling_thread_);
    }
    return desktop_media_lists_[type];
}

}  // namespace libwebrtc
