/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "src/internal/video_capturer.h"

#include <algorithm>
#include <opencv2/opencv.hpp>

#include "api/scoped_refptr.h"
#include "api/video/i420_buffer.h"
#include "api/video/video_frame_buffer.h"
#include "api/video/video_rotation.h"

namespace webrtc {
namespace internal {
VideoCapturer::VideoCapturer() = default;
VideoCapturer::~VideoCapturer() = default;

void VideoCapturer::OnFrame(const VideoFrame& frame) {
  int cropped_width = 0;
  int cropped_height = 0;
  int out_width = 0;
  int out_height = 0;

  if (!video_adapter_.AdaptFrameResolution(
          frame.width(), frame.height(), frame.timestamp_us() * 1000,
          &cropped_width, &cropped_height, &out_width, &out_height)) {
    // Drop frame in order to respect frame rate constraint.
    return;
  }

  if (out_height != frame.height() || out_width != frame.width()) {
    // Video adapter has requested a down-scale. Allocate a new buffer and
    // return scaled version.
    webrtc::scoped_refptr<I420Buffer> scaled_buffer =
        I420Buffer::Create(out_width, out_height);
    scaled_buffer->ScaleFrom(*frame.video_frame_buffer()->ToI420());
    broadcaster_.OnFrame(VideoFrame::Builder()
                             .set_video_frame_buffer(scaled_buffer)
                             .set_rotation(kVideoRotation_0)
                             .set_timestamp_us(frame.timestamp_us())
                             .set_id(frame.id())
                             .build());
  } else {
    // No adaptations needed, just return the frame as is.
    broadcaster_.OnFrame(frame);
  }
}

webrtc::VideoSinkWants VideoCapturer::GetSinkWants() {
  return broadcaster_.wants();
}

void VideoCapturer::AddOrUpdateSink(webrtc::VideoSinkInterface<VideoFrame>* sink,
                                    const webrtc::VideoSinkWants& wants) {
  broadcaster_.AddOrUpdateSink(sink, wants);
  UpdateVideoAdapter();
}

void VideoCapturer::RemoveSink(webrtc::VideoSinkInterface<VideoFrame>* sink) {
  broadcaster_.RemoveSink(sink);
  UpdateVideoAdapter();
}

void VideoCapturer::UpdateVideoAdapter() {
  webrtc::VideoSinkWants wants = broadcaster_.wants();

  if (0 < wants.resolutions.size()) {
    auto size = wants.resolutions.at(0);
    std::pair<int, int> target_aspect_ratiot(size.width, size.height);
    video_adapter_.OnOutputFormatRequest(
        target_aspect_ratiot, wants.max_pixel_count, wants.max_framerate_fps);
  } else {
    video_adapter_.OnSinkWants(wants);
  }
}

}  // namespace internal
}  // namespace webrtc
// Function to convert I420Buffer to cv::Mat
cv::Mat I420BufferToMat(const rtc::scoped_refptr<webrtc::I420Buffer>& i420_buffer) {
    int width = i420_buffer->width();
    int height = i420_buffer->height();
    
    // Create a single buffer to hold the YUV420 data
    size_t y_size = width * height;
    size_t uv_size = (width / 2) * (height / 2);
    size_t buffer_size = y_size + 2 * uv_size;
    std::vector<uint8_t> yuv420_data(buffer_size);

    // Copy the data from the I420Buffer to the single buffer
    uint8_t* y_plane = yuv420_data.data();
    uint8_t* u_plane = y_plane + y_size;
    uint8_t* v_plane = u_plane + uv_size;

    memcpy(y_plane, i420_buffer->DataY(), y_size);
    memcpy(u_plane, i420_buffer->DataU(), uv_size);
    memcpy(v_plane, i420_buffer->DataV(), uv_size);

    // Create a cv::Mat with the YUV420 data
    cv::Mat yuv420_image(height + height / 2, width, CV_8UC1, yuv420_data.data());

    // Convert YUV420 image to BGR
    cv::Mat bgr_image;
    cv::cvtColor(yuv420_image, bgr_image, cv::COLOR_YUV2BGR_I420);

    return bgr_image;
}