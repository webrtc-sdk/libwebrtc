#include "rtc_mediaconstraints.h"
#include "src/rtc_mediaconstraints_impl.h"
#include "interop_api.h"

using namespace libwebrtc;

rtcMediaConstraintsHandle LIB_WEBRTC_CALL
MediaConstraints_Create() noexcept
{
    scoped_refptr<RTCMediaConstraints> media_constraints = RTCMediaConstraints::Create();
    return static_cast<rtcMediaConstraintsHandle>(media_constraints.release());
}