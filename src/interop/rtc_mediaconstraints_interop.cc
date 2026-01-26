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

rtcResultU4 LIB_WEBRTC_CALL
MediaConstraints_AddMandatoryConstraint(
    rtcMediaConstraintsHandle handle,
    const char* key,
    const char* value) noexcept
{
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER_EX(key, rtcResultU4::kInvalidParameter);
    CHECK_POINTER_EX(value, rtcResultU4::kInvalidParameter);

    scoped_refptr<RTCMediaConstraints> media_constraints = static_cast<RTCMediaConstraints*>(handle);
    media_constraints->AddMandatoryConstraint(string(key), string(value));

    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
MediaConstraints_AddOptionalConstraint(
    rtcMediaConstraintsHandle handle,
    const char* key,
    const char* value) noexcept
{
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER_EX(key, rtcResultU4::kInvalidParameter);
    CHECK_POINTER_EX(value, rtcResultU4::kInvalidParameter);

    scoped_refptr<RTCMediaConstraints> media_constraints = static_cast<RTCMediaConstraints*>(handle);
    media_constraints->AddOptionalConstraint(string(key), string(value));

    return rtcResultU4::kSuccess;
}