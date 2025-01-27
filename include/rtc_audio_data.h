#ifndef LIB_WEBRTC_RTC_AUDIO_DATA_HXX
#define LIB_WEBRTC_RTC_AUDIO_DATA_HXX

#include "rtc_types.h"

namespace libwebrtc {

class RTCAudioData : public RefCountInterface {
public:  
  LIB_WEBRTC_API static scoped_refptr<RTCAudioData> Create(
    const uint8_t* data,
    uint32_t bits_per_sample,
    int sample_rate_hz, // the sample rate in Hz.
    uint32_t num_channels = 1
  );

  LIB_WEBRTC_API static scoped_refptr<RTCAudioData> Create();

public:
  virtual const uint8_t* data() const = 0;
  virtual size_t data_size() const = 0;
  virtual uint32_t bits_per_sample() const = 0;
  virtual uint32_t samples_per_channel() const = 0;
  virtual int sample_rate_hz() const = 0;
  virtual uint32_t num_channels() const = 0;

  /**
   * @brief Scales the contents of another RTCAudioData.
   */
  virtual int ScaleFrom(scoped_refptr<RTCAudioData> src) = 0;
  
  /**
   * Empty the buffer space. Or fill it with 1khz tone signal.
   */
  virtual int Clear(bool fill_1khz_tone = false) = 0;

protected:
  /**
   * The destructor for the RTCAudioData class.
   */
  virtual ~RTCAudioData() {}
}; // end class RTCAudioData

}  // namespace libwebrtc

#endif // LIB_WEBRTC_RTC_AUDIO_DATA_HXX
