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
  }
  return g_is_initialized;
}

// Stops and cleans up the threads and SSL.
void LibWebRTC::Terminate() {
  webrtc::ThreadManager::Instance()->SetCurrentThread(NULL);
  webrtc::CleanupSSL();

  // Resets the static variable g_is_initialized to false.
  g_is_initialized = false;
}

// Creates and returns an instance of RTCPeerConnectionFactory.
scoped_refptr<RTCPeerConnectionFactory>
LibWebRTC::CreateRTCPeerConnectionFactory() {
  scoped_refptr<RTCPeerConnectionFactory> rtc_peerconnection_factory =
      scoped_refptr<RTCPeerConnectionFactory>(
          new RefCountedObject<RTCPeerConnectionFactoryImpl>());
  rtc_peerconnection_factory->Initialize();
  return rtc_peerconnection_factory;
}

}  // namespace libwebrtc
