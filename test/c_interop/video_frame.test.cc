#include <memory>
#include <string>
#include <vector>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// --- RTCVideoFrame creation negatives ---

TEST(RTCVideoFrameNegative, Create0WithNullOutPointerFails) {
    EXPECT_EQ(RTCVideoFrame_Create0(640, 480, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCVideoFrameNegative, Create0RejectsBadDimensions) {
    rtcVideoFrameHandle frame = nullptr;
    // width must be >= 2 and a multiple of 4, height >= 2.
    EXPECT_EQ(RTCVideoFrame_Create0(0, 480, &frame),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(RTCVideoFrame_Create0(641, 480, &frame),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(RTCVideoFrame_Create0(640, 0, &frame),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(frame, nullptr);
}

TEST(RTCVideoFrameNegative, Create1RejectsMismatchedBufferLength) {
    std::vector<unsigned char> buffer(640 * 480 * 3 / 2, 0);
    rtcVideoFrameHandle frame = nullptr;
    // A buffer length that does not equal width*height*3/2 is kOutOfRange.
    EXPECT_EQ(RTCVideoFrame_Create1(640, 480, buffer.data(), 100, &frame),
              rtcResultU4::kOutOfRange);
    EXPECT_EQ(frame, nullptr);
}

TEST(RTCVideoFrameNegative, Create1RejectsNullBuffer) {
    rtcVideoFrameHandle frame = nullptr;
    EXPECT_EQ(RTCVideoFrame_Create1(640, 480, nullptr,
                                    640 * 480 * 3 / 2, &frame),
              rtcResultU4::kInvalidParameter);
    EXPECT_EQ(frame, nullptr);
}

TEST(RTCVideoFrameNegative, GetWidthWithNullHandleFails) {
    int width = 0;
    EXPECT_NE(RTCVideoFrame_GetWidth(nullptr, &width),
              rtcResultU4::kSuccess);
}

TEST(RTCVideoFrameNegative, GetWidthWithNullOutPointerFails) {
    EXPECT_EQ(RTCVideoFrame_GetWidth(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

// --- RTCVideoFrame fixture ---

class RTCVideoFrameTest : public ::testing::Test {
protected:
    static constexpr int kWidth = 640;
    static constexpr int kHeight = 480;

    void SetUp() override {
        ASSERT_EQ(RTCVideoFrame_Create0(kWidth, kHeight, &frame_),
                  rtcResultU4::kSuccess);
        ASSERT_NE(frame_, nullptr);
    }

    void TearDown() override {
        if (frame_) {
            RefCountedObject_Release(frame_);
            frame_ = nullptr;
        }
    }

    rtcVideoFrameHandle frame_ = nullptr;
};

TEST_F(RTCVideoFrameTest, WidthAndHeightMatchCreation) {
    int width = 0;
    int height = 0;
    EXPECT_EQ(RTCVideoFrame_GetWidth(frame_, &width), rtcResultU4::kSuccess);
    EXPECT_EQ(RTCVideoFrame_GetHeight(frame_, &height), rtcResultU4::kSuccess);
    EXPECT_EQ(width, kWidth);
    EXPECT_EQ(height, kHeight);
}

TEST_F(RTCVideoFrameTest, GetSizeReturnsValue) {
    int size = 0;
    EXPECT_EQ(RTCVideoFrame_GetSize(frame_, &size), rtcResultU4::kSuccess);
    EXPECT_GT(size, 0);
}

TEST_F(RTCVideoFrameTest, StridesArePositive) {
    int stride_y = 0;
    int stride_u = 0;
    int stride_v = 0;
    EXPECT_EQ(RTCVideoFrame_GetStrideY(frame_, &stride_y),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCVideoFrame_GetStrideU(frame_, &stride_u),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCVideoFrame_GetStrideV(frame_, &stride_v),
              rtcResultU4::kSuccess);
    EXPECT_GE(stride_y, kWidth);
    EXPECT_GT(stride_u, 0);
    EXPECT_GT(stride_v, 0);
}

TEST_F(RTCVideoFrameTest, DataPlanesAreNonNull) {
    const unsigned char* data_y = nullptr;
    const unsigned char* data_u = nullptr;
    const unsigned char* data_v = nullptr;
    EXPECT_EQ(RTCVideoFrame_GetDataY(frame_, &data_y), rtcResultU4::kSuccess);
    EXPECT_EQ(RTCVideoFrame_GetDataU(frame_, &data_u), rtcResultU4::kSuccess);
    EXPECT_EQ(RTCVideoFrame_GetDataV(frame_, &data_v), rtcResultU4::kSuccess);
    EXPECT_NE(data_y, nullptr);
    EXPECT_NE(data_u, nullptr);
    EXPECT_NE(data_v, nullptr);
}

TEST_F(RTCVideoFrameTest, GetDataYWithNullOutPointerFails) {
    EXPECT_EQ(RTCVideoFrame_GetDataY(frame_, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST_F(RTCVideoFrameTest, DefaultRotationIsZero) {
    rtcVideoRotation rotation = rtcVideoRotation::kVideoRotation_90;
    EXPECT_EQ(RTCVideoFrame_GetRotation(frame_, &rotation),
              rtcResultU4::kSuccess);
    EXPECT_EQ(rotation, rtcVideoRotation::kVideoRotation_0);
}

TEST_F(RTCVideoFrameTest, TimestampRoundTrips) {
    const rtcTimestamp ts = 1234567;
    EXPECT_EQ(RTCVideoFrame_SetTimestampInMicroseconds(frame_, ts),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCVideoFrame_GetTimestampInMicroseconds(frame_), ts);
}

TEST_F(RTCVideoFrameTest, GetTimestampWithNullHandleReturnsZero) {
    EXPECT_EQ(RTCVideoFrame_GetTimestampInMicroseconds(nullptr),
              static_cast<rtcTimestamp>(0));
}

TEST_F(RTCVideoFrameTest, CopyProducesIndependentFrame) {
    rtcVideoFrameHandle copy = nullptr;
    EXPECT_EQ(RTCVideoFrame_Copy(frame_, &copy), rtcResultU4::kSuccess);
    ASSERT_NE(copy, nullptr);

    int width = 0;
    int height = 0;
    EXPECT_EQ(RTCVideoFrame_GetWidth(copy, &width), rtcResultU4::kSuccess);
    EXPECT_EQ(RTCVideoFrame_GetHeight(copy, &height), rtcResultU4::kSuccess);
    EXPECT_EQ(width, kWidth);
    EXPECT_EQ(height, kHeight);

    RefCountedObject_Release(copy);
}

TEST_F(RTCVideoFrameTest, CopyWithNullOutPointerFails) {
    EXPECT_EQ(RTCVideoFrame_Copy(frame_, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST_F(RTCVideoFrameTest, ConvertToARGBRejectsNullDest) {
    EXPECT_EQ(RTCVideoFrame_ConvertToARGB(frame_, nullptr), 0);
}

TEST_F(RTCVideoFrameTest, ConvertToARGBRejectsInvalidDest) {
    rtcVideoFrameARGB dest{};  // data == nullptr -> rejected
    EXPECT_EQ(RTCVideoFrame_ConvertToARGB(frame_, &dest), 0);
}

TEST_F(RTCVideoFrameTest, ConvertToARGBProducesData) {
    const int stride = kWidth * 4;
    std::vector<uint8_t> argb(static_cast<size_t>(stride) * kHeight, 0);
    rtcVideoFrameARGB dest{};
    dest.type = rtcVideoFrameTypeARGB::kARGB;
    dest.width = kWidth;
    dest.height = kHeight;
    dest.data = argb.data();
    dest.stride = stride;
    EXPECT_GT(RTCVideoFrame_ConvertToARGB(frame_, &dest), 0);
}

TEST_F(RTCVideoFrameTest, ScaleFromAnotherFrame) {
    rtcVideoFrameHandle source = nullptr;
    ASSERT_EQ(RTCVideoFrame_Create0(320, 240, &source), rtcResultU4::kSuccess);
    ASSERT_NE(source, nullptr);

    int out = 0;
    EXPECT_EQ(RTCVideoFrame_ScaleFrom(frame_, source, &out),
              rtcResultU4::kSuccess);

    RefCountedObject_Release(source);
}

TEST_F(RTCVideoFrameTest, ScaleFromWithNullSourceFails) {
    int out = 0;
    EXPECT_EQ(RTCVideoFrame_ScaleFrom(frame_, nullptr, &out),
              rtcResultU4::kInvalidNativeHandle);
}

TEST_F(RTCVideoFrameTest, ScaleFromARGBWithNullSourceFails) {
    int out = 0;
    EXPECT_EQ(RTCVideoFrame_ScaleFromARGB(frame_, nullptr, &out),
              rtcResultU4::kInvalidParameter);
}

TEST_F(RTCVideoFrameTest, ScaleFromYUVWithNullSourceFails) {
    int out = 0;
    EXPECT_EQ(RTCVideoFrame_ScaleFromYUV(frame_, nullptr, &out),
              rtcResultU4::kInvalidParameter);
}

TEST_F(RTCVideoFrameTest, ClearWithNullHandleFails) {
    int retval = 1;
    EXPECT_EQ(RTCVideoFrame_Clear(nullptr, rtcVideoFrameClearType::kNone,
                                  &retval),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(retval, 0);
}

TEST_F(RTCVideoFrameTest, ClearSucceeds) {
    int retval = 0;
    EXPECT_EQ(RTCVideoFrame_Clear(frame_, rtcVideoFrameClearType::kNone,
                                  &retval),
              rtcResultU4::kSuccess);
}

// --- RTCVideoRenderer ---

TEST(RTCVideoRendererNegative, CreateWithNullOutPointerFails) {
    EXPECT_EQ(RTCVideoRenderer_Create(nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCVideoRendererNegative, RegisterCallbackWithNullHandleFails) {
    EXPECT_EQ(RTCVideoRenderer_RegisterFrameCallback(nullptr, nullptr,
                                                     nullptr),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCVideoRendererNegative, UnRegisterCallbackWithNullHandleFails) {
    EXPECT_EQ(RTCVideoRenderer_UnRegisterFrameCallback(nullptr),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCVideoRendererTest, CreateAndRegisterUnregister) {
    rtcVideoRendererHandle renderer = nullptr;
    EXPECT_EQ(RTCVideoRenderer_Create(&renderer), rtcResultU4::kSuccess);
    ASSERT_NE(renderer, nullptr);

    EXPECT_EQ(RTCVideoRenderer_RegisterFrameCallback(renderer, nullptr,
                                                     nullptr),
              rtcResultU4::kSuccess);
    EXPECT_EQ(RTCVideoRenderer_UnRegisterFrameCallback(renderer),
              rtcResultU4::kSuccess);

    RefCountedObject_Release(renderer);
}

}  // namespace libwebrtc
