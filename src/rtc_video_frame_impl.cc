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

int VideoFrameBufferImpl::ConvertToARGB(RTCVideoFrameARGB* pDest)
{
  if (pDest == nullptr) {
    return 0;
  }

  rtc::scoped_refptr<webrtc::I420Buffer> i420 =
      webrtc::I420Buffer::Rotate(*buffer_.get(), rotation_);

  rtc::scoped_refptr<webrtc::I420Buffer> dest =
      webrtc::I420Buffer::Create(pDest->width, pDest->height);

  dest->ScaleFrom(*i420.get());
  // ARGB buffer size
  int buf_size = dest->width() * dest->height() * (32 >> 3);
  switch (pDest->type) {
    case RTCVideoFrameTypeARGB::kARGB:
      libyuv::I420ToARGB(dest->DataY(), dest->StrideY(),
                         dest->DataU(), dest->StrideU(),
                         dest->DataV(), dest->StrideV(),
                         pDest->data, pDest->stride,
                         pDest->width,
                         pDest->height);
      break;
    case RTCVideoFrameTypeARGB::kBGRA:
      libyuv::I420ToBGRA(dest->DataY(), dest->StrideY(),
                         dest->DataU(), dest->StrideU(),
                         dest->DataV(), dest->StrideV(),
                         pDest->data, pDest->stride,
                         pDest->width,
                         pDest->height);
      break;
    case RTCVideoFrameTypeARGB::kABGR:
      libyuv::I420ToABGR(dest->DataY(), dest->StrideY(),
                         dest->DataU(), dest->StrideU(),
                         dest->DataV(), dest->StrideV(),
                         pDest->data, pDest->stride,
                         pDest->width,
                         pDest->height);
      break;
    case RTCVideoFrameTypeARGB::kRGBA:
      libyuv::I420ToRGBA(dest->DataY(), dest->StrideY(),
                         dest->DataU(), dest->StrideU(),
                         dest->DataV(), dest->StrideV(),
                         pDest->data, pDest->stride,
                         pDest->width,
                         pDest->height);
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

int VideoFrameBufferImpl::ScaleFrom(RTCVideoFrameARGB* source)
{
  if (source == nullptr ||
      source->data == nullptr ||
      source->width < 16 || 
      source->height < 16 ||
      source->stride < 64 ||
      (source->width * 4) > source->stride)
  {
    return 0;
  }

  switch (source->type) {
    case RTCVideoFrameTypeARGB::kARGB:
    case RTCVideoFrameTypeARGB::kBGRA:
    case RTCVideoFrameTypeARGB::kABGR:
    case RTCVideoFrameTypeARGB::kRGBA:
      break;
    default:
      return 0; // Unknown type
  }

  rtc::scoped_refptr<webrtc::I420Buffer> i420_src =
      webrtc::I420Buffer::Create(source->width, source->height);
  rtc::scoped_refptr<webrtc::I420Buffer> i420_dst =
      webrtc::I420Buffer::Create(width(), height());

  switch (source->type) {
    case RTCVideoFrameTypeARGB::kARGB:
      libyuv::ARGBToI420(source->data, source->stride,
                         i420_src->MutableDataY(), i420_src->StrideY(),
                         i420_src->MutableDataU(), i420_src->StrideU(),
                         i420_src->MutableDataV(), i420_src->StrideV(),
                         source->width, source->height);
      break;
    case RTCVideoFrameTypeARGB::kBGRA:
      libyuv::BGRAToI420(source->data, source->stride,
                         i420_src->MutableDataY(), i420_src->StrideY(),
                         i420_src->MutableDataU(), i420_src->StrideU(),
                         i420_src->MutableDataV(), i420_src->StrideV(),
                         source->width, source->height);
      break;
    case RTCVideoFrameTypeARGB::kABGR:
      libyuv::ABGRToI420(source->data, source->stride,
                         i420_src->MutableDataY(), i420_src->StrideY(),
                         i420_src->MutableDataU(), i420_src->StrideU(),
                         i420_src->MutableDataV(), i420_src->StrideV(),
                         source->width, source->height);
      break;
    case RTCVideoFrameTypeARGB::kRGBA:
      libyuv::RGBAToI420(source->data, source->stride,
                         i420_src->MutableDataY(), i420_src->StrideY(),
                         i420_src->MutableDataU(), i420_src->StrideU(),
                         i420_src->MutableDataV(), i420_src->StrideV(),
                         source->width, source->height);
      break;
    default:
      break;
  }

  i420_dst->ScaleFrom(*i420_src.get());
  buffer_ = i420_dst;

  return size();
}

int VideoFrameBufferImpl::ScaleFrom(RTCVideoFrameYUV* source)
{
  if (source == nullptr ||
      source->width < 16 || 
      source->height < 16 ||
      source->dataY == nullptr ||
      source->dataU == nullptr ||
      source->dataV == nullptr ||
      source->strideY < 16 ||
      source->strideU < 8 ||
      source->strideV < 8)
  {
    return 0;
  }

  switch (source->type) {
    case RTCVideoFrameTypeYUV::kI420:
    case RTCVideoFrameTypeYUV::kYUY2:
    case RTCVideoFrameTypeYUV::kNV12:
      break;
    default:
      return 0; // Unknown type
  }

  if (source->type == RTCVideoFrameTypeYUV::kI420) {
    scoped_refptr<RTCVideoFrame> videoFrame = RTCVideoFrame::Create(
      source->width,
      source->height,
      static_cast<const uint8_t*>(source->dataY),
      source->strideY,
      static_cast<const uint8_t*>(source->dataU),
      source->strideU,
      static_cast<const uint8_t*>(source->dataV),
      source->strideV
    );

    return ScaleFrom(videoFrame);
  }
  else if (source->type == RTCVideoFrameTypeYUV::kYUY2) {
    rtc::scoped_refptr<webrtc::I420Buffer> i420_src =
      webrtc::I420Buffer::Create(source->width, source->height);
    rtc::scoped_refptr<webrtc::I420Buffer> i420_dst =
        webrtc::I420Buffer::Create(width(), height());
        
    int retVal = libyuv::YUY2ToI420(
      static_cast<const uint8_t*>(source->dataY),
      source->strideY,
      i420_src->MutableDataY(), i420_src->StrideY(),
      i420_src->MutableDataU(), i420_src->StrideU(),
      i420_src->MutableDataV(), i420_src->StrideV(),
      source->width, source->height
    );

    if (retVal != 0) {
      return 0; // error
    }

    i420_dst->ScaleFrom(*i420_src.get());
    buffer_ = i420_dst;

    return size();
  }
  else if (source->type == RTCVideoFrameTypeYUV::kNV12) {
    return 0; // Not yet supported !!!
  }

  return 0;
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
