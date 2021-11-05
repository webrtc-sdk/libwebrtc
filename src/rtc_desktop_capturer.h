#ifndef LIB_WEBRTC_DESKTOP_CAPTURER_IMPL_HXX
#define LIB_WEBRTC_DESKTOP_CAPTURER_IMPL_HXX

#include "modules/desktop_capture/desktop_capture_options.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"

#include "api/scoped_refptr.h"
#include "api/video/i420_buffer.h"
#include "api/video/video_frame.h"
#include "api/video/video_source_interface.h"
#include "media/base/video_adapter.h"
#include "media/base/video_broadcaster.h"
#include "pc/video_track_source.h"
#include "rtc_base/thread.h"
#include "rtc_desktop_device.h"
#include "src/internal/vcm_capturer.h"
#include "src/internal/video_capturer.h"
#include "third_party/libyuv/include/libyuv.h"

// std::unique_ptr<cricket::VideoCapturer> video_device =
// std::unique_ptr<cricket::VideoCapturer>(new DesktopCapturerDeviceImpl());

namespace libwebrtc {

enum CaptureState { CS_RUNNING, CS_STOPPED };

class RTCDesktopCapturer : public webrtc::internal::VideoCapturer,
                           public rtc::MessageHandler,
                           public webrtc::DesktopCapturer::Callback {
 public:
  RTCDesktopCapturer(
      rtc::Thread* signaling_thread,std::unique_ptr<webrtc::DesktopCapturer>
          desktopcapturer);
  ~RTCDesktopCapturer();
  void CaptureFrame();
  virtual CaptureState Start(
      const cricket::VideoFormat& capture_format);
  virtual void Stop();
  virtual bool IsRunning();
  virtual void OnCaptureResult(
      webrtc::DesktopCapturer::Result result,
      std::unique_ptr<webrtc::DesktopFrame> frame) override;
  virtual void OnMessage(rtc::Message* msg) override;
 private:
  std::unique_ptr<webrtc::DesktopCapturer> capturer;
  rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_;
  CaptureState capture_state_ = CS_STOPPED;
  rtc::Thread* signaling_thread_ = nullptr;
};

}  // namespace libwebrtc
#endif