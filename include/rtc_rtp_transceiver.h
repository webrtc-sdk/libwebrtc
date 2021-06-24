#ifndef LIB_WEBRTC_RTC_RTP_TRANSCEIVER_HXX
#define LIB_WEBRTC_RTC_RTP_TRANSCEIVER_HXX

#include "base/refcount.h"
#include "rtc_media_types.h"
#include "rtc_rtp_parameters.h"
#include "rtc_rtp_transceiver_direction.h"
#include "rtc_types.h"
#include "rtc_rtp_receiver.h"
#include "rtc_rtp_sender.h"
#include "rtc_string.h"

namespace libwebrtc {

class RTCRtpTransceiverInit : public RefCountInterface {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCRtpTransceiverInit> Create();

  virtual RTCRtpTransceiverDirection GetDirection() = 0;
  virtual void SetDirection(RTCRtpTransceiverDirection value) = 0;

  virtual void GetStreamIds(OnString on) = 0;
  virtual void SetStreamIds(OnVectorString on) = 0;

  virtual void GetSendEncodings(OnRTCRtpEncodingParameters on) = 0;
  virtual void SetSendEncodings(OnVectorRTCRtpEncodingParameters on) = 0;
};

class RTCRtpTransceiver : public RefCountInterface {
 public:
  virtual RTCMediaType GetMediaType() const = 0;

  virtual void GetMid(OnString on) const = 0;

  virtual scoped_refptr<RTCRtpSender> Sender() const = 0;

  virtual scoped_refptr<RTCRtpReceiver> Receiver() const = 0;

  virtual bool Stopped() const = 0;

  virtual bool Stopping() const = 0;

  virtual RTCRtpTransceiverDirection Direction() const = 0;

  virtual void SetDirectionWithError(RTCRtpTransceiverDirection new_direction,
                                     OnString on) = 0;

  virtual RTCRtpTransceiverDirection CurrentDirection() const = 0;

  virtual RTCRtpTransceiverDirection FiredDirection() const = 0;

  virtual void StopStandard(OnString on) = 0;

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

typedef fixed_size_function<void(scoped_refptr<RTCRtpTransceiver> param)>
    OnRTCRtpTransceiver;

typedef fixed_size_function<void(OnRTCRtpTransceiver param)>
    OnVectorRTCRtpTransceiver;


}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_TYPES_HXX
