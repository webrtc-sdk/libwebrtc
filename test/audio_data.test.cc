#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_audio_data.h"
#include "rtc_types.h"
#include "libwebrtc.h"

namespace libwebrtc {

// Fixture that owns a freshly created RTCAudioData object via the C++ API.
class RTCAudioDataTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // 10ms of 16-bit stereo audio at 48kHz.
    const int sample_rate_hz = 48000;
    const uint32_t num_channels = 2;
    const uint32_t bits_per_sample = 16;
    const size_t samples_per_channel = sample_rate_hz / 100;
    buffer_.assign(
        samples_per_channel * num_channels * (bits_per_sample / 8), 0);

    audio_ = RTCAudioData::Create(0, buffer_.data(), bits_per_sample,
                                  sample_rate_hz, num_channels);
    ASSERT_TRUE(audio_.get() != nullptr);
  }

  void TearDown() override { audio_ = nullptr; }

  std::vector<uint8_t> buffer_;
  scoped_refptr<RTCAudioData> audio_;
};

TEST_F(RTCAudioDataTest, DefaultCreateSucceeds) {
  scoped_refptr<RTCAudioData> empty = RTCAudioData::Create();
  ASSERT_TRUE(empty.get() != nullptr);
}

TEST_F(RTCAudioDataTest, GetAndSetIndex) {
  EXPECT_EQ(audio_->index(), 0u);
  audio_->set_index(42);
  EXPECT_EQ(audio_->index(), 42u);
}

TEST_F(RTCAudioDataTest, DataPointerIsNonNull) {
  EXPECT_TRUE(audio_->data() != nullptr);
}

TEST_F(RTCAudioDataTest, DataSizeMatchesBuffer) {
  EXPECT_EQ(audio_->data_size(), buffer_.size());
}

TEST_F(RTCAudioDataTest, BitsPerSampleMatchesCreation) {
  EXPECT_EQ(audio_->bits_per_sample(), 16u);
}

TEST_F(RTCAudioDataTest, SampleRateMatchesCreation) {
  EXPECT_EQ(audio_->sample_rate_hz(), 48000);
}

TEST_F(RTCAudioDataTest, NumChannelsMatchesCreation) {
  EXPECT_EQ(audio_->num_channels(), 2u);
}

TEST_F(RTCAudioDataTest, SamplesPerChannelIsConsistent) {
  // 480 samples per channel for 10ms @ 48kHz.
  EXPECT_EQ(audio_->samples_per_channel(), 480u);
}

TEST_F(RTCAudioDataTest, ScaleFromAnotherAudioData) {
  scoped_refptr<RTCAudioData> source =
      RTCAudioData::Create(0, buffer_.data(), 16, 48000, 2);
  ASSERT_TRUE(source.get() != nullptr);
  // ScaleFrom returns the number of bytes written into the destination buffer.
  EXPECT_EQ(audio_->ScaleFrom(source), static_cast<int>(buffer_.size()));
}

TEST_F(RTCAudioDataTest, ClearEmptiesBuffer) {
  // Clear returns the number of bytes processed (the full buffer).
  EXPECT_EQ(audio_->Clear(RTCAudioDataToneFrequency::kNone),
            static_cast<int>(buffer_.size()));
}

TEST_F(RTCAudioDataTest, ClearWithToneSucceeds) {
  EXPECT_EQ(audio_->Clear(RTCAudioDataToneFrequency::kTone1kHz),
            static_cast<int>(buffer_.size()));
}

}  // namespace libwebrtc
