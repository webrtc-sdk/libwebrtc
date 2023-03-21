#include "helper.h"
#include "rtc_base/helpers.h"

namespace libwebrtc {
/**
 * Generates a random UUID string using the WebRTC library function rtc::CreateRandomUuid().
 * 
 * @return A string representation of a random UUID.
 */
string Helper::CreateRandomUuid() {
  return rtc::CreateRandomUuid();
}

}  // namespace libwebrtc
