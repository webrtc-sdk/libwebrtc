#include "include/ref_counted_object_capi.h"

#include "base/refcount.h"
#include "base/scoped_ref_ptr.h"

using namespace libwebrtc;

LIB_WEBRTC_CAPI int RTCRefCountedObject_AddRef(
    rtcRefCountedObjectHandle handle) {
  if (handle == nullptr) {
    return -1;
  }
  RefCountInterface* p = static_cast<RefCountInterface*>(handle);
  return p->AddRef();
}

LIB_WEBRTC_CAPI int RTCRefCountedObject_Release(
    rtcRefCountedObjectHandle handle) {
  if (handle == nullptr) {
    return -1;
  }
  RefCountInterface* p = static_cast<RefCountInterface*>(handle);
  return p->Release();
}