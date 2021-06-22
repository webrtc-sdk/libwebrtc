#ifndef LIB_WEBRTC_RTP_RECEIVER_HXX
#define LIB_WEBRTC_RTP_RECEIVER_HXX

//#include <string.h>
#include "rtc_types.h"
#include "base/refcount.h"
#include "base/scoped_ref_ptr.h"
#include "rtc_dtls_transport.h"
#include "rtc_media_stream.h"
#include "rtc_media_types.h"
#include "rtc_rtp_parameters.h"
#include "rtc_media_track.h"
//#include "rtc_frame_decryptor.h"
//#include "rtc_frame_encryptor.h"

namespace libwebrtc {

class RTCRtpReceiverObserver {
 public:
  virtual void OnFirstPacketReceived(RTCMediaType media_type) = 0;

 protected:
  virtual ~RTCRtpReceiverObserver() {}
};

class RTCRtpReceiver : public RefCountInterface {
 public:
  virtual scoped_refptr<RTCMediaTrack> Track() const = 0;

  virtual scoped_refptr<RTCDtlsTransport> DtlsTransport() const = 0;

  virtual void StreamIds(OnString on) const = 0;
  virtual Vector<scoped_refptr<RTCMediaStream>> Streams() const = 0;

  virtual RTCMediaType MediaType() const = 0;

  virtual void Id(OnString on) const = 0;

  virtual scoped_refptr<RTCRtpParameters> GetParameters() const = 0;

  virtual bool SetParameters(scoped_refptr<RTCRtpParameters> parameters) = 0;

  virtual void SetObserver(RTCRtpReceiverObserver* observer) = 0;

  virtual void SetJitterBufferMinimumDelay(double delay_seconds) = 0;

  // virtual Vector<RtpSource> GetSources() const = 0;

  // virtual void SetFrameDecryptor(
  //    scoped_refptr<FrameDecryptor> frame_decryptor);

  // virtual scoped_refptr<FrameDecryptor> GetFrameDecryptor() const = 0;

  // virtual void SetDepacketizerToDecoderFrameTransformer(
  //    scoped_refptr<FrameTransformerInterface> frame_transformer) = 0;
};

}  // namespace libwebrtc

#endif  // !LIB_WEBRTC_RTP_RECEIVER_H_