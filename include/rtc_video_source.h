#ifndef LIB_WEBRTC_RTC_VIDEO_SOURCE_HXX
#define LIB_WEBRTC_RTC_VIDEO_SOURCE_HXX

#include "rtc_types.h"
#include "rtc_video_frame.h"
#include "rtc_video_processor.h"

namespace libwebrtc {

class RTCVideoSource : public RefCountInterface {
 public:
  enum class SourceType { kPlatformDevice, kCustom };

  // Push a user-supplied frame into the source, bypassing any underlying
  // capture device. The frame travels through the same adaptation and
  // broadcast pipeline that camera/desktop sources use, so it is delivered to
  // every sink (RTP encoder, local renderer, etc.) that has subscribed to
  // this source.
  //
  // Sources that are not backed by an injectable capturer (e.g. the desktop
  // capture path on some platforms) should treat this as a no-op.
  virtual void OnCapturedFrame(scoped_refptr<RTCVideoFrame> frame)  = 0;

  virtual SourceType GetSourceType() const = 0;

  // Runs `processor` on every frame of this source before the frame is
  // encoded or rendered. Processors run in the order they were added. The
  // caller owns `processor` and must keep it alive until RemoveProcessor()
  // returns. Once RemoveProcessor() returns, `processor` is not called again.
  // Do not call these from inside RTCVideoProcessor::Process().
  virtual void AddProcessor(RTCVideoProcessor* processor) = 0;

  virtual void RemoveProcessor(RTCVideoProcessor* processor) = 0;

  ~RTCVideoSource() {}
};
}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_VIDEO_SOURCE_HXX
