#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_video_frame.h"
#include "rtc_video_renderer.h"
#include "rtc_types.h"
#include "libwebrtc.h"

namespace libwebrtc {

// --- Construction variants ---

TEST(RTCVideoFrameCreate, CreateFromDimensionsSucceeds) {
  scoped_refptr<RTCVideoFrame> frame = RTCVideoFrame::Create(640, 480);
  ASSERT_TRUE(frame.get() != nullptr);
  EXPECT_EQ(frame->width(), 640);
  EXPECT_EQ(frame->height(), 480);
}

TEST(RTCVideoFrameCreate, CreateFromI420BufferSucceeds) {
  const int width = 640;
  const int height = 480;
  std::vector<uint8_t> buffer(width * height * 3 / 2, 0);
  scoped_refptr<RTCVideoFrame> frame =
      RTCVideoFrame::Create(width, height, buffer.data(),
                            static_cast<int>(buffer.size()));
  ASSERT_TRUE(frame.get() != nullptr);
  EXPECT_EQ(frame->width(), width);
  EXPECT_EQ(frame->height(), height);
}

TEST(RTCVideoFrameCreate, CreateFromPlanesSucceeds) {
  const int width = 640;
  const int height = 480;
  const int stride_y = width;
  const int stride_uv = (width + 1) / 2;
  std::vector<uint8_t> y(stride_y * height, 16);
  std::vector<uint8_t> u(stride_uv * ((height + 1) / 2), 128);
  std::vector<uint8_t> v(stride_uv * ((height + 1) / 2), 128);
  scoped_refptr<RTCVideoFrame> frame = RTCVideoFrame::Create(
      width, height, y.data(), stride_y, u.data(), stride_uv, v.data(),
      stride_uv);
  ASSERT_TRUE(frame.get() != nullptr);
  EXPECT_EQ(frame->width(), width);
  EXPECT_EQ(frame->height(), height);
}

// --- Fixture for a default I420 frame ---

class RTCVideoFrameTest : public ::testing::Test {
 protected:
  static constexpr int kWidth = 640;
  static constexpr int kHeight = 480;

  void SetUp() override {
    frame_ = RTCVideoFrame::Create(kWidth, kHeight);
    ASSERT_TRUE(frame_.get() != nullptr);
  }

  void TearDown() override { frame_ = nullptr; }

  scoped_refptr<RTCVideoFrame> frame_;
};

TEST_F(RTCVideoFrameTest, WidthAndHeightMatchCreation) {
  EXPECT_EQ(frame_->width(), kWidth);
  EXPECT_EQ(frame_->height(), kHeight);
}

TEST_F(RTCVideoFrameTest, SizeIsPositive) {
  EXPECT_GT(frame_->size(), 0);
}

TEST_F(RTCVideoFrameTest, DefaultRotationIsZero) {
  EXPECT_EQ(frame_->rotation(), RTCVideoFrame::kVideoRotation_0);
}

TEST_F(RTCVideoFrameTest, StridesArePlausible) {
  EXPECT_GE(frame_->StrideY(), kWidth);
  EXPECT_GT(frame_->StrideU(), 0);
  EXPECT_GT(frame_->StrideV(), 0);
}

TEST_F(RTCVideoFrameTest, DataPlanesAreNonNull) {
  EXPECT_TRUE(frame_->DataY() != nullptr);
  EXPECT_TRUE(frame_->DataU() != nullptr);
  EXPECT_TRUE(frame_->DataV() != nullptr);
}

TEST_F(RTCVideoFrameTest, TimestampRoundTrips) {
  const int64_t ts = 1234567;
  frame_->set_timestamp_us(ts);
  EXPECT_EQ(frame_->timestamp_us(), ts);
}

TEST_F(RTCVideoFrameTest, CopyProducesIndependentFrame) {
  scoped_refptr<RTCVideoFrame> copy = frame_->Copy();
  ASSERT_TRUE(copy.get() != nullptr);
  EXPECT_EQ(copy->width(), kWidth);
  EXPECT_EQ(copy->height(), kHeight);
}

TEST_F(RTCVideoFrameTest, ConvertToARGBProducesData) {
  const int stride = kWidth * 4;
  std::vector<uint8_t> argb(static_cast<size_t>(stride) * kHeight, 0);
  RTCVideoFrameARGB dest;
  dest.type = RTCVideoFrameTypeARGB::kARGB;
  dest.width = kWidth;
  dest.height = kHeight;
  dest.data = argb.data();
  dest.stride = stride;
  EXPECT_GT(frame_->ConvertToARGB(&dest), 0);
}

TEST_F(RTCVideoFrameTest, ConvertToARGBRejectsNullDest) {
  EXPECT_EQ(frame_->ConvertToARGB(nullptr), 0);
}

TEST_F(RTCVideoFrameTest, ScaleFromAnotherFrame) {
  scoped_refptr<RTCVideoFrame> source = RTCVideoFrame::Create(320, 240);
  ASSERT_TRUE(source.get() != nullptr);
  // Scaling into a 640x480 frame returns the resulting frame size (> 0).
  EXPECT_GT(frame_->ScaleFrom(source), 0);
}

TEST_F(RTCVideoFrameTest, ScaleFromNullArgbReturnsZero) {
  RTCVideoFrameARGB* argb = nullptr;
  EXPECT_EQ(frame_->ScaleFrom(argb), 0);
}

TEST_F(RTCVideoFrameTest, ScaleFromNullYuvReturnsZero) {
  RTCVideoFrameYUV* yuv = nullptr;
  EXPECT_EQ(frame_->ScaleFrom(yuv), 0);
}

TEST_F(RTCVideoFrameTest, ClearSucceeds) {
  // Clear returns the number of bytes cleared (the full frame buffer).
  EXPECT_EQ(frame_->Clear(RTCVideoFrameClearType::kNone), frame_->size());
}

// Note: RTCVideoRenderer<T> is a templated interface whose Create() is not
// instantiated in the prebuilt library, so it is not exercised here.

}  // namespace libwebrtc
