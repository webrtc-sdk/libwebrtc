#ifndef LIB_WEBRTC_RTC_DUMMY_VIDEO_CAPTURER_IMPL_HXX
#define LIB_WEBRTC_RTC_DUMMY_VIDEO_CAPTURER_IMPL_HXX

#include "rtc_types.h"
#include "rtc_video_frame.h"
#include "rtc_dummy_video_capturer.h"
#include "rtc_base/thread.h"
#include "api/video/i420_buffer.h"
#include "src/internal/video_capturer.h"

namespace libwebrtc {

/**
 * class DummyVideoCapturerObserverImpl
 */
class DummyVideoCapturerObserverImpl : public DummyVideoCapturerObserver
{
 public:
   DummyVideoCapturerObserverImpl(void* callbacks /* rtcVideoCapturerObserverCallbacks* */);
   ~DummyVideoCapturerObserverImpl();

  void OnStart() override;
  void OnPaused() override;
  void OnStop() override;
  void OnError() override;
  void OnFillBuffer(scoped_refptr<RTCVideoFrame> frame) override;

 private:
   void* callbacks_ /* rtcVideoCapturerObserverCallbacks* */;
}; // end class DummyVideoCapturerObserverImpl

/**
 * class RTCDummyVideoCapturerImpl
 */
class RTCDummyVideoCapturerImpl : public RTCDummyVideoCapturer,
                                  public webrtc::internal::VideoCapturer {
public:
  RTCDummyVideoCapturerImpl(rtc::Thread* signaling_thread,
                            uint32_t fps, 
                            uint32_t width,
                            uint32_t height);
  ~RTCDummyVideoCapturerImpl();

  void RegisterObserver(
      DummyVideoCapturerObserver* observer) override {
    observer_ = observer;
  }

  void DeRegisterObserver() override {
    observer_ = nullptr;
  }

  DummyVideoCapturerObserver* GetObserver() override { return observer_; }

  RTCCaptureState Start() override;

  RTCCaptureState Start(uint32_t fps) override;

  RTCCaptureState Start(uint32_t fps, 
                        uint32_t width,
                        uint32_t height) override;
  void Stop() override;

  bool IsRunning() const override;

  RTCCaptureState state() const override { return capture_state_; }

private:
  void OnFrameReady(const scoped_refptr<RTCVideoFrame>& frame);
  void FillBuffer();
  void CaptureFrame();

private:
  DummyVideoCapturerObserver* observer_ = nullptr;
  std::unique_ptr<rtc::Thread> thread_;
  rtc::Thread* signaling_thread_ = nullptr;
  rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_;
  RTCCaptureState capture_state_ = RTCCaptureState::CS_STOPPED;
  uint32_t capture_delay_ = 1000;  // 1s
  uint32_t fps_ = 25;
  uint32_t width_ = 960;
  uint32_t height_ = 540;
  scoped_refptr<RTCVideoFrame> frame_buffer_;
}; // end class RTCDummyVideoCapturerImpl

} // namespace libwebrtc

#endif // LIB_WEBRTC_RTC_DUMMY_VIDEO_CAPTURER_IMPL_HXX
