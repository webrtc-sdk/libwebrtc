#ifndef LIB_WEBRTC_RTC_RTP_TRANSCEIVER_HXX
#define LIB_WEBRTC_RTC_RTP_TRANSCEIVER_HXX

#include "base/refcount.h"
#include "rtc_media_types.h"
#include "rtc_rtp_parameters.h"
#include "rtc_rtp_transceiver_direction.h"
#include "rtc_types.h"
#include "rtc_rtp_receiver.h"
#include "rtc_rtp_sender.h"

namespace libwebrtc {
class RTCRtpTransceiverInit : public RefCountInterface {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCRtpTransceiverInit> Create();

  virtual RTCRtpTransceiverDirection GetDirection() = 0;
  virtual void SetDirection(RTCRtpTransceiverDirection value) = 0;

  virtual Vector<String> GetStreamIds() = 0;
  virtual void SetStreamIds(Vector<String> value) = 0;

  virtual Vector<scoped_refptr<RTCRtpEncodingParameters>>
  GetSendEncodings() = 0;
  virtual void SetSendEncodings(
      Vector<scoped_refptr<RTCRtpEncodingParameters>> value) = 0;
};

class RTCRtpTransceiver : public RefCountInterface {
 public:
  virtual RTCMediaType GetMediaType() const = 0;

  virtual String GetMid() const = 0;

  virtual scoped_refptr<RTCRtpSender> Sender() const = 0;

  virtual scoped_refptr<RTCRtpReceiver> Receiver() const = 0;

  virtual bool Stopped() const = 0;

  virtual bool Stopping() const = 0;

  virtual RTCRtpTransceiverDirection Direction() const = 0;

  virtual String SetDirectionWithError(
      RTCRtpTransceiverDirection new_direction) = 0;

  virtual RTCRtpTransceiverDirection CurrentDirection() const = 0;

  virtual RTCRtpTransceiverDirection FiredDirection() const = 0;

  virtual String StopStandard() = 0;

  virtual void StopInternal() = 0;

  // virtual String SetCodecPreferences(
  //    rtc::ArrayView<RTCRtpCodecCapability> codecs);

  // virtual std::vector<RTCRtpCodecCapability> codec_preferences() const;

  // virtual std::vector<RtpHeaderExtensionCapability> HeaderExtensionsToOffer()
  //    const;

  // virtual std::vector<RTCRtpHeaderExtensionCapability>
  // HeaderExtensionsNegotiated()
  //    const;

  // virtual webrtc::RTCError SetOfferedRtpHeaderExtensions(
  //    rtc::ArrayView<const RTCRtpHeaderExtensionCapability>
  //        header_extensions_to_offer);
};
}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_TYPES_HXX
