/**
 * File provided for Reference Use Only by isoft (c) 2025.
 * Copyright (c) isoft. All rights reserved.
 * 
 */

#include "rtc_video_frame_impl.h"

#include "api/video/i420_buffer.h"
#include "libyuv/convert.h"
#include "libyuv/convert_argb.h"
#include "libyuv/convert_from.h"
#include "libyuv/convert_from_argb.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"

namespace libwebrtc {

VideoFrameBufferImpl::VideoFrameBufferImpl(
    rtc::scoped_refptr<webrtc::VideoFrameBuffer> frame_buffer)
    : buffer_(frame_buffer) {}

VideoFrameBufferImpl::VideoFrameBufferImpl(
    rtc::scoped_refptr<webrtc::I420Buffer> frame_buffer)
    : buffer_(frame_buffer) {}

VideoFrameBufferImpl::~VideoFrameBufferImpl() {}

scoped_refptr<RTCVideoFrame> VideoFrameBufferImpl::Copy() {
  scoped_refptr<VideoFrameBufferImpl> frame =
      scoped_refptr<VideoFrameBufferImpl>(
          new RefCountedObject<VideoFrameBufferImpl>(buffer_));
  return frame;
}

int VideoFrameBufferImpl::width() const { return buffer_->width(); }

int VideoFrameBufferImpl::height() const { return buffer_->height(); }

int VideoFrameBufferImpl::size() const {
  int buffer_size = (StrideY() * height()) + (StrideU() * (height() / 2)) + (StrideV() * (height() / 2));
  return buffer_size;
}

const uint8_t* VideoFrameBufferImpl::DataY() const {
  return buffer_->GetI420()->DataY();
}

const uint8_t* VideoFrameBufferImpl::DataU() const {
  return buffer_->GetI420()->DataU();
}

const uint8_t* VideoFrameBufferImpl::DataV() const {
  return buffer_->GetI420()->DataV();
}

int VideoFrameBufferImpl::StrideY() const {
  return buffer_->GetI420()->StrideY();
}

int VideoFrameBufferImpl::StrideU() const {
  return buffer_->GetI420()->StrideU();
}

int VideoFrameBufferImpl::StrideV() const {
  return buffer_->GetI420()->StrideV();
}

int VideoFrameBufferImpl::ConvertToARGB(Type type, uint8_t* dst_buffer,
                                        int dst_stride, int dest_width,
                                        int dest_height) {
  rtc::scoped_refptr<webrtc::I420Buffer> i420 =
      webrtc::I420Buffer::Rotate(*buffer_.get(), rotation_);

  rtc::scoped_refptr<webrtc::I420Buffer> dest =
      webrtc::I420Buffer::Create(dest_width, dest_height);

  dest->ScaleFrom(*i420.get());
  // ARGB buffer size
  int buf_size = dest->width() * dest->height() * (32 >> 3);
  switch (type) {
    case libwebrtc::RTCVideoFrame::Type::kARGB:
      libyuv::I420ToARGB(dest->DataY(), dest->StrideY(), dest->DataU(),
                         dest->StrideU(), dest->DataV(), dest->StrideV(),
                         dst_buffer, dest->width() * 32 / 8, dest->width(),
                         dest->height());
      break;
    case libwebrtc::RTCVideoFrame::Type::kBGRA:
      libyuv::I420ToBGRA(dest->DataY(), dest->StrideY(), dest->DataU(),
                         dest->StrideU(), dest->DataV(), dest->StrideV(),
                         dst_buffer, dest->width() * 32 / 8, dest->width(),
                         dest->height());
      break;
    case libwebrtc::RTCVideoFrame::Type::kABGR:
      libyuv::I420ToABGR(dest->DataY(), dest->StrideY(), dest->DataU(),
                         dest->StrideU(), dest->DataV(), dest->StrideV(),
                         dst_buffer, dest->width() * 32 / 8, dest->width(),
                         dest->height());
      break;
    case libwebrtc::RTCVideoFrame::Type::kRGBA:
      libyuv::I420ToRGBA(dest->DataY(), dest->StrideY(), dest->DataU(),
                         dest->StrideU(), dest->DataV(), dest->StrideV(),
                         dst_buffer, dest->width() * 32 / 8, dest->width(),
                         dest->height());
      break;
    default:
      break;
  }
  return buf_size;
}

int VideoFrameBufferImpl::ScaleFrom(scoped_refptr<RTCVideoFrame> source)
{
  if (nullptr == source || this == source.get()) {
    return 0;
  }
  scoped_refptr<VideoFrameBufferImpl> source_impl =
      scoped_refptr<VideoFrameBufferImpl>(
        static_cast<VideoFrameBufferImpl*>(source.get())
      );
  if (nullptr == source_impl->buffer_) {
    return 0;
  }
  rtc::scoped_refptr<webrtc::I420Buffer> i420 =
      webrtc::I420Buffer::Rotate(*buffer_.get(), rotation_);
  rtc::scoped_refptr<webrtc::I420Buffer> i420_source =
      webrtc::I420Buffer::Rotate(
        *source_impl->buffer_.get(), 
        static_cast<webrtc::VideoRotation>(source_impl->rotation())
      );
  i420->ScaleFrom(*i420_source.get());
  buffer_ = i420;

  return size();
}

int VideoFrameBufferImpl::ScaleFrom(
    Type type,
    const uint8_t* src_argb,
    int src_stride_argb,
    int src_width,
    int src_height
  )
{
  rtc::scoped_refptr<webrtc::I420Buffer> i420_src =
      webrtc::I420Buffer::Create(src_width, src_height);
  rtc::scoped_refptr<webrtc::I420Buffer> i420_dst =
      webrtc::I420Buffer::Create(width(), height());

  switch (type) {
    case libwebrtc::RTCVideoFrame::Type::kARGB:
      libyuv::ARGBToI420(src_argb, src_stride_argb,
                         i420_src->MutableDataY(), i420_src->StrideY(),
                         i420_src->MutableDataU(), i420_src->StrideU(),
                         i420_src->MutableDataV(), i420_src->StrideV(),
                         src_width, src_height);
      break;
    case libwebrtc::RTCVideoFrame::Type::kBGRA:
      libyuv::BGRAToI420(src_argb, src_stride_argb,
                         i420_src->MutableDataY(), i420_src->StrideY(),
                         i420_src->MutableDataU(), i420_src->StrideU(),
                         i420_src->MutableDataV(), i420_src->StrideV(),
                         src_width, src_height);
      break;
    case libwebrtc::RTCVideoFrame::Type::kABGR:
      libyuv::ABGRToI420(src_argb, src_stride_argb,
                         i420_src->MutableDataY(), i420_src->StrideY(),
                         i420_src->MutableDataU(), i420_src->StrideU(),
                         i420_src->MutableDataV(), i420_src->StrideV(),
                         src_width, src_height);
      break;
    case libwebrtc::RTCVideoFrame::Type::kRGBA:
      libyuv::RGBAToI420(src_argb, src_stride_argb,
                         i420_src->MutableDataY(), i420_src->StrideY(),
                         i420_src->MutableDataU(), i420_src->StrideU(),
                         i420_src->MutableDataV(), i420_src->StrideV(),
                         src_width, src_height);
      break;
    default:
      break;
  }

  i420_dst->ScaleFrom(*i420_src.get());
  buffer_ = i420_dst;

  return size();
}

int VideoFrameBufferImpl::Clear(RTCVideoFrameClearType clearType) {
  int buffer_size = size();
  if (buffer_size <= 0) {
    return 0;
  }

  switch (clearType)
  {
  case RTCVideoFrameClearType::kNoise:
    {
      const int noiseLevel = 50;
      const int height = buffer_->height();
      uint8_t* mutableDataY = const_cast<uint8_t*>(DataY());
      uint8_t* mutableDataU = const_cast<uint8_t*>(DataU());
      uint8_t* mutableDataV = const_cast<uint8_t*>(DataV());
      const int countY = StrideY() * height;
      const int countU = StrideU() * height / 2;
      const int countV = StrideV() * height / 2;

      for (int i = 0; i < countY; i++) {
        mutableDataY[i] = static_cast<uint8_t>((rand() % (256 - noiseLevel)) + noiseLevel);
      }
      memset(mutableDataU, 128, countU);
      memset(mutableDataV, 128, countV);
    }
    break;
  case RTCVideoFrameClearType::kColorBar:
    {
      const int width = buffer_->width();
      const int height = buffer_->height();
      const int blockWidth = width/ 8;
      uint8_t* mutableDataY = const_cast<uint8_t*>(DataY());
      uint8_t* mutableDataU = const_cast<uint8_t*>(DataU());
      uint8_t* mutableDataV = const_cast<uint8_t*>(DataV());
      const int strideY = StrideY();
      const int strideU = StrideU();
      const int strideV = StrideV();

      // Colorbar colors
      // Colors: White, Yellow, Cyan, Green, Magenta, Red, Blue, Black
      const uint8_t colorBarY[8] = {235, 214, 183, 162,  88,  73,  36,   0};
      const uint8_t colorBarU[8] = {128,  38, 157,  65, 188,  95, 218, 128};
      const uint8_t colorBarV[8] = {128, 142,  39,  54, 203, 226, 114, 128};

      for (int i = 0; i < 8; ++i) {
          for (int y = 0; y < height; ++y) {
              for (int x = i * blockWidth; x < (i + 1) * blockWidth; ++x) {
                  mutableDataY[y * strideY + x] = colorBarY[i];
              }
          }
      }

      for (int i = 0; i < 8; ++i) {
          for (int y = 0; y < height / 2; ++y) {
              for (int x = i * blockWidth / 2; x < (i + 1) * blockWidth / 2; ++x) {
                  mutableDataU[(y * strideU) + x] = colorBarU[i];
                  mutableDataV[(y * strideV) + x] = colorBarV[i];
              }
          }
      }
    }
    break;
  default: // Clear
    {
      const int height = buffer_->height();
      uint8_t* mutableDataY = const_cast<uint8_t*>(DataY());
      uint8_t* mutableDataU = const_cast<uint8_t*>(DataU());
      uint8_t* mutableDataV = const_cast<uint8_t*>(DataV());
      const int countY = StrideY() * height;
      const int countU = StrideU() * height / 2;
      const int countV = StrideV() * height / 2;
      memset(mutableDataY, 0, countY);
      memset(mutableDataU, 128, countU);
      memset(mutableDataV, 128, countV);
    }
    break;
  }
  return buffer_size;
}

libwebrtc::RTCVideoFrame::VideoRotation VideoFrameBufferImpl::rotation() {
  switch (rotation_) {
    case webrtc::kVideoRotation_0:
      return RTCVideoFrame::kVideoRotation_0;
    case webrtc::kVideoRotation_90:
      return RTCVideoFrame::kVideoRotation_90;
    case webrtc::kVideoRotation_180:
      return RTCVideoFrame::kVideoRotation_180;
    case webrtc::kVideoRotation_270:
      return RTCVideoFrame::kVideoRotation_270;
    default:
      break;
  }
  return RTCVideoFrame::kVideoRotation_0;
}

scoped_refptr<RTCVideoFrame> RTCVideoFrame::Create(int width, int height) {
  RTC_DCHECK(width > 1);
  RTC_DCHECK(height > 1);

  rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer = webrtc::I420Buffer::Create(
      width, height);
  
  scoped_refptr<VideoFrameBufferImpl> frame =
      scoped_refptr<VideoFrameBufferImpl>(
          new RefCountedObject<VideoFrameBufferImpl>(i420_buffer));
  return frame;
}

scoped_refptr<RTCVideoFrame> RTCVideoFrame::Create(int width, int height,
                                                   const uint8_t* buffer,
                                                   int length) {
  int stride_y = width;
  int stride_uv = (width + 1) / 2;

  int size_y = stride_y * height;
  int size_u = stride_uv * height / 2;
  // int size_v = size_u;

  RTC_DCHECK(length == (width * height * 3) / 2);

  const uint8_t* data_y = buffer;
  const uint8_t* data_u = buffer + size_y;
  const uint8_t* data_v = buffer + size_y + size_u;

  rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer = webrtc::I420Buffer::Copy(
      width, height, data_y, stride_y, data_u, stride_uv, data_v, stride_uv);

  scoped_refptr<VideoFrameBufferImpl> frame =
      scoped_refptr<VideoFrameBufferImpl>(
          new RefCountedObject<VideoFrameBufferImpl>(i420_buffer));
  return frame;
}

scoped_refptr<RTCVideoFrame> RTCVideoFrame::Create(
    int width, int height, const uint8_t* data_y, int stride_y,
    const uint8_t* data_u, int stride_u, const uint8_t* data_v, int stride_v) {
  rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer = webrtc::I420Buffer::Copy(
      width, height, data_y, stride_y, data_u, stride_u, data_v, stride_v);

  scoped_refptr<VideoFrameBufferImpl> frame =
      scoped_refptr<VideoFrameBufferImpl>(
          new RefCountedObject<VideoFrameBufferImpl>(i420_buffer));
  return frame;
}

}  // namespace libwebrtc
