#ifndef LIB_WEBRTC_RTC_RTP_SENDER_HXX
#define LIB_WEBRTC_RTC_RTP_SENDER_HXX

#include "rtc_types.h"
#include "base/refcount.h"
#include "rtc_media_track.h"
#include "rtc_media_types.h"
#include "rtc_rtp_parameters.h"
#include "rtc_dtls_transport.h"
#include "rtc_dtmf_sender.h"

namespace libwebrtc {


class RTCRtpSender : public RefCountInterface {
 public:
  virtual bool SetTrack(scoped_refptr<RTCMediaTrack> track) = 0;

  virtual scoped_refptr<RTCMediaTrack> GetTrack() const = 0;

  virtual scoped_refptr<RTCDtlsTransport> GetDtlsTransport() const = 0;

  virtual uint32_t GetSsrc() const = 0;

  virtual RTCMediaType GetMediaType() const = 0;

  virtual void Id(OnString on) const = 0;

  virtual void GetStreamIds(OnString on) const = 0;

  virtual void SetStreams(OnVectorString on) const = 0;

  virtual void InitSendEncodings(OnRTCRtpEncodingParameters on)
      const = 0;

  virtual scoped_refptr<RTCRtpParameters> GetParameters() const = 0;

  virtual bool SetParameters(
      const scoped_refptr<RTCRtpParameters> parameters) = 0;

  virtual scoped_refptr<RTCDtmfSender> GetDtmfSender() const = 0;

};

typedef fixed_size_function<void(scoped_refptr<RTCRtpSender> val)>
    OnRTCRtpSender;
typedef fixed_size_function<void(OnRTCRtpSender on)> OnVectorRTCRtpSender;

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_TYPES_HXX