#include "rtc_session_description.h"
#include "rtc_sdp_parse_error.h"
#include "interop_api.h"

using namespace libwebrtc;

rtcResultU4 LIB_WEBRTC_CALL
RTCSessionDescription_Create(
    const char* type,
    const char* sdp,
    rtcSdpParseErrorHandle error,
    rtcSessionDescriptionHandle* pOutRetVal
) noexcept {
    CHECK_POINTER(type);
    CHECK_POINTER(sdp);
    CHECK_OUT_POINTER(pOutRetVal);
    
    // Convert C error handle to C++ error object
    scoped_refptr<RTCSdpParseError> pError = nullptr;
    if (error != nullptr) {
        pError = scoped_refptr<RTCSdpParseError>(static_cast<RTCSdpParseError*>(error));
    }
    
    // Create session description
    scoped_refptr<RTCSessionDescription> pDesc = 
        RTCSessionDescription::Create(
            string(type), 
            string(sdp), 
            pError.get()
        );
    
    if (!pDesc) {
        return rtcResultU4::kUnknownError;
    }
    
    *pOutRetVal = static_cast<rtcSessionDescriptionHandle>(pDesc.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCSessionDescription_GetSdp(
    rtcSessionDescriptionHandle handle,
    char* value,
    int sz_value
) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    CHECK_AND_ZERO_MEMORY(value, sz_value);
    
    scoped_refptr<RTCSessionDescription> pDesc = 
        static_cast<RTCSessionDescription*>(handle);
    
    string sdp = pDesc->sdp();
    std::string stdSdp = sdp.std_string();
    size_t len = stdSdp.copy(value, static_cast<size_t>(sz_value));
    
    return stdSdp.size() > len ? rtcResultU4::kBufferTooSmall 
                               : rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCSessionDescription_GetType(
    rtcSessionDescriptionHandle handle,
    char* value,
    int sz_value
) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    CHECK_AND_ZERO_MEMORY(value, sz_value);
    
    scoped_refptr<RTCSessionDescription> pDesc = 
        static_cast<RTCSessionDescription*>(handle);
    
    string type = pDesc->type();
    std::string stdType = type.std_string();
    size_t len = stdType.copy(value, static_cast<size_t>(sz_value));
    
    return stdType.size() > len ? rtcResultU4::kBufferTooSmall 
                               : rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCSessionDescription_GetSdpType(
    rtcSessionDescriptionHandle handle,
    rtcSdpType* pOutRetVal
) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    
    scoped_refptr<RTCSessionDescription> pDesc = 
        static_cast<RTCSessionDescription*>(handle);
    
    RTCSessionDescription::SdpType sdpType = pDesc->GetType();
    *pOutRetVal = static_cast<rtcSdpType>(sdpType);
    
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCSessionDescription_ToString(
    rtcSessionDescriptionHandle handle,
    char* value,
    int sz_value,
    rtcBool32* pOutSuccess
) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    CHECK_AND_ZERO_MEMORY(value, sz_value);
    
    *pOutSuccess = rtcBool32::kFalse;
    
    scoped_refptr<RTCSessionDescription> pDesc = 
        static_cast<RTCSessionDescription*>(handle);
    
    string result;
    bool success = pDesc->ToString(result);
    
    if (success) {
        std::string stdResult = result.c_string();
        size_t len = stdResult.copy(value, static_cast<size_t>(sz_value));
        *pOutSuccess = rtcBool32::kTrue;
        
        return stdResult.size() > len ? rtcResultU4::kBufferTooSmall 
                                     : rtcResultU4::kSuccess;
    }
    
    return rtcResultU4::kSuccess;
}