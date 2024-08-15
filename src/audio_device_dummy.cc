#include "audio_device_dummy.h"

const int kSampleRate = 48000;
const int kChannels = 2;
const int kBytesPerSample = kChannels * sizeof(int16_t);
const int kSamplesPer10Ms = kSampleRate / 100;

namespace libwebrtc {

AudioDeviceDummy::AudioDeviceDummy(webrtc::TaskQueueFactory* task_queue_factory)
    : data_(kSamplesPer10Ms * kChannels),
      task_queue_factory_(task_queue_factory) {}

AudioDeviceDummy::~AudioDeviceDummy() {
  Terminate();
}

int32_t AudioDeviceDummy::ActiveAudioLayer(AudioLayer* audioLayer) const {
  *audioLayer = AudioLayer::kDummyAudio;
  return 0;
}

int32_t AudioDeviceDummy::RegisterAudioCallback(webrtc::AudioTransport* transport) {
  webrtc::MutexLock lock(&mutex_);
  audio_transport_ = transport;
  return 0;
}

int32_t AudioDeviceDummy::Init() {
  webrtc::MutexLock lock(&mutex_);
  if (initialized_)
    return 0;

  audio_queue_ = task_queue_factory_->CreateTaskQueue(
          "AudioDevice", webrtc::TaskQueueFactory::Priority::NORMAL);

  audio_task_ =
      webrtc::RepeatingTaskHandle::Start(audio_queue_.get(), [this]() {
        webrtc::MutexLock lock(&mutex_);

        if (playing_) {
          int64_t elapsed_time_ms = -1;
          int64_t ntp_time_ms = -1;
          size_t n_samples_out = 0;
          void* data = data_.data();

          // Request the AudioData, otherwise WebRTC will ignore the packets.
          // 10ms of audio data.
          audio_transport_->NeedMorePlayData(
              kSamplesPer10Ms, kBytesPerSample, kChannels, kSampleRate, data,
              n_samples_out, &elapsed_time_ms, &ntp_time_ms);
        }

        return webrtc::TimeDelta::Millis(10);
      });

  initialized_ = true;
  return 0;
}

int32_t AudioDeviceDummy::Terminate() {
  {
    webrtc::MutexLock lock(&mutex_);
    if (!initialized_)
      return 0;

    initialized_ = false;
  }
  audio_queue_ = nullptr;
  return 0;
}

bool AudioDeviceDummy::Initialized() const {
  webrtc::MutexLock lock(&mutex_);
  return initialized_;
}

int16_t AudioDeviceDummy::PlayoutDevices() {
  return 0;
}

int16_t AudioDeviceDummy::RecordingDevices() {
  return 0;
}

int32_t AudioDeviceDummy::PlayoutDeviceName(uint16_t index,
                                       char name[webrtc::kAdmMaxDeviceNameSize],
                                       char guid[webrtc::kAdmMaxGuidSize]) {
  return 0;
}

int32_t AudioDeviceDummy::RecordingDeviceName(
    uint16_t index,
    char name[webrtc::kAdmMaxDeviceNameSize],
    char guid[webrtc::kAdmMaxGuidSize]) {
  return 0;
}

int32_t AudioDeviceDummy::SetPlayoutDevice(uint16_t index) {
  return 0;
}

int32_t AudioDeviceDummy::SetPlayoutDevice(WindowsDeviceType device) {
  return 0;
}

int32_t AudioDeviceDummy::SetRecordingDevice(uint16_t index) {
  return 0;
}

int32_t AudioDeviceDummy::SetRecordingDevice(WindowsDeviceType device) {
  return 0;
}

int32_t AudioDeviceDummy::PlayoutIsAvailable(bool* available) {
  return 0;
}

int32_t AudioDeviceDummy::InitPlayout() {
  return 0;
}

bool AudioDeviceDummy::PlayoutIsInitialized() const {
  return false;
}

int32_t AudioDeviceDummy::RecordingIsAvailable(bool* available) {
  return 0;
}

int32_t AudioDeviceDummy::InitRecording() {
  return 0;
}

bool AudioDeviceDummy::RecordingIsInitialized() const {
  return false;
}

int32_t AudioDeviceDummy::StartPlayout() {
  webrtc::MutexLock lock(&mutex_);
  playing_ = true;
  return 0;
}

int32_t AudioDeviceDummy::StopPlayout() {
  webrtc::MutexLock lock(&mutex_);
  playing_ = false;
  return 0;
}

bool AudioDeviceDummy::Playing() const {
  webrtc::MutexLock lock(&mutex_);
  return playing_;
}

int32_t AudioDeviceDummy::StartRecording() {
  return 0;
}

int32_t AudioDeviceDummy::StopRecording() {
  return 0;
}

bool AudioDeviceDummy::Recording() const {
  return false;
}

int32_t AudioDeviceDummy::InitSpeaker() {
  return 0;
}

bool AudioDeviceDummy::SpeakerIsInitialized() const {
  return false;
}

int32_t AudioDeviceDummy::InitMicrophone() {
  return 0;
}

bool AudioDeviceDummy::MicrophoneIsInitialized() const {
  return false;
}

int32_t AudioDeviceDummy::SpeakerVolumeIsAvailable(bool* available) {
  return 0;
}

int32_t AudioDeviceDummy::SetSpeakerVolume(uint32_t volume) {
  return 0;
}

int32_t AudioDeviceDummy::SpeakerVolume(uint32_t* volume) const {
  return 0;
}

int32_t AudioDeviceDummy::MaxSpeakerVolume(uint32_t* maxVolume) const {
  return 0;
}

int32_t AudioDeviceDummy::MinSpeakerVolume(uint32_t* minVolume) const {
  return 0;
}

int32_t AudioDeviceDummy::MicrophoneVolumeIsAvailable(bool* available) {
  return 0;
}

int32_t AudioDeviceDummy::SetMicrophoneVolume(uint32_t volume) {
  return 0;
}

int32_t AudioDeviceDummy::MicrophoneVolume(uint32_t* volume) const {
  return 0;
}

int32_t AudioDeviceDummy::MaxMicrophoneVolume(uint32_t* maxVolume) const {
  return 0;
}

int32_t AudioDeviceDummy::MinMicrophoneVolume(uint32_t* minVolume) const {
  return 0;
}

int32_t AudioDeviceDummy::SpeakerMuteIsAvailable(bool* available) {
  return 0;
}

int32_t AudioDeviceDummy::SetSpeakerMute(bool enable) {
  return 0;
}

int32_t AudioDeviceDummy::SpeakerMute(bool* enabled) const {
  return 0;
}

int32_t AudioDeviceDummy::MicrophoneMuteIsAvailable(bool* available) {
  return 0;
}

int32_t AudioDeviceDummy::SetMicrophoneMute(bool enable) {
  return 0;
}

int32_t AudioDeviceDummy::MicrophoneMute(bool* enabled) const {
  return 0;
}

int32_t AudioDeviceDummy::StereoPlayoutIsAvailable(bool* available) const {
  *available = true;
  return 0;
}

int32_t AudioDeviceDummy::SetStereoPlayout(bool enable) {
  return 0;
}

int32_t AudioDeviceDummy::StereoPlayout(bool* enabled) const {
  return 0;
}

int32_t AudioDeviceDummy::StereoRecordingIsAvailable(bool* available) const {
  return 0;
}

int32_t AudioDeviceDummy::SetStereoRecording(bool enable) {
  return 0;
}

int32_t AudioDeviceDummy::StereoRecording(bool* enabled) const {
  *enabled = true;
  return 0;
}

int32_t AudioDeviceDummy::PlayoutDelay(uint16_t* delayMS) const {
  return 0;
}

bool AudioDeviceDummy::BuiltInAECIsAvailable() const {
  return false;
}

bool AudioDeviceDummy::BuiltInAGCIsAvailable() const {
  return false;
}

bool AudioDeviceDummy::BuiltInNSIsAvailable() const {
  return false;
}

int32_t AudioDeviceDummy::EnableBuiltInAEC(bool enable) {
  return 0;
}

int32_t AudioDeviceDummy::EnableBuiltInAGC(bool enable) {
  return 0;
}

int32_t AudioDeviceDummy::EnableBuiltInNS(bool enable) {
  return 0;
}

#if defined(WEBRTC_IOS)
int AudioDeviceDummy::GetPlayoutAudioParameters(
    webrtc::AudioParameters* params) const {
  return 0;
}

int AudioDeviceDummy::GetRecordAudioParameters(
    webrtc::AudioParameters* params) const {
  return 0;
}
#endif  // WEBRTC_IOS

int32_t AudioDeviceDummy::SetAudioDeviceSink(webrtc::AudioDeviceSink* sink) const {
  return 0;
}

}  // namespace libwebrtc
