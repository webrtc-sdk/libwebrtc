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

#include "rtc_desktop_capturer_impl.h"

#include "third_party/libyuv/include/libyuv.h"

namespace libwebrtc {

enum { kCaptureDelay = 33, kCaptureMessageId = 1000 };

RTCDesktopCapturerImpl::RTCDesktopCapturerImpl(
    DesktopType type,webrtc::DesktopCapturer::SourceId source_id, DesktopCapturerObserver *observer)
    : thread_(rtc::Thread::Create()), source_id_(source_id),observer_(observer) {
  RTC_DCHECK(observer_);
  options_ = webrtc::DesktopCaptureOptions::CreateDefault();
  options_.set_detect_updated_region(true);
  if (type == kScreen) {
    capturer_ = std::make_unique<webrtc::DesktopAndCursorComposer>(webrtc::DesktopCapturer::CreateScreenCapturer(options_), options_);
  }
  else { capturer_ = std::make_unique<webrtc::DesktopAndCursorComposer>(webrtc::DesktopCapturer::CreateWindowCapturer(options_), options_); }
  type_ = type;
  thread_->Start();
}

RTCDesktopCapturerImpl::~RTCDesktopCapturerImpl() {
  thread_->Stop();
}

RTCDesktopCapturerImpl::CaptureState RTCDesktopCapturerImpl::Start(
    uint32_t fps) {
if(fps == 0) {
      capture_state_ = CS_FAILED;
      return capture_state_;
  }

  if(fps >= 60) {
    capture_delay_ = uint32_t(1000.0 / 60.0);
  } else {
    capture_delay_ = uint32_t(1000.0 / fps);
  }

  if(source_id_ != -1) {
    if(!capturer_->SelectSource(source_id_)) {
        capture_state_ = CS_FAILED;
        return capture_state_;
    }
    if(type_ == kWindow) {
      if(!capturer_->FocusOnSelectedSource()) {
        capture_state_ = CS_FAILED;
        return capture_state_;
      }
    }
  }

  capturer_->Start(this);
  capture_state_ = CS_RUNNING;
  CaptureFrame();
  observer_->OnStart(this);
  return capture_state_;
}

void RTCDesktopCapturerImpl::Stop() {
  observer_->OnStop(this);
  capture_state_ = CS_STOPPED;
}

bool RTCDesktopCapturerImpl::IsRunning() {
  return capture_state_ == CS_RUNNING;
}

void RTCDesktopCapturerImpl::OnCaptureResult(
    webrtc::DesktopCapturer::Result result,
                                        std::unique_ptr<webrtc::DesktopFrame> frame) {
  if (result != result_) {
    if (result == webrtc::DesktopCapturer::Result::ERROR_PERMANENT) {
      observer_->OnError(this);
      capture_state_ = CS_FAILED;
      return;
    }

    if (result == webrtc::DesktopCapturer::Result::ERROR_TEMPORARY) {
      result_ = result;
      observer_->OnPaused(this);
      return;
    }

    if (result == webrtc::DesktopCapturer::Result::SUCCESS) {
      result_ = result;
      observer_->OnStart(this);
    }
  }

  if (result == webrtc::DesktopCapturer::Result::ERROR_TEMPORARY) {
      return;
  }
  
  int width = frame->size().width();
  int height = frame->size().height();
  int real_width = width;

#if defined(TARGET_OS_OSX)
  if(type_ == kWindow) {
    int multiple = 0;
#if defined(WEBRTC_ARCH_X86_FAMILY)
    multiple = 16;
#elif defined(WEBRTC_ARCH_ARM64)
    multiple = 32;
#endif
    // A multiple of $multiple must be used as the width of the src frame,
    // and the right black border needs to be cropped during conversion.
    if( multiple != 0 && (width % multiple) != 0 ) {
      width = (width / multiple + 1) * multiple;
    }
  }
 #endif
 
  if (!i420_buffer_ || !i420_buffer_.get() ||
      i420_buffer_->width() * i420_buffer_->height() != real_width * height) {
    i420_buffer_ = webrtc::I420Buffer::Create(real_width, height);
  }

  libyuv::ConvertToI420(frame->data(),
                        0,
                        i420_buffer_->MutableDataY(),
                        i420_buffer_->StrideY(),
                        i420_buffer_->MutableDataU(),
                        i420_buffer_->StrideU(),
                        i420_buffer_->MutableDataV(),
                        i420_buffer_->StrideV(),
                        0,
                        0,
                        width,
                        height,
                        real_width,
                        height,
                        libyuv::kRotate0,
                        libyuv::FOURCC_ARGB);

  OnFrame(webrtc::VideoFrame(i420_buffer_, 0, 0, webrtc::kVideoRotation_0));
}

void RTCDesktopCapturerImpl::OnMessage(rtc::Message* msg) {
  if (msg->message_id == kCaptureMessageId) {
    CaptureFrame();
  }
}

void RTCDesktopCapturerImpl::CaptureFrame() {
  if (capture_state_ == CS_RUNNING) {
    capturer_->CaptureFrame();
    thread_->PostDelayed(RTC_FROM_HERE, capture_delay_, this, kCaptureMessageId);
  }
}

}  // namespace webrtc
