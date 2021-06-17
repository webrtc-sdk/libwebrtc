#ifndef LIB_WEBRTC_RTP_TRANSCEIVER_DIRECTION_H_
#define LIB_WEBRTC_RTP_TRANSCEIVER_DIRECTION_H_

namespace libwebrtc {

enum class RTCRtpTransceiverDirection {
  kSendRecv,
  kSendOnly,
  kRecvOnly,
  kInactive,
  kStopped,
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTP_TRANSCEIVER_DIRECTION_H_
