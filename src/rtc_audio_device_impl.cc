#include "rtc_audio_device_impl.h"
#include "rtc_base/logging.h"

namespace libwebrtc {

AudioDeviceImpl::AudioDeviceImpl(
    rtc::scoped_refptr<webrtc::AudioDeviceModule> audio_device_module,
    rtc::Thread* worker_thread)
    : audio_device_module_(audio_device_module), worker_thread_(worker_thread) {
  audio_device_module_->SetAudioDeviceSink(this);
}

AudioDeviceImpl::~AudioDeviceImpl() {
  RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
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
  return worker_thread_->Invoke<int32_t>(RTC_FROM_HERE, [&] {
    RTC_DCHECK_RUN_ON(worker_thread_);
    return audio_device_module_->PlayoutDeviceName(index, name, guid);
  });
}

int32_t AudioDeviceImpl::RecordingDeviceName(uint16_t index,
                                             char name[kAdmMaxDeviceNameSize],
                                             char guid[kAdmMaxGuidSize]) {
  return worker_thread_->Invoke<int32_t>(RTC_FROM_HERE, [&] {
    RTC_DCHECK_RUN_ON(worker_thread_);
    return audio_device_module_->RecordingDeviceName(index, name, guid);
  });
}

int32_t AudioDeviceImpl::SetPlayoutDevice(uint16_t index) {
  return worker_thread_->Invoke<int32_t>(RTC_FROM_HERE, [&] {
    RTC_DCHECK_RUN_ON(worker_thread_);
    if (audio_device_module_->Playing()) {
      audio_device_module_->StopPlayout();
      audio_device_module_->SetPlayoutDevice(index);
      audio_device_module_->InitPlayout();
      return audio_device_module_->StartPlayout();
    } else {
      return audio_device_module_->SetPlayoutDevice(index);
    }
  });
}

int32_t AudioDeviceImpl::SetRecordingDevice(uint16_t index) {
  return worker_thread_->Invoke<int32_t>(RTC_FROM_HERE, [&] {
    RTC_DCHECK_RUN_ON(worker_thread_);
    if (audio_device_module_->Recording()) {
      audio_device_module_->StopRecording();
      audio_device_module_->SetRecordingDevice(index);
      audio_device_module_->InitRecording();
      return audio_device_module_->StartRecording();
    } else {
      return audio_device_module_->SetRecordingDevice(index);
    }
  });
}

int32_t AudioDeviceImpl::OnDeviceChange(OnDeviceChangeCallback listener) {
  listener_ = listener;
  return 0;
}

void AudioDeviceImpl::OnDevicesUpdated() {
  if (listener_)
    listener_();
}

}  // namespace libwebrtc
