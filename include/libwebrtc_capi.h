#ifndef LIB_WEBRTC_HXX_CAPI
#define LIB_WEBRTC_HXX_CAPI

#include "rtc_peerconnection_factory_capi.h"
#include "rtc_types_capi.h"

extern "C" {
/**
 * @brief Initializes the WebRTC library.
 *
 * This function initializes the WebRTC library. It must be called before
 * using any other WebRTC functions.
 *
 * @return rtcBoolean Returns kTrue if initialization is successful, kFalse
 * otherwise.
 */
LIB_WEBRTC_CAPI rtcBoolean LibWebRTC_Initialize();

/**
 * @brief Creates a new WebRTC PeerConnectionFactory.
 *
 * This function creates a new WebRTC PeerConnectionFactory. It must be
 * called after initializing the WebRTC library using LibWebRTC_Initialize().
 *
 * @return A pointer to the newly created RTCPeerConnectionFactory.
 */
LIB_WEBRTC_CAPI rtcPeerConnectionFactoryHandle
LibWebRTC_CreatePeerConnectionFactory();

/**
 * @brief Terminates the WebRTC PeerConnectionFactory and threads.
 *
 * Terminates the WebRTC PeerConnectionFactory and threads. This method is
 * thread-safe and can be called from any thread. It cleans up SSL and stops
 * and destroys the three threads: worker_thread, signaling_thread and
 * network_thread.
 *
 */
LIB_WEBRTC_CAPI void LibWebRTC_Terminate();
}

#endif  // LIB_WEBRTC_HXX_CAPI
