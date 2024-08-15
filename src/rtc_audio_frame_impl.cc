#include "rtc_audio_frame_impl.h"

#include "audio/utility/audio_frame_operations.h"

namespace libwebrtc {

scoped_refptr<RTCAudioFrame> RTCAudioFrame::Create() {
  return scoped_refptr<RTCAudioFrame>(
      new RefCountedObject<RTCAudioFrameImpl>());
}

scoped_refptr<RTCAudioFrame> RTCAudioFrame::Create(
    uint32_t timestamp, const int16_t* data, size_t samples_per_channel,
    int sample_rate_hz, size_t num_channels /*= 1*/) {
  auto audio_frame =
      scoped_refptr<RTCAudioFrame>(new RefCountedObject<RTCAudioFrameImpl>());
  audio_frame->UpdateFrame(timestamp, data, samples_per_channel, sample_rate_hz,
                           num_channels);
  return audio_frame;
}

RTCAudioFrameImpl::RTCAudioFrameImpl(webrtc::AudioFrame& buffer) {
  buffer_.CopyFrom(buffer);
}

RTCAudioFrameImpl::~RTCAudioFrameImpl() {}

void RTCAudioFrameImpl::UpdateFrame(uint32_t timestamp, const int16_t* data,
                                    size_t samples_per_channel,
                                    int sample_rate_hz,
                                    size_t num_channels /*= 1*/) {
  buffer_.UpdateFrame(timestamp, data, samples_per_channel, sample_rate_hz,
                      webrtc::AudioFrame::kNormalSpeech,
                      webrtc::AudioFrame::kVadUnknown, num_channels);
}

void RTCAudioFrameImpl::CopyFrom(const scoped_refptr<RTCAudioFrame> src) {
  RTCAudioFrameImpl* frame = static_cast<RTCAudioFrameImpl*>(src.get());
  buffer_.CopyFrom(frame->buffer_);
}

void RTCAudioFrameImpl::Add(const scoped_refptr<RTCAudioFrame> frame_to_add) {
  RTCAudioFrameImpl* frame =
      static_cast<RTCAudioFrameImpl*>(frame_to_add.get());
  webrtc::AudioFrameOperations::Add(frame->buffer_, &buffer_);
}

void RTCAudioFrameImpl::Mute() { buffer_.Mute(); }

const int16_t* RTCAudioFrameImpl::data() { return buffer_.data(); }

size_t RTCAudioFrameImpl::samples_per_channel() {
  return buffer_.samples_per_channel_;
}

int RTCAudioFrameImpl::sample_rate_hz() { return buffer_.sample_rate_hz_; }

size_t RTCAudioFrameImpl::num_channels() { return buffer_.num_channels_; }

uint32_t RTCAudioFrameImpl::timestamp() { return buffer_.timestamp_; }

}  // namespace libwebrtc
