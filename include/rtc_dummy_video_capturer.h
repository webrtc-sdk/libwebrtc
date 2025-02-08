/**
 * File provided for Reference Use Only by isoft (c) 2025.
 * Copyright (c) isoft. All rights reserved.
 * 
 */

#ifndef LIB_WEBRTC_RTC_DUMMY_VIDEO_CAPTURER_HXX
#define LIB_WEBRTC_RTC_DUMMY_VIDEO_CAPTURER_HXX

#include "rtc_types.h"
#include "rtc_video_device.h"
#include "rtc_video_frame.h"

namespace libwebrtc {

/**
 * class DummyVideoCapturerObserver
 */
class DummyVideoCapturerObserver {
 public:
  virtual void OnStart() = 0;

  virtual void OnPaused() = 0;

  virtual void OnStop() = 0;

  virtual void OnError() = 0;

  virtual void OnFillBuffer(scoped_refptr<RTCVideoFrame> frame) = 0;

 protected:
  virtual ~DummyVideoCapturerObserver() {}
}; // class DummyVideoCapturerObserver

/**
 * class RTCDummyVideoCapturer
 */
class RTCDummyVideoCapturer : public RefCountInterface {
 public:
  virtual void RegisterObserver(
      DummyVideoCapturerObserver* observer) = 0;

  virtual void DeRegisterObserver() = 0;

  virtual RTCCaptureState Start() = 0;

  virtual RTCCaptureState Start(uint32_t fps) = 0;

  virtual RTCCaptureState Start(uint32_t fps, 
                                uint32_t width,
                                uint32_t height) = 0;
  virtual void Stop() = 0;

  virtual bool IsRunning() const = 0;

  virtual RTCCaptureState state() const = 0;

  virtual DummyVideoCapturerObserver* GetObserver() = 0;

  virtual ~RTCDummyVideoCapturer() {}
}; // class RTCDummyVideoCapturer

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_DUMMY_VIDEO_CAPTURER_HXX
