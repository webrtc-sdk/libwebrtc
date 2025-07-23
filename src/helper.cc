#include "helper.h"

#include "rtc_base/crypto_random.h"

namespace libwebrtc {
/**
 * Generates a random UUID string using the WebRTC library function
 * webrtc::CreateRandomUuid().
 *
 * @return A string representation of a random UUID.
 */
string Helper::CreateRandomUuid() { return webrtc::CreateRandomUuid(); }

}  // namespace libwebrtc
