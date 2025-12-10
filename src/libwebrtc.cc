#include "libwebrtc.h"

#include "api/scoped_refptr.h"
#include "rtc_base/ssl_adapter.h"
#include "rtc_base/thread.h"
#include "rtc_peerconnection_factory_impl.h"

namespace libwebrtc {

// Initialize static variable g_is_initialized to false.
static bool g_is_initialized = false;

// Initializes SSL, if not initialized.
bool LibWebRTC::Initialize() {
  if (!g_is_initialized) {
    webrtc::InitializeSSL();
    g_is_initialized = true;
#ifdef WEBRTC_WIN
  WSADATA data;
  WSAStartup(MAKEWORD(1, 0), &data);
#endif
  }
  return g_is_initialized;
}

// Stops and cleans up the threads and SSL.
void LibWebRTC::Terminate() {
  if(!g_is_initialized) {
    return;
  }
  webrtc::ThreadManager::Instance()->SetCurrentThread(NULL);
  webrtc::CleanupSSL();
#ifdef WEBRTC_WIN
  WSACleanup();
#endif
  // Resets the static variable g_is_initialized to false.
  g_is_initialized = false;
}

// Creates and returns an instance of RTCPeerConnectionFactory.
scoped_refptr<RTCPeerConnectionFactory>
LibWebRTC::CreateRTCPeerConnectionFactory() {
  scoped_refptr<RTCPeerConnectionFactory> rtc_peerconnection_factory =
      scoped_refptr<RTCPeerConnectionFactory>(
          new RefCountedObject<RTCPeerConnectionFactoryImpl>());
  return rtc_peerconnection_factory;
}

}  // namespace libwebrtc
