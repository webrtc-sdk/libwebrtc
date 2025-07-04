/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef INTERNAL_CUSTOM_AUDIO_STATE_H_
#define INTERNAL_CUSTOM_AUDIO_STATE_H_

#include <map>
#include <memory>

#include "api/sequence_checker.h"
#include "audio/audio_transport_impl.h"
#include "call/audio_sender.h"
#include "call/audio_state.h"
#include "rtc_base/containers/flat_set.h"
#include "rtc_base/ref_count.h"
#include "rtc_base/task_utils/repeating_task.h"
#include "rtc_base/thread_annotations.h"

namespace webrtc {

class CustomAudioTransportImpl;
class AudioSendStream;
class AudioReceiveStreamInterface;

class CustomAudioState : public webrtc::AudioState {
 public:
  explicit CustomAudioState(
      const AudioState::Config& config,
      std::unique_ptr<CustomAudioTransportImpl> custom_audio_transport);

  CustomAudioState() = delete;
  CustomAudioState(const CustomAudioState&) = delete;
  CustomAudioState& operator=(const CustomAudioState&) = delete;

  ~CustomAudioState() override;

  AudioProcessing* audio_processing() override;
  AudioTransport* audio_transport() override;

  void SetPlayout(bool enabled) override;
  void SetRecording(bool enabled) override;

  void SetStereoChannelSwapping(bool enable) override;

  void OnMuteStreamChanged() override;

  AudioDeviceModule* audio_device_module() {
    RTC_DCHECK(config_.audio_device_module);
    return config_.audio_device_module.get();
  }

  void AddReceivingStream(webrtc::AudioReceiveStreamInterface* stream) override;
  void RemoveReceivingStream(
      webrtc::AudioReceiveStreamInterface* stream) override;

  void AddSendingStream(webrtc::AudioSendStream* stream, int sample_rate_hz,
                        size_t num_channels) override;
  void RemoveSendingStream(webrtc::AudioSendStream* stream) override;

 private:
  void UpdateAudioTransportWithSendingStreams();
  void UpdateNullAudioPollerState() RTC_RUN_ON(&thread_checker_);

  // Returns true when at least 1 stream exists and all streams are not muted.
  bool ShouldRecord();

  SequenceChecker thread_checker_;
  SequenceChecker process_thread_checker_{SequenceChecker::kDetached};
  const webrtc::AudioState::Config config_;
  bool recording_enabled_ = true;
  bool playout_enabled_ = true;

  // Transports mixed audio from the mixer to the audio device and
  // recorded audio to the sending streams.
  std::unique_ptr<CustomAudioTransportImpl> audio_transport_;

  // Null audio poller is used to continue polling the audio streams if audio
  // playout is disabled so that audio processing still happens and the audio
  // stats are still updated.
  RepeatingTaskHandle null_audio_poller_ RTC_GUARDED_BY(&thread_checker_);

  webrtc::flat_set<webrtc::AudioReceiveStreamInterface*> receiving_streams_;
  struct StreamProperties {
    int sample_rate_hz = 0;
    size_t num_channels = 0;
  };
  std::map<webrtc::AudioSendStream*, StreamProperties> sending_streams_;
};

}  // namespace webrtc

#endif  // INTERNAL_CUSTOM_AUDIO_STATE_H_
