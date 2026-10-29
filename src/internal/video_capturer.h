/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef INTERNAL_VIDEO_CAPTURER_H_
#define INTERNAL_VIDEO_CAPTURER_H_

#include <stddef.h>

#include <memory>
#include <vector>

#include "api/video/video_frame.h"
#include "api/video/video_source_interface.h"
#include "media/base/video_adapter.h"
#include "media/base/video_broadcaster.h"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"
#include "pc/video_track_source.h"
#include "rtc_base/synchronization/mutex.h"
#include "rtc_base/thread_annotations.h"
#include "rtc_video_processor.h"

namespace webrtc {
namespace internal {

class VideoCapturer : public webrtc::VideoSourceInterface<VideoFrame> {
 public:
  VideoCapturer();
  virtual ~VideoCapturer();

  virtual bool StartCapture() { return false; }

  virtual bool CaptureStarted() { return false; }

  virtual void StopCapture() {}

  void AddOrUpdateSink(webrtc::VideoSinkInterface<VideoFrame>* sink,
                       const webrtc::VideoSinkWants& wants) override;
  void RemoveSink(webrtc::VideoSinkInterface<VideoFrame>* sink) override;

  // Pushes a captured frame through the adaptation/broadcast pipeline.
  // Made public so RTCVideoSourceImpl can forward user-supplied frames into
  // the same path that camera/desktop subclasses feed.
  void OnFrame(const VideoFrame& frame);

  void AddProcessor(libwebrtc::RTCVideoProcessor* processor);
  void RemoveProcessor(libwebrtc::RTCVideoProcessor* processor);

 protected:
  webrtc::VideoSinkWants GetSinkWants();

 private:
  void UpdateVideoAdapter();
  void DeliverFrame(VideoFrame frame);
  // Returns false when a processor dropped the frame.
  bool ApplyProcessors(VideoFrame& frame);

  webrtc::VideoBroadcaster broadcaster_;
  webrtc::VideoAdapter video_adapter_;
  webrtc::Mutex processors_lock_;
  std::vector<libwebrtc::RTCVideoProcessor*> processors_
      RTC_GUARDED_BY(processors_lock_);
};
}  // namespace internal
}  // namespace webrtc

#endif  // INTERNAL_VIDEO_CAPTURER_H_
