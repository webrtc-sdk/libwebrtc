/**
 * File provided for Reference Use Only by isoft (c) 2025.
 * Copyright (c) isoft. All rights reserved.
 * 
 */

#ifndef LIB_WEBRTC_RTC_AUDIO_DATA_HXX
#define LIB_WEBRTC_RTC_AUDIO_DATA_HXX

#include "rtc_types.h"

namespace libwebrtc {

class RTCAudioData : public RefCountInterface {
public:
  LIB_WEBRTC_API static scoped_refptr<RTCAudioData> Create(
    uint32_t index,
    const uint8_t* data,
    uint32_t bits_per_sample,
    int sample_rate_hz, // the sample rate in Hz.
    uint32_t num_channels = 1
  );

  LIB_WEBRTC_API static scoped_refptr<RTCAudioData> Create();

public:
  virtual uint32_t index() const = 0;
  virtual void set_index(uint32_t index) = 0;
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
  virtual int Clear(RTCAudioDataToneFrequency frequency = RTCAudioDataToneFrequency::kNone) = 0;

protected:
  /**
   * The destructor for the RTCAudioData class.
   */
  virtual ~RTCAudioData() {}
}; // end class RTCAudioData

}  // namespace libwebrtc

#endif // LIB_WEBRTC_RTC_AUDIO_DATA_HXX
