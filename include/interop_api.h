#ifndef LIB_WEBRTC_INTROP_API_HXX
#define LIB_WEBRTC_INTROP_API_HXX

#ifdef LIB_WEBRTC_API_EXPORTS
#define LIB_WEBRTC_CALL __stdcall
#elif defined(LIB_WEBRTC_API_DLL)
#define LIB_WEBRTC_CALL __stdcall
#else
#define LIB_WEBRTC_CALL
#endif

#include "rtc_types.h"

extern "C" {

/// 32-bit boolean for interop API.
enum class rtcBool32 : int { kTrue = -1, kFalse = 0 };

/// Opaque handle to a native interop object.
using rtcObjectHandle = void*;

/// Opaque handle to a native reference-counted interop object.
using rtcRefCountedObjectHandle = rtcObjectHandle;

/// Opaque handle to a native RTCPeerConnectionFactory interop object.
using rtcPeerConnectionFactoryHandle = rtcRefCountedObjectHandle;

/**
 * @brief Initializes the WebRTC PeerConnectionFactory and threads.
 *
 * Initializes the WebRTC PeerConnectionFactory and threads. This method is
 * thread-safe and can be called from any thread. It initializes SSL and
 * creates three threads: worker_thread, signaling_thread and network_thread.
 *
 * @return kTrue if initialization is successful, kFalse otherwise.
 */
LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
LibWebRTC_Initialize() noexcept;

/**
 * @brief Creates a new WebRTC PeerConnectionFactory.
 *
 * Creates a new WebRTC PeerConnectionFactory. This method is thread-safe and
 * can be called from any thread. It creates a new instance of the
 * RTCPeerConnectionFactoryImpl class and initializes it.
 *
 * @return A scoped_refptr object that points to the newly created
 * RTCPeerConnectionFactory.
 */
LIB_WEBRTC_API rtcPeerConnectionFactoryHandle LIB_WEBRTC_CALL
LibWebRTC_CreateRTCPeerConnectionFactory() noexcept;

/**
 * @brief Terminates the WebRTC PeerConnectionFactory and threads.
 *
 * Terminates the WebRTC PeerConnectionFactory and threads. This method is
 * thread-safe and can be called from any thread. It cleans up SSL and stops
 * and destroys the three threads: worker_thread, signaling_thread and
 * network_thread.
 *
 */
LIB_WEBRTC_API void LIB_WEBRTC_CALL
LibWebRTC_Terminate() noexcept;

/**
 * Add ref a pointer.
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RefCountedObject_AddRef(rtcRefCountedObjectHandle handle) noexcept;

/**
 * Release a pointer.
 */
LIB_WEBRTC_API int LIB_WEBRTC_CALL
RefCountedObject_Release(rtcRefCountedObjectHandle handle) noexcept;

} // extern "C"

#endif  // LIB_WEBRTC_INTROP_API_HXX