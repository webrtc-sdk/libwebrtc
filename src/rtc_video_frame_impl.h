#ifndef LIB_WEBRTC_VIDEO_FRAME_IMPL_HXX
#define LIB_WEBRTC_VIDEO_FRAME_IMPL_HXX

#include "rtc_video_frame.h"

#include "api/video/i420_buffer.h"
#include "api/video/video_frame_buffer.h"
#include "api/video/video_rotation.h"
#include "common_video/include/video_frame_buffer.h"

namespace libwebrtc {

class VideoFrameBufferImpl : public RTCVideoFrame {
 public:
  VideoFrameBufferImpl(
      rtc::scoped_refptr<webrtc::VideoFrameBuffer> frame_buffer);
  VideoFrameBufferImpl(rtc::scoped_refptr<webrtc::I420Buffer> frame_buffer);

  virtual ~VideoFrameBufferImpl();

  scoped_refptr<RTCVideoFrame> Copy() override;

 public:
  int width() const override;

  int height() const override;

  const uint8_t* DataY() const override;

  const uint8_t* DataU() const override;

  const uint8_t* DataV() const override;

  int StrideY() const override;

  int StrideU() const override;

  int StrideV() const override;

  int ConvertToARGB(Type type,
                    uint8_t* dst_argb,
                    int dst_stride_argb,
                    int dest_width,
                    int dest_height) override;

  rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer() { return buffer_; }

  // System monotonic clock, same timebase as rtc::TimeMicros().
  int64_t timestamp_us() const { return timestamp_us_; }
  void set_timestamp_us(int64_t timestamp_us) { timestamp_us_ = timestamp_us; }

  virtual RTCVideoFrame::VideoRotation rotation() override;

  webrtc::VideoRotation rotation() const { return rotation_; }

  void set_rotation(webrtc::VideoRotation rotation) { rotation_ = rotation; }

 private:
  rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer_;
  int64_t timestamp_us_ = 0;
  webrtc::VideoRotation rotation_ = webrtc::kVideoRotation_0;
};

};  // namespace libwebrtc

#endif  // LIB_WEBRTC_VIDEO_FRAME_IMPL_HXX
