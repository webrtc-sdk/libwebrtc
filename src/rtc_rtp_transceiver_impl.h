#ifndef LIB_WEBRTC_RTC_RTP_TRANSCEIVER_IMPL_HXX
#define LIB_WEBRTC_RTC_RTP_TRANSCEIVER_IMPL_HXX

#include "api/rtp_transceiver_interface.h"
#include "api/scoped_refptr.h"
#include "rtc_rtp_transceiver.h"

namespace libwebrtc {
class RTCRtpTransceiverInitImpl : public RTCRtpTransceiverInit {
 public:
  RTCRtpTransceiverInitImpl();

  virtual RTCRtpTransceiverDirection GetDirection() override;
  virtual void SetDirection(RTCRtpTransceiverDirection value) override;
  virtual Vector<String> GetStreamIds() override;
  virtual void SetStreamIds(Vector<String> value) override;
  virtual Vector<scoped_refptr<RTCRtpEncodingParameters>> GetSendEncodings()
      override;
  virtual void SetSendEncodings(
      Vector<scoped_refptr<RTCRtpEncodingParameters>> value) override;

  webrtc::RtpTransceiverInit rtp_transceiver_init();

 private:
  webrtc::RtpTransceiverInit rtp_transceiver_init_;
};

class RTCRtpTransceiverImpl : public RTCRtpTransceiver {
 public:
  RTCRtpTransceiverImpl(
      rtc::scoped_refptr<webrtc::RtpTransceiverInterface> rtp_transceiver);

  virtual RTCMediaType GetMediaType() const override;
  virtual String GetMid() const override;
  virtual scoped_refptr<RTCRtpSender> Sender() const override;
  virtual scoped_refptr<RTCRtpReceiver> Receiver() const override;
  virtual bool Stopped() const override;
  virtual bool Stopping() const override;
  virtual RTCRtpTransceiverDirection Direction() const override;
  virtual String SetDirectionWithError(
      RTCRtpTransceiverDirection new_direction) override;
  virtual RTCRtpTransceiverDirection CurrentDirection() const override;
  virtual RTCRtpTransceiverDirection FiredDirection() const override;
  virtual String StopStandard() override;
  virtual void StopInternal() override;

  rtc::scoped_refptr<webrtc::RtpTransceiverInterface> rtp_transceiver();

 private:
  rtc::scoped_refptr<webrtc::RtpTransceiverInterface> rtp_transceiver_;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_TYPES_HXX

//
//
// class RtpTransceiverInitImpl : public RtpTransceiverInit {
// public:
//  RtpTransceiverInitImpl();
//  RtpTransceiverInitImpl(const RtpTransceiverInit&);
//  ~RtpTransceiverInitImpl();
//
//  RtpTransceiverDirection GetDirection() override;
//  void SetDirection(RtpTransceiverDirection value) override;
//
//  std::vector<std::string> GetStreamIds();
//  void GetStreamIds(std::vector<std::string> value) override;
//
//  std::vector<scoped_refptr<RtpEncodingParameters>> GetSendEncodings()
//  override; void SetSendEncodings(
//      std::vector<scoped_refptr<RtpEncodingParameters>> value) override;
//
// public:
//  scoped_refptr<webrtc::RtpTransceiverInit> rtc_init();
//
// private:
//  scoped_refptr<webrtc::RtpTransceiverInit> rtp_transceiver_init_;
//};
//
// class RtpTransceiverImpl : public RtpTransceiver {
// public:
//  MediaType GetMediaType() const;
//
//  std::string GetMid() const;
//
//  scoped_refptr<RtpSender> Sender() const;
//
//  scoped_refptr<RtpReceiver> Receiver() const;
//
//  bool Stopped() const;
//
//  bool Stopping() const;
//
//  RtpTransceiverDirection direction() const;
//
//  RTCError SetDirectionWithError(RtpTransceiverDirection new_direction);
//
//  RtpTransceiverDirection current_direction() const;
//
//  RtpTransceiverDirection fired_direction() const;
//
//  RTCError StopStandard();
//
//  void StopInternal();
//
//  RTCError SetCodecPreferences(rtc::ArrayView<RtpCodecCapability> codecs);
//
//  std::vector<RtpCodecCapability> codec_preferences() const;
//
//  std::vector<RtpHeaderExtensionCapability> HeaderExtensionsToOffer() const;
//
//  std::vector<RtpHeaderExtensionCapability> HeaderExtensionsNegotiated()
//  const;
//
//  webrtc::RTCError SetOfferedRtpHeaderExtensions(
//      rtc::ArrayView<const RtpHeaderExtensionCapability>
//          header_extensions_to_offer);
//
//  void SetRtpTransceiverInterface(
//      rtc::scoped_refptr<webrtc::RtpTransceiverInterface> rti);
//
//  rtc::scoped_refptr<webrtc::RtpTransceiverInterface>
//  GetRtpTransceiverInterface();
//
// protected:
//  ~RtpTransceiverImpl() override = default;
//
// private:
//  rtc::scoped_refptr<webrtc::RtpTransceiverInterface> rtp_transceiver_;
//};
//
//}  // namespace libwebrtc
