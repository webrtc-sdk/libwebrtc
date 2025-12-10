#ifndef LIB_WEBRTC_VIDEO_FRAME_IMPL_HXX
#define LIB_WEBRTC_VIDEO_FRAME_IMPL_HXX

#include "api/video/i420_buffer.h"
#include "api/video/video_frame_buffer.h"
#include "api/video/video_rotation.h"
#include "common_video/include/video_frame_buffer.h"
#include "rtc_video_frame.h"

namespace libwebrtc {

class VideoFrameBufferImpl : public RTCVideoFrame {
 public:
  VideoFrameBufferImpl(
      webrtc::scoped_refptr<webrtc::VideoFrameBuffer> frame_buffer);
  VideoFrameBufferImpl(webrtc::scoped_refptr<webrtc::I420Buffer> frame_buffer);

  virtual ~VideoFrameBufferImpl();

  scoped_refptr<RTCVideoFrame> Copy() override;

 public:
  int width() const override;

  int height() const override;
  
  int size() const override;

  const uint8_t* DataY() const override;

  const uint8_t* DataU() const override;

  const uint8_t* DataV() const override;

  int StrideY() const override;

  int StrideU() const override;

  int StrideV() const override;

  int ConvertToARGB(RTCVideoFrameARGB* pDest) override;

  int ScaleFrom(scoped_refptr<RTCVideoFrame> source) override;

  int ScaleFrom(RTCVideoFrameARGB* source) override;

  int ScaleFrom(RTCVideoFrameYUV* source) override;

  int Clear(RTCVideoFrameClearType clearType) override;

  webrtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer() { return buffer_; }

  // System monotonic clock, same timebase as webrtc::TimeMicros().
  int64_t timestamp_us() const override{ return timestamp_us_; }
  void set_timestamp_us(int64_t timestamp_us) override { timestamp_us_ = timestamp_us; }

  virtual RTCVideoFrame::VideoRotation rotation() override;

  webrtc::VideoRotation rotation() const { return rotation_; }

  void set_rotation(webrtc::VideoRotation rotation) { rotation_ = rotation; }

 private:
  webrtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer_;
  int64_t timestamp_us_ = 0;
  webrtc::VideoRotation rotation_ = webrtc::kVideoRotation_0;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_VIDEO_FRAME_IMPL_HXX
