
#ifndef LIB_WEBRTC_DTMF_SENDER_INTERFACE_H_
#define LIB_WEBRTC_DTMF_SENDER_INTERFACE_H_

#include <string>

#include "api/dtmf_sender_interface.h"
#include "api/scoped_refptr.h"
#include "rtc_dtmf_sender.h"

namespace libwebrtc {

class RTCDtmfSenderImpl : public RTCDtmfSender,
                          public webrtc::DtmfSenderObserverInterface {
 public:
  RTCDtmfSenderImpl(
      rtc::scoped_refptr<webrtc::DtmfSenderInterface> dtmf_sender);

  virtual void RegisterObserver(RTCDtmfSenderObserver* observer) override;
  virtual void UnregisterObserver() override;
  virtual bool CanInsertDtmf() override;
  virtual int Duration() const override;
  virtual int InterToneGap() const override;
  virtual int CommaDelay() const override;
  virtual bool InsertDtmf(const char* tones,
                          size_t size,
                          int duration,
                          int inter_tone_gap) override;
  virtual bool InsertDtmf(const char* tones,
                          size_t size,
                          int duration,
                          int inter_tone_gap,
                          int comma_delay) override;
  virtual void Tones(OnString on) const override;

  virtual void OnToneChange(const std::string& tone,
                            const std::string& tone_buffer) override;

  virtual void OnToneChange(const std::string& tone) override;

  rtc::scoped_refptr<webrtc::DtmfSenderInterface> dtmf_sender();

 private:
  rtc::scoped_refptr<webrtc::DtmfSenderInterface> dtmf_sender_;
  RTCDtmfSenderObserver* observer_;
};

}  // namespace libwebrtc

#endif  // API_DTMF_SENDER_INTERFACE_H_
