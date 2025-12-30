#ifndef LIB_WEBRTC_RTC_RTP_SENDER_HXX
#define LIB_WEBRTC_RTC_RTP_SENDER_HXX

#include "base/refcount.h"
#include "base/scoped_ref_ptr.h"
#include "rtc_rtp_parameters.h"
#include "rtc_types.h"
#include "rtc_base_list.h"

namespace libwebrtc {

class RTCMediaTrack;
class RTCDtlsTransport;
class RTCDtmfSender;

class RTCRtpSender : public RefCountInterface {
 public:
  virtual bool set_track(scoped_refptr<RTCMediaTrack> track) = 0;

  virtual scoped_refptr<RTCMediaTrack> track() const = 0;

  virtual scoped_refptr<RTCDtlsTransport> dtls_transport() const = 0;

  virtual uint32_t ssrc() const = 0;

  virtual RTCMediaType media_type() const = 0;

  virtual const string id() const = 0;

  virtual const vector<string> stream_ids() const = 0;

  virtual void set_stream_ids(const vector<string> stream_ids) const = 0;

  virtual const vector<scoped_refptr<RTCRtpEncodingParameters>>
  init_send_encodings() const = 0;

  virtual scoped_refptr<RTCRtpParameters> parameters() const = 0;

  virtual bool set_parameters(
      const scoped_refptr<RTCRtpParameters> parameters) = 0;

  virtual scoped_refptr<RTCDtmfSender> dtmf_sender() const = 0;
};

/**
 * class RTCRtpSenderList
 */
class RTCRtpSenderList : public RTCBaseList<scoped_refptr<RTCRtpSender>> {
 public:
  LIB_WEBRTC_API static scoped_refptr<RTCRtpSenderList> Create(
    const vector<scoped_refptr<RTCRtpSender>>& source);

 protected:
  RTCRtpSenderList(const vector<scoped_refptr<RTCRtpSender>>& source)
    : RTCBaseList<scoped_refptr<RTCRtpSender>>(source)
  {}

  ~RTCRtpSenderList() {}
}; // end class RTCRtpSenderList

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_TYPES_HXX