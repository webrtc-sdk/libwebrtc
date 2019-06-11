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

#include "api/video/video_frame.h"
#include "api/video/video_source_interface.h"
#include "media/base/video_adapter.h"
#include "media/base/video_broadcaster.h"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"
#include "pc/video_track_source.h"

namespace webrtc {
namespace internal {

class VideoCapturer : public rtc::VideoSourceInterface<VideoFrame> {
 public:
  VideoCapturer();
  virtual ~VideoCapturer();

  void AddOrUpdateSink(rtc::VideoSinkInterface<VideoFrame>* sink,
                       const rtc::VideoSinkWants& wants) override;
  void RemoveSink(rtc::VideoSinkInterface<VideoFrame>* sink) override;

 protected:
  void OnFrame(const VideoFrame& frame);
  rtc::VideoSinkWants GetSinkWants();

 private:
  void UpdateVideoAdapter();

  rtc::VideoBroadcaster broadcaster_;
  cricket::VideoAdapter video_adapter_;
};
}  // namespace internal
}  // namespace webrtc

#endif  // INTERNAL_VIDEO_CAPTURER_H_
