#ifndef LIB_WEBRTC_AUDIO_DEVICE_IMPL_HXX
#define LIB_WEBRTC_AUDIO_DEVICE_IMPL_HXX

#include "modules/audio_device/audio_device_impl.h"
#include "modules/audio_device/include/audio_device.h"
#include "rtc_audio_device.h"
#include "rtc_base/ref_count.h"
#include "rtc_base/thread.h"

namespace libwebrtc {
class AudioDeviceImpl : public RTCAudioDevice, public webrtc::AudioDeviceObserver {
 public:
  AudioDeviceImpl(
      webrtc::scoped_refptr<webrtc::AudioDeviceModule> audio_device_module,
      webrtc::Thread* worker_thread);

  virtual ~AudioDeviceImpl();

 public:
  int16_t PlayoutDevices() override;

  int16_t RecordingDevices() override;

  int32_t PlayoutDeviceName(uint16_t index, char name[kAdmMaxDeviceNameSize],
                            char guid[kAdmMaxGuidSize]) override;

  int32_t RecordingDeviceName(uint16_t index, char name[kAdmMaxDeviceNameSize],
                              char guid[kAdmMaxGuidSize]) override;

  int32_t SetPlayoutDevice(uint16_t index) override;

  int32_t SetRecordingDevice(uint16_t index) override;

  int32_t SetMicrophoneVolume(uint32_t volume) override;

  int32_t MicrophoneVolume(uint32_t& volume) override;

  int32_t SetSpeakerVolume(uint32_t volume) override;

  int32_t SpeakerVolume(uint32_t& volume) override;

  int32_t OnDeviceChange(OnDeviceChangeCallback listener) override;

 protected:
  void OnDevicesUpdated() override;

 private:
  webrtc::scoped_refptr<webrtc::AudioDeviceModule> audio_device_module_;
  webrtc::Thread* worker_thread_ = nullptr;
  OnDeviceChangeCallback listener_ = nullptr;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_AUDIO_DEVICE_IMPL_HXX
