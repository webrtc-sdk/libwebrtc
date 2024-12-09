#include "src/rtc_mediaconstraints_impl.h"
#include "src/rtc_peerconnection_factory_impl.h"

#include "interop_api.h"

using namespace libwebrtc;

RTCConfiguration CreateRtcConfiguration(const rtcPeerConnectionConfiguration* configuration)
{
    RTCConfiguration result;
    if (configuration == nullptr) {
        return result;
    }

    for (int i = 0; i < kMaxIceServerSize; i++) {
        result.ice_servers[i].uri = string(configuration->ice_servers[i].uri);
        result.ice_servers[i].username = string(configuration->ice_servers[i].username);
        result.ice_servers[i].password = string(configuration->ice_servers[i].password);
    }
    result.type = configuration->type;
    result.bundle_policy = configuration->bundle_policy;
    result.rtcp_mux_policy = configuration->rtcp_mux_policy;
    result.candidate_network_policy = configuration->candidate_network_policy;
    result.tcp_candidate_policy = configuration->tcp_candidate_policy;
    result.ice_candidate_pool_size = configuration->ice_candidate_pool_size;
    result.srtp_type = configuration->srtp_type;
    result.sdp_semantics = configuration->sdp_semantics;
    result.offer_to_receive_audio = configuration->offer_to_receive_audio != rtcBool32::kFalse;
    result.offer_to_receive_video = configuration->offer_to_receive_video != rtcBool32::kFalse;
    result.disable_ipv6 = configuration->disable_ipv6 != rtcBool32::kFalse;
    result.disable_ipv6_on_wifi = configuration->disable_ipv6_on_wifi != rtcBool32::kFalse;
    result.max_ipv6_networks = configuration->max_ipv6_networks;
    result.disable_link_local_networks = configuration->disable_link_local_networks != rtcBool32::kFalse;
    result.screencast_min_bitrate = configuration->screencast_min_bitrate;
    result.use_rtp_mux = configuration->use_rtp_mux != rtcBool32::kFalse;
    result.local_audio_bandwidth = configuration->local_audio_bandwidth;
    result.local_video_bandwidth = configuration->local_video_bandwidth;

    return result;
}

rtcPeerConnectionFactoryHandle LIB_WEBRTC_CALL
RTCPeerConnectionFactory_Create() noexcept
{
    scoped_refptr<RTCPeerConnectionFactory> rtc_peerconnection_factory =
      scoped_refptr<RTCPeerConnectionFactory>(
          new RefCountedObject<RTCPeerConnectionFactoryImpl>());
    return static_cast<rtcPeerConnectionFactoryHandle>(rtc_peerconnection_factory.release());
} // end RTCPeerConnectionFactory_Create

rtcBool32 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_Initialize(
    rtcPeerConnectionFactoryHandle factory
) noexcept
{
    if (factory == nullptr) {
        return rtcBool32::kFalse;
    }

    scoped_refptr<RTCPeerConnectionFactory> pFactory = static_cast<RTCPeerConnectionFactory*>(factory);
    return pFactory->Initialize()
        ? rtcBool32::kTrue
        : rtcBool32::kFalse;
} // end RTCPeerConnectionFactory_Initialize

rtcBool32 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_Terminate(
    rtcPeerConnectionFactoryHandle factory
) noexcept
{
    if (factory == nullptr) {
        return rtcBool32::kFalse;
    }

    scoped_refptr<RTCPeerConnectionFactory> pFactory = static_cast<RTCPeerConnectionFactory*>(factory);
    return pFactory->Terminate()
        ? rtcBool32::kTrue
        : rtcBool32::kFalse;
} // end RTCPeerConnectionFactory_Terminate

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreatePeerConnection(
    rtcPeerConnectionFactoryHandle factory,
    const rtcPeerConnectionConfiguration* configuration,
    rtcMediaConstraintsHandle constraints,
    rtcPeerConnectionHandle* pRetVal) noexcept
{
    CHECK_OUT_POINTER(pRetVal);
    CHECK_NATIVE_HANDLE(factory);

    RTCConfiguration rtcConfig = CreateRtcConfiguration(configuration);
    scoped_refptr<RTCPeerConnectionFactory> pFactory = static_cast<RTCPeerConnectionFactory*>(factory);
    scoped_refptr<RTCMediaConstraints> media_constraints = (constraints == nullptr)
        ? RTCMediaConstraints::Create()
        : scoped_refptr<RTCMediaConstraints>(static_cast<RTCMediaConstraints*>(constraints));
    scoped_refptr<RTCPeerConnection> peer_connection = pFactory->Create(rtcConfig, media_constraints);

    /// The 'release' operation preserves the pointer.
    *pRetVal = static_cast<rtcPeerConnectionHandle>(peer_connection.release());
    return rtcResultU4::kSuccess;
} // end RTCPeerConnectionFactory_CreatePeerConnection

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_DeletePeerConnection(
    rtcPeerConnectionFactoryHandle factory,
    rtcPeerConnectionHandle handle
) noexcept
{
    CHECK_NATIVE_HANDLE(factory);
    CHECK_POINTER_EX(handle, rtcResultU4::kInvalidParameter);

    scoped_refptr<RTCPeerConnectionFactory> pFactory = static_cast<RTCPeerConnectionFactory*>(factory);
    scoped_refptr<RTCPeerConnection> peer_connection = static_cast<RTCPeerConnection*>(handle);
    pFactory->Delete(peer_connection);

    return rtcResultU4::kSuccess;
} // end RTCPeerConnectionFactory_DeletePeerConnection

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetAudioDevice(
    rtcPeerConnectionFactoryHandle factory,
    rtcAudioDeviceHandle* pRetVal
) noexcept
{
    CHECK_OUT_POINTER(pRetVal);
    CHECK_NATIVE_HANDLE(factory);

    scoped_refptr<RTCPeerConnectionFactory> pFactory = static_cast<RTCPeerConnectionFactory*>(factory);
    scoped_refptr<RTCAudioDevice> audio_device = pFactory->GetAudioDevice();
    
    /// The 'release' operation preserves the pointer.
    *pRetVal = static_cast<rtcAudioDeviceHandle>(audio_device.release());
    return rtcResultU4::kSuccess;
} // end RTCPeerConnectionFactory_GetAudioDevice

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetVideoDevice(
    rtcPeerConnectionFactoryHandle factory,
    rtcVideoDeviceHandle* pRetVal
) noexcept
{
    CHECK_OUT_POINTER(pRetVal);
    CHECK_NATIVE_HANDLE(factory);

    scoped_refptr<RTCPeerConnectionFactory> pFactory = static_cast<RTCPeerConnectionFactory*>(factory);
    scoped_refptr<RTCVideoDevice> video_device = pFactory->GetVideoDevice();
    
    *pRetVal = static_cast<rtcVideoDeviceHandle>(video_device.release());
    return rtcResultU4::kSuccess;
} // end RTCPeerConnectionFactory_GetVideoDevice

#if defined(WEBRTC_WIN) && defined(RTC_DESKTOP_DEVICE)
rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetDesktopDevice(
    rtcPeerConnectionFactoryHandle factory,
    rtcDesktopDeviceHandle* pRetVal
) noexcept
{
    CHECK_OUT_POINTER(pRetVal);
    CHECK_NATIVE_HANDLE(factory);

    scoped_refptr<RTCPeerConnectionFactory> pFactory = static_cast<RTCPeerConnectionFactory*>(factory);
    scoped_refptr<RTCDesktopDevice> desktop_device = pFactory->GetDesktopDevice();
    
    *pRetVal = static_cast<rtcDesktopDeviceHandle>(desktop_device.release());
    return rtcResultU4::kSuccess;
} // end RTCPeerConnectionFactory_GetDesktopDevice
#endif // defined(WEBRTC_WIN) && defined(RTC_DESKTOP_DEVICE)

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateAudioSource(
    rtcPeerConnectionFactoryHandle factory,
    const char* audio_source_label,
    rtcAudioSourceHandle* pRetVal
) noexcept
{
    CHECK_OUT_POINTER(pRetVal);
    CHECK_NATIVE_HANDLE(factory);

    scoped_refptr<RTCPeerConnectionFactory> pFactory = static_cast<RTCPeerConnectionFactory*>(factory);
    scoped_refptr<RTCAudioSource> audio_source = pFactory->CreateAudioSource(string(audio_source_label));

    *pRetVal = static_cast<rtcAudioSourceHandle>(audio_source.release());
    return rtcResultU4::kSuccess;
} // end RTCPeerConnectionFactory_CreateAudioSource

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateVideoSource(
    rtcPeerConnectionFactoryHandle factory,
    rtcVideoCapturerHandle capturer,
    const char* video_source_label,
    rtcMediaConstraintsHandle constraints,
    rtcVideoSourceHandle* pRetVal
) noexcept
{
    CHECK_OUT_POINTER(pRetVal);
    CHECK_NATIVE_HANDLE(factory);
    CHECK_POINTER_EX(capturer, rtcResultU4::kInvalidParameter);

    scoped_refptr<RTCPeerConnectionFactory> pFactory = static_cast<RTCPeerConnectionFactory*>(factory);
    scoped_refptr<RTCVideoCapturer> video_capturer = static_cast<RTCVideoCapturer*>(capturer);
    /// A null value is accepted for 'constraints'.
    scoped_refptr<RTCMediaConstraints> media_constraints = static_cast<RTCMediaConstraints*>(constraints);
    scoped_refptr<RTCVideoSource> video_source = pFactory->CreateVideoSource(video_capturer, string(video_source_label), media_constraints);

    *pRetVal = static_cast<rtcVideoSourceHandle>(video_source.release());
    return rtcResultU4::kSuccess;
} // end RTCPeerConnectionFactory_CreateVideoSource

#if defined(WEBRTC_WIN) && defined(RTC_DESKTOP_DEVICE)
rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateDesktopSource(
    rtcPeerConnectionFactoryHandle factory,
    rtcDesktopCapturerHandle capturer,
    const char* video_source_label,
    rtcMediaConstraintsHandle constraints,
    rtcVideoSourceHandle* pRetVal
) noexcept
{
    CHECK_OUT_POINTER(pRetVal);
    CHECK_NATIVE_HANDLE(factory);
    CHECK_POINTER_EX(capturer, rtcResultU4::kInvalidParameter);

    scoped_refptr<RTCPeerConnectionFactory> pFactory = static_cast<RTCPeerConnectionFactory*>(factory);
    scoped_refptr<RTCDesktopCapturer> desktop_capturer = static_cast<RTCDesktopCapturer*>(capturer);
    /// A null value is accepted for 'constraints'.
    scoped_refptr<RTCMediaConstraints> media_constraints = static_cast<RTCMediaConstraints*>(constraints);
    scoped_refptr<RTCVideoSource> video_source = pFactory->CreateDesktopSource(desktop_capturer, string(video_source_label), media_constraints);
    
    *pRetVal = static_cast<rtcVideoSourceHandle>(video_source.release());
    return rtcResultU4::kSuccess;
} // end RTCPeerConnectionFactory_CreateDesktopSource
#endif // defined(WEBRTC_WIN) && defined(RTC_DESKTOP_DEVICE)

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateAudioTrack(
    rtcPeerConnectionFactoryHandle factory,
    rtcAudioSourceHandle source,
    const char* track_id,
    rtcAudioTrackHandle* pRetVal
) noexcept
{
    CHECK_OUT_POINTER(pRetVal);
    CHECK_NATIVE_HANDLE(factory);
    CHECK_POINTER_EX(source, rtcResultU4::kInvalidParameter);

    scoped_refptr<RTCPeerConnectionFactory> pFactory = static_cast<RTCPeerConnectionFactory*>(factory);
    scoped_refptr<RTCAudioSource> audio_source = static_cast<RTCAudioSource*>(source);
    scoped_refptr<RTCAudioTrack> audio_track = pFactory->CreateAudioTrack(audio_source, string(track_id));

    *pRetVal = static_cast<rtcAudioTrackHandle>(audio_track.release());
    return rtcResultU4::kSuccess;
} // end RTCPeerConnectionFactory_CreateAudioTrack

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateVideoTrack(
    rtcPeerConnectionFactoryHandle factory,
    rtcVideoSourceHandle source,
    const char* track_id,
    rtcVideoTrackHandle* pRetVal
) noexcept
{
    CHECK_OUT_POINTER(pRetVal);
    CHECK_NATIVE_HANDLE(factory);
    CHECK_POINTER_EX(source, rtcResultU4::kInvalidParameter);

    scoped_refptr<RTCPeerConnectionFactory> pFactory = static_cast<RTCPeerConnectionFactory*>(factory);
    scoped_refptr<RTCVideoSource> video_source = static_cast<RTCVideoSource*>(source);
    scoped_refptr<RTCVideoTrack> video_track = pFactory->CreateVideoTrack(video_source, string(track_id));

    *pRetVal = static_cast<rtcVideoTrackHandle>(video_track.release());
    return rtcResultU4::kSuccess;
} // end RTCPeerConnectionFactory_CreateVideoTrack

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_CreateStream(
    rtcPeerConnectionFactoryHandle factory,
    const char* stream_id,
    rtcMediaStreamHandle* pRetVal
) noexcept
{
    CHECK_OUT_POINTER(pRetVal);
    CHECK_NATIVE_HANDLE(factory);

    scoped_refptr<RTCPeerConnectionFactory> pFactory = static_cast<RTCPeerConnectionFactory*>(factory);
    scoped_refptr<RTCMediaStream> media_stream = pFactory->CreateStream(string(stream_id));

    *pRetVal = static_cast<rtcMediaStreamHandle>(media_stream.release());
    return rtcResultU4::kSuccess;
} // end RTCPeerConnectionFactory_CreateStream

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetRtpSenderCapabilities(
    rtcPeerConnectionFactoryHandle factory,
    RTCMediaType media_type,
    rtcRtpCapabilitiesHandle* pRetVal
) noexcept
{
    CHECK_OUT_POINTER(pRetVal);
    CHECK_NATIVE_HANDLE(factory);

    scoped_refptr<RTCPeerConnectionFactory> pFactory = static_cast<RTCPeerConnectionFactory*>(factory);
    scoped_refptr<RTCRtpCapabilities> rtp_capabilities = pFactory->GetRtpSenderCapabilities(media_type);

    *pRetVal = static_cast<rtcRtpCapabilitiesHandle>(rtp_capabilities.release());
    return rtcResultU4::kSuccess;
} // end RTCPeerConnectionFactory_GetRtpSenderCapabilities

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnectionFactory_GetRtpReceiverCapabilities(
    rtcPeerConnectionFactoryHandle factory,
    RTCMediaType media_type,
    rtcRtpCapabilitiesHandle* pRetVal
) noexcept
{
    CHECK_OUT_POINTER(pRetVal);
    CHECK_NATIVE_HANDLE(factory);

    scoped_refptr<RTCPeerConnectionFactory> pFactory = static_cast<RTCPeerConnectionFactory*>(factory);
    scoped_refptr<RTCRtpCapabilities> rtp_capabilities = pFactory->GetRtpReceiverCapabilities(media_type);

    *pRetVal = static_cast<rtcRtpCapabilitiesHandle>(rtp_capabilities.release());
    return rtcResultU4::kSuccess;
} // end RTCPeerConnectionFactory_GetRtpReceiverCapabilities
