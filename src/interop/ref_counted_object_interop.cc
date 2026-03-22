#include "interop_api.h"

using namespace libwebrtc;

int LIB_WEBRTC_CALL RefCountedObject_AddRef(rtcRefCountedObjectHandle handle) noexcept
{
    if (handle == nullptr) {
        return -1;
    }
    RefCountInterface* p = static_cast<RefCountInterface*>(handle);
    return p->AddRef();
}

int LIB_WEBRTC_CALL RefCountedObject_Release(rtcRefCountedObjectHandle handle) noexcept
{
    if (handle == nullptr) {
        return -1;
    }
    RefCountInterface* p = static_cast<RefCountInterface*>(handle);
    return p->Release();
}