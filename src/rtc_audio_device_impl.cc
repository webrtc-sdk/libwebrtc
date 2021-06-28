#include "rtc_audio_device_impl.h"
#include "rtc_base/logging.h"

namespace libwebrtc {

AudioDeviceImpl::~AudioDeviceImpl() {
  RTC_LOG(INFO) << __FUNCTION__ << ": dtor ";
}

int16_t AudioDeviceImpl::PlayoutDevices() {
  return audio_device_module_->PlayoutDevices();
}

int16_t AudioDeviceImpl::RecordingDevices() {
  return audio_device_module_->RecordingDevices();
}

int32_t AudioDeviceImpl::PlayoutDeviceName(uint16_t index,
                                           char name[kAdmMaxDeviceNameSize],
                                           char guid[kAdmMaxGuidSize]) {
  return audio_device_module_->PlayoutDeviceName(index, name, guid);
}

int32_t AudioDeviceImpl::RecordingDeviceName(uint16_t index,
                                             char name[kAdmMaxDeviceNameSize],
                                             char guid[kAdmMaxGuidSize]) {
  return audio_device_module_->RecordingDeviceName(index, name, guid);
}

int32_t AudioDeviceImpl::SetPlayoutDevice(uint16_t index) {
  return audio_device_module_->SetPlayoutDevice(index);
}

int32_t AudioDeviceImpl::SetRecordingDevice(uint16_t index) {
  return audio_device_module_->SetRecordingDevice(index);
}

}  // namespace libwebrtc
