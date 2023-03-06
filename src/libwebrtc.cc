/**

@file libwebrtc.h
@brief Contains the implementation of the LibWebRTC class.
*/
#include "libwebrtc.h"
#include "api/scoped_refptr.h"
#include "rtc_base/ssl_adapter.h"
#include "rtc_base/thread.h"

#include "rtc_peerconnection_factory_impl.h"

namespace libwebrtc {

static bool g_is_initialized = false;
std::unique_ptrrtc::Thread worker_thread;
std::unique_ptrrtc::Thread signaling_thread;
std::unique_ptrrtc::Thread network_thread;

/**

@brief Initializes the required threads and SSL for the LibWebRTC library.

@return true if initialization was successful, false otherwise.
*/
bool LibWebRTC::Initialize() {
if (!g_is_initialized) {
rtc::InitializeSSL();
g_is_initialized = true;

if (worker_thread.get() == nullptr) {
worker_thread = rtc::Thread::Create();
worker_thread->SetName("worker_thread", nullptr);
RTC_CHECK(worker_thread->Start()) << "Failed to start thread";
}

if (signaling_thread.get() == nullptr) {
signaling_thread = rtc::Thread::Create();
signaling_thread->SetName("signaling_thread", NULL);
RTC_CHECK(signaling_thread->Start()) << "Failed to start thread";
}

if (network_thread.get() == nullptr) {
network_thread = rtc::Thread::CreateWithSocketServer();
network_thread->SetName("network_thread", nullptr);
RTC_CHECK(network_thread->Start()) << "Failed to start thread";
}
}
return g_is_initialized;
}

/**

@brief Terminates the threads and SSL used by the LibWebRTC library.
*/
void LibWebRTC::Terminate() {
rtc::ThreadManager::Instance()->SetCurrentThread(NULL);
rtc::CleanupSSL();
if (worker_thread.get() != nullptr) {
worker_thread->Stop();
worker_thread.reset(nullptr);
}

if (signaling_thread.get() != nullptr) {
signaling_thread->Stop();
signaling_thread.reset(nullptr);
}

if (network_thread.get() != nullptr) {
network_thread->Stop();
network_thread.reset(nullptr);
}

g_is_initialized = false;
}

/**

@brief Creates an RTCPeerConnectionFactory object and initializes it.
@return a scoped_refptr<RTCPeerConnectionFactory> object.
*/
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
} // namespace libwebrtc
