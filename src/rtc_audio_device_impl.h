#ifndef LIB_WEBRTC_AUDIO_DEVICE_IMPL_HXX
#define LIB_WEBRTC_AUDIO_DEVICE_IMPL_HXX

#include "rtc_audio_device.h"
#include "modules/audio_device/include/audio_device.h"
#include "modules/audio_device/audio_device_impl.h"
#include "rtc_base/ref_count.h"

namespace libwebrtc {
class AudioDeviceImpl : public RTCAudioDevice {
 public:
  AudioDeviceImpl(
      rtc::scoped_refptr<webrtc::AudioDeviceModule> audio_device_module)
      : audio_device_module_(audio_device_module) {}

  virtual ~AudioDeviceImpl();

 public:
  int16_t PlayoutDevices() override;

  int16_t RecordingDevices() override;

  int32_t PlayoutDeviceName(uint16_t index,
                            char name[kAdmMaxDeviceNameSize],
                            char guid[kAdmMaxGuidSize]) override;

  int32_t RecordingDeviceName(uint16_t index,
                              char name[kAdmMaxDeviceNameSize],
                              char guid[kAdmMaxGuidSize]) override;

  int32_t SetPlayoutDevice(uint16_t index) override;

  int32_t SetRecordingDevice(uint16_t index) override;

 private:
  rtc::scoped_refptr<webrtc::AudioDeviceModule> audio_device_module_;
};

} // namespace libwebrtc

#endif //LIB_WEBRTC_AUDIO_DEVICE_IMPL_HXX
