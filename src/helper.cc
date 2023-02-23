#include "helper.h"
#include "rtc_base/helpers.h"
namespace libwebrtc {
string Helper::CreateRandomUuid() {
  return rtc::CreateRandomUuid();
}

}  // namespace libwebrtc
