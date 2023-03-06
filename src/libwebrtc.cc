#include "libwebrtc.h"

#include "api/scoped_refptr.h"
#include "rtc_base/ssl_adapter.h"
#include "rtc_base/thread.h"

#include "rtc_peerconnection_factory_impl.h"

namespace libwebrtc {

// Initialize static variable g_is_initialized to false.
static bool g_is_initialized = false;
// Thread pointers for different thread types.
std::unique_ptr<rtc::Thread> worker_thread;
std::unique_ptr<rtc::Thread> signaling_thread;
std::unique_ptr<rtc::Thread> network_thread;

// Initializes SSL and the required threads, if not initialized.
bool LibWebRTC::Initialize() {
  if (!g_is_initialized) {
    rtc::InitializeSSL();
    g_is_initialized = true;

    // Creates and starts the worker thread.
    if (worker_thread.get() == nullptr) {
      worker_thread = rtc::Thread::Create();
      worker_thread->SetName("worker_thread", nullptr);
      RTC_CHECK(worker_thread->Start()) << "Failed to start thread";
    }

    // Creates and starts the signaling thread.
    if (signaling_thread.get() == nullptr) {
      signaling_thread = rtc::Thread::Create();
      signaling_thread->SetName("signaling_thread", NULL);
      RTC_CHECK(signaling_thread->Start()) << "Failed to start thread";
    }

    // Creates and starts the network thread.
    if (network_thread.get() == nullptr) {
      network_thread = rtc::Thread::CreateWithSocketServer();
      network_thread->SetName("network_thread", nullptr);
      RTC_CHECK(network_thread->Start()) << "Failed to start thread";
    }
  }
  return g_is_initialized;
}

// Stops and cleans up the threads and SSL.
void LibWebRTC::Terminate() {
  rtc::ThreadManager::Instance()->SetCurrentThread(NULL);
  rtc::CleanupSSL();

  // Stops and resets the worker thread.
  if (worker_thread.get() != nullptr) {
    worker_thread->Stop();
    worker_thread.reset(nullptr);
  }

  // Stops and resets the signaling thread.
  if (signaling_thread.get() != nullptr) {
    signaling_thread->Stop();
    signaling_thread.reset(nullptr);
  }

  // Stops and resets the network thread.
  if (network_thread.get() != nullptr) {
    network_thread->Stop();
    network_thread.reset(nullptr);
  }

  // Resets the static variable g_is_initialized to false.
  g_is_initialized = false;
}

// Creates and returns an instance of RTCPeerConnectionFactory.
scoped_refptr<RTCPeerConnectionFactory>
LibWebRTC::CreateRTCPeerConnectionFactory() {
  scoped_refptr<RTCPeerConnectionFactory> rtc_peerconnection_factory =
      scoped_refptr<RTCPeerConnectionFactory>(
          new RefCountedObject<RTCPeerConnectionFactoryImpl>(
              worker_thread.get(), signaling_thread.get(),
              network_thread.get()));
  rtc_peerconnection_factory->Initialize();
  return rtc_peerconnection_factory;
}

}  // namespace libwebrtc
