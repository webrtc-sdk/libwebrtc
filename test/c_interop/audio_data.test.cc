#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// Fixture that owns a freshly created RTCAudioData object.
class RTCAudioDataTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 10ms of 16-bit stereo audio at 48kHz.
        const int sample_rate_hz = 48000;
        const unsigned int num_channels = 2;
        const unsigned int bits_per_sample = 16;
        const size_t samples_per_channel = sample_rate_hz / 100;
        buffer_.assign(samples_per_channel * num_channels *
                           (bits_per_sample / 8),
                       0);

        ASSERT_EQ(RTCAudioData_Create(0, buffer_.data(), bits_per_sample,
                                      sample_rate_hz, num_channels, &audio_),
                  rtcResultU4::kSuccess);
        ASSERT_NE(audio_, nullptr);
    }

    void TearDown() override {
        if (audio_) {
            RefCountedObject_Release(audio_);
            audio_ = nullptr;
        }
    }

    std::vector<unsigned char> buffer_;
    rtcAudioDataHandle audio_ = nullptr;
};

TEST_F(RTCAudioDataTest, CreateWithNullOutPointerFails) {
    EXPECT_EQ(RTCAudioData_Create(0, buffer_.data(), 16, 48000, 2, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST_F(RTCAudioDataTest, GetAndSetIndex) {
    unsigned int index = 99;
    EXPECT_EQ(RTCAudioData_GetIndex(audio_, &index), rtcResultU4::kSuccess);
    EXPECT_EQ(index, 0u);

    EXPECT_EQ(RTCAudioData_SetIndex(audio_, 42), rtcResultU4::kSuccess);
    EXPECT_EQ(RTCAudioData_GetIndex(audio_, &index), rtcResultU4::kSuccess);
    EXPECT_EQ(index, 42u);
}

TEST_F(RTCAudioDataTest, GetData) {
    const unsigned char* data = nullptr;
    EXPECT_EQ(RTCAudioData_GetData(audio_, &data), rtcResultU4::kSuccess);
}

TEST_F(RTCAudioDataTest, GetDataSize) {
    int size = -1;
    EXPECT_EQ(RTCAudioData_GetDataSize(audio_, &size), rtcResultU4::kSuccess);
    EXPECT_GE(size, 0);
}

TEST_F(RTCAudioDataTest, GetBitsPerSample) {
    int bits = 0;
    EXPECT_EQ(RTCAudioData_GetBitsPerSample(audio_, &bits),
              rtcResultU4::kSuccess);
    EXPECT_EQ(bits, 16);
}

TEST_F(RTCAudioDataTest, GetSamplesPerChannel) {
    int samples = -1;
    EXPECT_EQ(RTCAudioData_GetSamplesPerChannel(audio_, &samples),
              rtcResultU4::kSuccess);
    EXPECT_GE(samples, 0);
}

TEST_F(RTCAudioDataTest, GetSampleRateHz) {
    int rate = 0;
    EXPECT_EQ(RTCAudioData_GetSampleRateHz(audio_, &rate),
              rtcResultU4::kSuccess);
    EXPECT_EQ(rate, 48000);
}

TEST_F(RTCAudioDataTest, GetNumChannels) {
    int channels = 0;
    EXPECT_EQ(RTCAudioData_GetNumChannels(audio_, &channels),
              rtcResultU4::kSuccess);
    EXPECT_EQ(channels, 2);
}

TEST_F(RTCAudioDataTest, ScaleFrom) {
    rtcAudioDataHandle source = nullptr;
    ASSERT_EQ(RTCAudioData_Create(0, buffer_.data(), 16, 48000, 2, &source),
              rtcResultU4::kSuccess);
    ASSERT_NE(source, nullptr);

    int retval = -1;
    EXPECT_EQ(RTCAudioData_ScaleFrom(audio_, source, &retval),
              rtcResultU4::kSuccess);

    RefCountedObject_Release(source);
}

TEST_F(RTCAudioDataTest, Clear) {
    int retval = -1;
    EXPECT_EQ(RTCAudioData_Clear(audio_, rtcAudioDataToneFrequency::kNone,
                                 &retval),
              rtcResultU4::kSuccess);
}

// --- Negative paths ---

TEST_F(RTCAudioDataTest, GetIndexNullOutFails) {
    EXPECT_EQ(RTCAudioData_GetIndex(audio_, nullptr), rtcResultU4::kInvalidPointer);
}

TEST_F(RTCAudioDataTest, GetIndexNullHandleFails) {
    unsigned int index = 0;
    EXPECT_EQ(RTCAudioData_GetIndex(nullptr, &index),
              rtcResultU4::kInvalidNativeHandle);
}

TEST_F(RTCAudioDataTest, SetIndexNullHandleFails) {
    EXPECT_EQ(RTCAudioData_SetIndex(nullptr, 1),
              rtcResultU4::kInvalidNativeHandle);
}

TEST_F(RTCAudioDataTest, GetDataNullOutFails) {
    EXPECT_EQ(RTCAudioData_GetData(audio_, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST_F(RTCAudioDataTest, GetDataNullHandleFails) {
    const unsigned char* data = nullptr;
    EXPECT_EQ(RTCAudioData_GetData(nullptr, &data),
              rtcResultU4::kInvalidNativeHandle);
}

TEST_F(RTCAudioDataTest, GetDataSizeNullOutFails) {
    EXPECT_EQ(RTCAudioData_GetDataSize(audio_, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST_F(RTCAudioDataTest, GetBitsPerSampleNullHandleFails) {
    int bits = 0;
    EXPECT_EQ(RTCAudioData_GetBitsPerSample(nullptr, &bits),
              rtcResultU4::kInvalidNativeHandle);
}

TEST_F(RTCAudioDataTest, ScaleFromNullHandleFails) {
    int retval = 0;
    EXPECT_EQ(RTCAudioData_ScaleFrom(nullptr, audio_, &retval),
              rtcResultU4::kInvalidNativeHandle);
}

TEST_F(RTCAudioDataTest, ScaleFromNullSourceFails) {
    int retval = 0;
    EXPECT_EQ(RTCAudioData_ScaleFrom(audio_, nullptr, &retval),
              rtcResultU4::kInvalidParameter);
}

TEST_F(RTCAudioDataTest, ClearNullHandleFails) {
    int retval = 0;
    EXPECT_EQ(RTCAudioData_Clear(nullptr, rtcAudioDataToneFrequency::kNone,
                                 &retval),
              rtcResultU4::kInvalidNativeHandle);
}

}  // namespace libwebrtc
