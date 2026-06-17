#include "rtc_dtls_transport.h"
#include "src/rtc_dtls_transport_impl.h"
#include "interop_api.h"

using namespace libwebrtc;

/*
 * ----------------------------------------------------------------------
 * RTCDtlsTransportInformation interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCDtlsTransportInformation_GetState (
    rtcDtlsTransportInformationHandle handle,
    rtcDtlsTransportState* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcDtlsTransportState, RTCDtlsTransportInformation, state);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDtlsTransportInformation_GetSslCipherSuite (
    rtcDtlsTransportInformationHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCDtlsTransportInformation, ssl_cipher_suite);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDtlsTransportInformation_GetSrtpCipherSuite (
    rtcDtlsTransportInformationHandle handle,
    int* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, int, RTCDtlsTransportInformation, srtp_cipher_suite);
}

/*
 * ----------------------------------------------------------------------
 * RTCDtlsTransport interop methods
 * ----------------------------------------------------------------------
 */

rtcResultU4 LIB_WEBRTC_CALL
RTCDtlsTransport_GetInformation (
    rtcDtlsTransportHandle handle,
    rtcDtlsTransportInformationHandle* pOutRetVal
) noexcept
{
    DECLARE_GET_VALUE(handle, pOutRetVal, rtcDtlsTransportInformationHandle, RTCDtlsTransport, GetInformation().release);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDtlsTransport_RegisterObserver (
    rtcDtlsTransportHandle handle,
    rtcDtlsTransportObserverCallbacks* callbacks
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER_EX(callbacks, rtcResultU4::kInvalidParameter);
    
    // unregistered
    RTCDtlsTransport_UnregisterObserver(handle);

    scoped_refptr<RTCDtlsTransport> p = static_cast<RTCDtlsTransport*>(handle);
    RTCDtlsTransportObserver* pObserver = static_cast<RTCDtlsTransportObserver*>
    (
        new RTCDtlsTransportObserverImpl(static_cast<void*>(callbacks))
    );
    p->RegisterObserver(pObserver);
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCDtlsTransport_UnregisterObserver (
    rtcDtlsTransportHandle handle
) noexcept
{
    CHECK_NATIVE_HANDLE(handle);

    scoped_refptr<RTCDtlsTransport> p = static_cast<RTCDtlsTransport*>(handle);
    RTCDtlsTransportObserverImpl* pObserverImpl = static_cast<RTCDtlsTransportObserverImpl*>(p->GetObserver());
    p->UnregisterObserver();    
    if (pObserverImpl) {
        delete pObserverImpl;
    }
    return rtcResultU4::kSuccess;
}
