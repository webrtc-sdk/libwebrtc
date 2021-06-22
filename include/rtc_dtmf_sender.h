
#ifndef LIB_WEBRTC_DTMF_SENDER__H_
#define LIB_WEBRTC_DTMF_SENDER__H_

#include "base/refcount.h"
#include "rtc_types.h"

namespace libwebrtc {

class RTCDtmfSenderObserver {
 public:
  virtual void OnToneChange(const char* tone,
                            size_t tone_size,
                            const char* tone_buffer,
                            size_t b_size) = 0;

  virtual void OnToneChange(const char* tone, size_t tone_size) = 0;

 protected:
  virtual ~RTCDtmfSenderObserver() = default;
};

class RTCDtmfSender : public RefCountInterface {
 public:
  static const int kDtmfDefaultCommaDelayMs = 2000;

  virtual void RegisterObserver(RTCDtmfSenderObserver* observer) = 0;

  virtual void UnregisterObserver() = 0;

  virtual bool CanInsertDtmf() = 0;

  virtual bool InsertDtmf(const char* tones,
                          size_t size,
                          int duration,
                          int inter_tone_gap) = 0;

  virtual bool InsertDtmf(const char* tones,
                          size_t size,
                          int duration,
                          int inter_tone_gap,
                          int comma_delay) = 0;

  virtual void Tones(OnString on) const = 0;

  virtual int Duration() const = 0;

  virtual int InterToneGap() const = 0;

  virtual int CommaDelay() const = 0;
};

}  // namespace libwebrtc

#endif  // API_DTMF_SENDER__H_
