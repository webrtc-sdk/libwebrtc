#ifndef LIB_WEBRTC_REF_COUNTED_OBJECT_CAPI
#define LIB_WEBRTC_REF_COUNTED_OBJECT_CAPI

#include "rtc_types_capi.h"

extern "C" {

LIB_WEBRTC_CAPI int RTCRefCountedObject_AddRef(rtcRefCountedObjectHandle handle);

LIB_WEBRTC_CAPI int RTCRefCountedObject_Release(rtcRefCountedObjectHandle handle);

}  // extern "C"

#endif  // LIB_WEBRTC_REF_COUNTED_OBJECT_CAPI