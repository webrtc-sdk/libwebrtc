#ifndef LIB_WEBRTC_RTP_RECEIVER_HXX
#define LIB_WEBRTC_RTP_RECEIVER_HXX

//#include <string.h>
//#include "rtc_types.h"
#include "base/refcount.h"
//#include "dtls_transport.h"
//#include "rtc_media_stream.h"
#include "rtc_media_types.h"
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
  /*virtual scoped_refptr<RTCMediaStream> track() const = 0;

  virtual scoped_refptr<DtlsTransport> dtls_transport() const = 0;

  virtual std::vector<std::string> stream_ids() const = 0;
  virtual std::vector<scoped_refptr<RTCMediaStream>> streams() const = 0;

  virtual MediaType media_type() const = 0;

  virtual std::string id() const = 0;

  virtual RtpParameters GetParameters() const = 0;

  virtual bool SetParameters(const RtpParameters& parameters) = 0;

  virtual void SetObserver(RtpReceiverObserver* observer) = 0;

  virtual void SetJitterBufferMinimumDelay(
      absl::optional<double> delay_seconds) = 0;

  virtual std::vector<RtpSource> GetSources() const = 0;

  virtual void SetFrameDecryptor(
      scoped_refptr<FrameDecryptor> frame_decryptor);

  virtual scoped_refptr<FrameDecryptor> GetFrameDecryptor() const = 0;

  virtual void SetDepacketizerToDecoderFrameTransformer(
      scoped_refptr<FrameTransformerInterface> frame_transformer) = 0;

 protected:
  ~RtpReceiver() override = default;*/
};
}  // namespace libwebrtc

#endif  // !LIB_WEBRTC_RTP_RECEIVER_H_