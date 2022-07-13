/*
 * Copyright 2022 LiveKit
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LIBWEBRTC_RTC_DESKTOP_CAPTURER_IMPL_HXX
#define LIBWEBRTC_RTC_DESKTOP_CAPTURER_IMPL_HXX

#include "api/video/i420_buffer.h"
#include "api/video/video_frame.h"
#include "modules/desktop_capture/desktop_and_cursor_composer.h"
#include "modules/desktop_capture/desktop_capture_options.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "rtc_base/thread.h"
#include "src/internal/vcm_capturer.h"
#include "src/internal/video_capturer.h"

namespace libwebrtc {

enum DesktopType { kScreen, kWindow };

class RTCDesktopCapturerImpl;

class DesktopCapturerObserver {
public:
  virtual void OnStart(RTCDesktopCapturerImpl *capturer) = 0;
  virtual void OnPaused(RTCDesktopCapturerImpl *capturer) = 0;
  virtual void OnStop(RTCDesktopCapturerImpl *capturer) = 0;
  virtual void OnError(RTCDesktopCapturerImpl *capturer) = 0;

protected:
   ~DesktopCapturerObserver() {}
};

class RTCDesktopCapturerImpl : public webrtc::DesktopCapturer::Callback,
                               public rtc::MessageHandler,
                               public webrtc::internal::VideoCapturer {
 public:
  enum CaptureState { CS_RUNNING, CS_STOPPED, CS_FAILED};

 public:
  RTCDesktopCapturerImpl(DesktopType type, webrtc::DesktopCapturer::SourceId source_id, DesktopCapturerObserver *observer);
  ~RTCDesktopCapturerImpl();

  virtual CaptureState Start(uint32_t fps);

  virtual void Stop();

  virtual bool IsRunning();

 protected:
  virtual void OnCaptureResult(webrtc::DesktopCapturer::Result result,
                               std::unique_ptr<webrtc::DesktopFrame> frame) override;
  virtual void OnMessage(rtc::Message* msg) override;

 private:
  void CaptureFrame();
  webrtc::DesktopCaptureOptions options_;
  std::unique_ptr<webrtc::DesktopCapturer> capturer_;
  std::unique_ptr<rtc::Thread> thread_;
  rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_;
  CaptureState capture_state_ = CS_STOPPED;
  DesktopType type_;
  webrtc::DesktopCapturer::SourceId source_id_;
  DesktopCapturerObserver *observer_;
  uint32_t capture_delay_ = 1000; // 1s
  webrtc::DesktopCapturer::Result result_ = webrtc::DesktopCapturer::Result::SUCCESS;
};

}  // namespace webrtc

#endif  // LIBWEBRTC_RTC_DESKTOP_CAPTURER_IMPL_HXX
