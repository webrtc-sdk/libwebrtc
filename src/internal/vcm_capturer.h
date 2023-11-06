/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef INTERNAL_VCM_CAPTURER_H_
#define INTERNAL_VCM_CAPTURER_H_

#include <memory>
#include <vector>

#include "api/scoped_refptr.h"
#include "modules/video_capture/video_capture.h"
#include "rtc_base/thread.h"
#include "src/internal/video_capturer.h"

namespace webrtc {
namespace internal {

class VcmCapturer : public VideoCapturer,
                    public rtc::VideoSinkInterface<VideoFrame> {
 public:
  static std::shared_ptr<VcmCapturer> Create(rtc::Thread* worker_thread,
                                             size_t width, size_t height,
                                             size_t target_fps,
                                             size_t capture_device_index);
  VcmCapturer(rtc::Thread* worker_thread);

  virtual ~VcmCapturer();

  bool StartCapture() override;

  bool CaptureStarted() override;

  void StopCapture() override;

  void OnFrame(const VideoFrame& frame) override;

 private:
  bool Init(size_t width, size_t height, size_t target_fps,
            size_t capture_device_index);
  void Destroy();

  rtc::scoped_refptr<VideoCaptureModule> vcm_;
  rtc::Thread* worker_thread_ = nullptr;
  VideoCaptureCapability capability_;
};

class CapturerTrackSource : public webrtc::VideoTrackSource {
 public:
  static rtc::scoped_refptr<CapturerTrackSource> Create(
      rtc::Thread* worker_thread);

 public:
  explicit CapturerTrackSource(std::shared_ptr<VideoCapturer> capturer)
      : VideoTrackSource(/*remote=*/false), capturer_(capturer) {}

 private:
  rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
    return capturer_.get();
  }
  std::shared_ptr<VideoCapturer> capturer_;
};
}  // namespace internal
}  // namespace webrtc

#endif  // INTERNAL_VCM_CAPTURER_H_
