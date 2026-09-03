#ifndef LIB_WEBRTC_HXX
#define LIB_WEBRTC_HXX

#include "rtc_peerconnection_factory.h"
#include "rtc_types.h"

namespace libwebrtc {

/**
 * @class LibWebRTC
 * @brief Provides static methods for initializing, creating and terminating
 * the WebRTC PeerConnectionFactory and threads.
 *
 * This class provides static methods for initializing, creating and terminating
 * the WebRTC PeerConnectionFactory and threads. These methods are thread-safe
 * and can be called from any thread. This class is not meant to be
 * instantiated.
 *
 */
class LibWebRTC {
 public:
  /**
   * @brief Initializes the WebRTC PeerConnectionFactory and threads.
   *
   * Initializes the WebRTC PeerConnectionFactory and threads. This method is
   * thread-safe and can be called from any thread. It initializes SSL and
   * creates three threads: worker_thread, signaling_thread and network_thread.
   *
   * @return true if initialization is successful, false otherwise.
   */
  LIB_WEBRTC_API static bool Initialize();

  /**
   * @brief Applies the given field trials and initializes WebRTC.
   *
   * Configures the global WebRTC field trials and then performs the same
   * initialization as Initialize(). Field trials are read when objects are
   * created, so this must be called before any other call into WebRTC, in
   * particular before CreateRTCPeerConnectionFactory().
   *
   * Each entry is either "Key/Value" or "Key/Value/", e.g.
   * "WebRTC-FlexFEC-03/Enabled/". See rtc_field_trials.h for the well known
   * keys and for finer grained control over the field trials.
   *
   * @param field_trials The field trial entries to apply.
   */
  LIB_WEBRTC_API static void InitializeWithFieldTrials(
      vector<string> field_trials);

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
  LIB_WEBRTC_API static scoped_refptr<RTCPeerConnectionFactory>
  CreateRTCPeerConnectionFactory();

  /**
   * @brief Terminates the WebRTC PeerConnectionFactory and threads.
   *
   * Terminates the WebRTC PeerConnectionFactory and threads. This method is
   * thread-safe and can be called from any thread. It cleans up SSL and stops
   * and destroys the three threads: worker_thread, signaling_thread and
   * network_thread.
   *
   */
  LIB_WEBRTC_API static void Terminate();
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_HXX
