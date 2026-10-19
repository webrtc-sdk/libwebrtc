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

#include <algorithm>
#include <memory>

#include "api/sequence_checker.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "third_party/libyuv/include/libyuv.h"
#ifdef WEBRTC_WIN
#include "modules/desktop_capture/win/window_capture_utils.h"
#if defined(RTC_ENABLE_WIN_WGC)
#include <windows.foundation.metadata.h>
#include <windows.graphics.capture.h>
#include <windows.security.authorization.appcapabilityaccess.h>
#include <wrl/client.h>
#include <wrl/event.h>

#include <cwchar>

#include "modules/desktop_capture/win/wgc_capturer_win.h"
#include "rtc_base/win/get_activation_factory.h"
#include "rtc_base/win/hstring.h"
#endif
#endif

namespace libwebrtc {

enum { kCaptureDelay = 33, kCaptureMessageId = 1000 };

namespace {

RTCDesktopCapturerOptions OptionsWithCursor(bool show_cursor) {
  RTCDesktopCapturerOptions options;
  options.show_cursor = show_cursor;
  return options;
}

#if defined(WEBRTC_WIN) && defined(RTC_ENABLE_WIN_WGC)
// Whether Windows lets this app capture without the WGC capture border: the
// system has GraphicsCaptureSession.IsBorderRequired (Windows 11) and
// GraphicsCaptureAccess grants borderless capture. Runs on a COM initialized
// thread.
bool CanCaptureWithoutBorder() {
  using Microsoft::WRL::ComPtr;
  namespace WGC = ABI::Windows::Graphics::Capture;
  namespace Access = ABI::Windows::Security::Authorization::AppCapabilityAccess;
  namespace WF = ABI::Windows::Foundation;

  if (!webrtc::ResolveCoreWinRTDelayload() ||
      !webrtc::ResolveCoreWinRTStringDelayload()) {
    return false;
  }

  ComPtr<WF::Metadata::IApiInformationStatics> api_info;
  HRESULT hr = webrtc::GetActivationFactory<
      WF::Metadata::IApiInformationStatics,
      RuntimeClass_Windows_Foundation_Metadata_ApiInformation>(&api_info);
  if (FAILED(hr)) {
    return false;
  }
  static const wchar_t kSessionType[] =
      L"Windows.Graphics.Capture.GraphicsCaptureSession";
  static const wchar_t kBorderProperty[] = L"IsBorderRequired";
  HSTRING session_type = nullptr;
  HSTRING border_property = nullptr;
  boolean has_border_property = false;
  if (SUCCEEDED(webrtc::CreateHstring(kSessionType, wcslen(kSessionType),
                                      &session_type)) &&
      SUCCEEDED(webrtc::CreateHstring(kBorderProperty, wcslen(kBorderProperty),
                                      &border_property))) {
    if (FAILED(api_info->IsPropertyPresent(session_type, border_property,
                                           &has_border_property))) {
      has_border_property = false;
    }
  }
  if (session_type) {
    webrtc::DeleteHstring(session_type);
  }
  if (border_property) {
    webrtc::DeleteHstring(border_property);
  }
  if (!has_border_property) {
    return false;
  }

  ComPtr<WGC::IGraphicsCaptureAccessStatics> access;
  hr = webrtc::GetActivationFactory<
      WGC::IGraphicsCaptureAccessStatics,
      RuntimeClass_Windows_Graphics_Capture_GraphicsCaptureAccess>(&access);
  if (FAILED(hr)) {
    return false;
  }
  ComPtr<WF::IAsyncOperation<Access::AppCapabilityAccessStatus>> request;
  if (FAILED(access->RequestAccessAsync(
          WGC::GraphicsCaptureAccessKind_Borderless, &request))) {
    return false;
  }
  // Desktop apps get an answer without a prompt. The event is shared with
  // the completion handler, which may run after a timeout here.
  auto done = std::make_shared<Microsoft::WRL::Wrappers::Event>(
      CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS));
  if (!done->IsValid()) {
    return false;
  }
  auto handler = Microsoft::WRL::Callback<
      WF::IAsyncOperationCompletedHandler<Access::AppCapabilityAccessStatus>>(
      [done](WF::IAsyncOperation<Access::AppCapabilityAccessStatus>*,
             WF::AsyncStatus) {
        SetEvent(done->Get());
        return S_OK;
      });
  if (!handler || FAILED(request->put_Completed(handler.Get())) ||
      WaitForSingleObject(done->Get(), 5000) != WAIT_OBJECT_0) {
    return false;
  }
  Access::AppCapabilityAccessStatus status;
  if (FAILED(request->GetResults(&status))) {
    return false;
  }
  return status == Access::AppCapabilityAccessStatus_Allowed;
}
#endif

}  // namespace

RTCDesktopCapturerImpl::RTCDesktopCapturerImpl(
    DesktopType type, webrtc::DesktopCapturer::SourceId source_id,
    webrtc::Thread* signaling_thread, scoped_refptr<MediaSource> source,
    bool showCursor)
    : RTCDesktopCapturerImpl(type, source_id, signaling_thread, source,
                             OptionsWithCursor(showCursor)) {}

RTCDesktopCapturerImpl::RTCDesktopCapturerImpl(
    DesktopType type, webrtc::DesktopCapturer::SourceId source_id,
    webrtc::Thread* signaling_thread, scoped_refptr<MediaSource> source,
    const RTCDesktopCapturerOptions& options)
    : thread_(webrtc::Thread::Create()),
      source_id_(source_id),
      signaling_thread_(signaling_thread),
      source_(source),
      focus_window_(options.focus_window) {
  RTC_DCHECK(thread_);
  type_ = type;
  thread_->Start();
  options_ = webrtc::DesktopCaptureOptions::CreateDefault();
  options_.set_detect_updated_region(true);
#ifdef WEBRTC_WIN
  options_.set_allow_directx_capturer(true);
#if defined(RTC_ENABLE_WIN_WGC)
  if (type == kWindow && options.allow_wgc_window_capturer) {
    options_.set_allow_wgc_window_capturer(true);
    options_.set_wgc_require_border(options.wgc_border_required);
  }
#endif
#endif
#ifdef WEBRTC_LINUX
  if (type == kScreen) {
    options_.set_allow_pipewire(true);
  }
#endif
  const bool showCursor = options.show_cursor;
  const bool wants_borderless = !options.wgc_border_required;
  thread_->BlockingCall([this, type, showCursor, wants_borderless] {
#if defined(WEBRTC_WIN) && defined(RTC_ENABLE_WIN_WGC)
    if (options_.allow_wgc_window_capturer()) {
      // WGC creates WinRT objects and a DispatcherQueue on the thread it
      // captures on, which must already be COM initialized.
      com_initializer_ = std::make_unique<webrtc::ScopedCOMInitializer>(
          webrtc::ScopedCOMInitializer::kMTA);
      use_wgc_ = com_initializer_->Succeeded() &&
                 webrtc::IsWgcSupported(webrtc::CaptureType::kWindow);
      if (!use_wgc_) {
        options_.set_allow_wgc_window_capturer(false);
      }
      // Ask WGC to drop its border only when Windows allows it, so the
      // border state is known rather than a request that may silently fail.
      border_hidden_ =
          use_wgc_ && wants_borderless && CanCaptureWithoutBorder();
      options_.set_wgc_require_border(!border_hidden_);
      RTC_LOG(LS_INFO) << "RTCDesktopCapturerImpl: window capture with "
                       << (use_wgc_ ? "Windows.Graphics.Capture"
                                    : "the default capturer")
                       << (border_hidden_ ? ", without the capture border"
                                          : "");
    }
#else
    (void)wants_borderless;
#endif
    if (type == kScreen) {
      if (showCursor) {
        capturer_ = std::make_unique<webrtc::DesktopAndCursorComposer>(
            webrtc::DesktopCapturer::CreateScreenCapturer(options_), options_);
      } else {
        capturer_ = webrtc::DesktopAndCursorComposer::CreateWithoutMouseCursorMonitor(
                webrtc::DesktopCapturer::CreateScreenCapturer(options_));
      }
    } else {
      capturer_ = std::make_unique<webrtc::DesktopAndCursorComposer>(
          webrtc::DesktopCapturer::CreateWindowCapturer(options_), options_);
    }
  });
}

RTCDesktopCapturerImpl::~RTCDesktopCapturerImpl() {
#ifdef WEBRTC_WIN
  if (com_initializer_) {
    // Release the capturer's WinRT objects, then COM, on the thread that
    // created them.
    thread_->BlockingCall([this] {
      capture_state_ = CS_STOPPED;
      capturer_.reset();
      com_initializer_.reset();
    });
  }
#endif
  thread_->Stop();
  capturer_.reset();
}

RTCDesktopCapturerImpl::CaptureState RTCDesktopCapturerImpl::Start(
    uint32_t fps, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
  x_ = x;
  y_ = y;
  w_ = w;
  h_ = h;
  if (!w_ || !h) {
    x_ = 0;
    y_ = 0;
  }
  return Start(fps);
}

RTCDesktopCapturerImpl::CaptureState RTCDesktopCapturerImpl::Start(
    uint32_t fps) {
  if (capture_state_ == CS_RUNNING) {
    return capture_state_;
  }

  if (fps == 0) {
    capture_state_ = CS_FAILED;
    return capture_state_;
  }

  if (fps >= 60) {
    capture_delay_ = uint32_t(1000.0 / 60.0);
  } else {
    capture_delay_ = uint32_t(1000.0 / fps);
  }

  if (source_id_ != -1) {
    // WGC creates its capture item when the source is selected: do it on
    // the thread it captures on.
    const bool selected = use_wgc_ ? thread_->BlockingCall([this] {
      return capturer_->SelectSource(source_id_);
    })
                                   : capturer_->SelectSource(source_id_);
    if (!selected) {
      capture_state_ = CS_FAILED;
      return capture_state_;
    }
    if (type_ == kWindow && focus_window_) {
      if (!capturer_->FocusOnSelectedSource()) {
        capture_state_ = CS_FAILED;
        return capture_state_;
      }
    }
  }

  thread_->BlockingCall([this] { capturer_->Start(this); });
  capture_state_ = CS_RUNNING;
  thread_->PostTask([this] { CaptureFrame(); });
  if (observer_) {
    signaling_thread_->BlockingCall([&, this]() { observer_->OnStart(this); });
  }
  return capture_state_;
}

void RTCDesktopCapturerImpl::Stop() {
  if (observer_) {
    if (!signaling_thread_->IsCurrent()) {
      signaling_thread_->BlockingCall([&, this]() { observer_->OnStop(this); });
    } else {
      observer_->OnStop(this);
    }
  }
  capture_state_ = CS_STOPPED;
}

bool RTCDesktopCapturerImpl::IsRunning() {
  return capture_state_ == CS_RUNNING;
}

#ifdef WEBRTC_WIN
int filterException(int code, PEXCEPTION_POINTERS ex) {
  return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void RTCDesktopCapturerImpl::OnCaptureResult(
    webrtc::DesktopCapturer::Result result,
    std::unique_ptr<webrtc::DesktopFrame> frame) {
  if (result != result_) {
    if (result == webrtc::DesktopCapturer::Result::ERROR_PERMANENT) {
      if (observer_) {
        signaling_thread_->BlockingCall(
            [&, this]() { observer_->OnError(this); });
      }
      capture_state_ = CS_FAILED;
      return;
    }

    if (result == webrtc::DesktopCapturer::Result::ERROR_TEMPORARY) {
      result_ = result;
      if (observer_) {
        signaling_thread_->BlockingCall(
            [&, this]() { observer_->OnPaused(this); });
      }
      return;
    }

    if (result == webrtc::DesktopCapturer::Result::SUCCESS) {
      result_ = result;
      if (observer_) {
        signaling_thread_->BlockingCall(
            [&, this]() { observer_->OnStart(this); });
      }
    }
  }

  if (result == webrtc::DesktopCapturer::Result::ERROR_TEMPORARY) {
    return;
  }

  if (use_wgc_) {
    OnWgcFrame(*frame);
    return;
  }

  int width = frame->size().width();
  int height = frame->size().height();
#ifdef WEBRTC_WIN
  webrtc::DesktopRect rect_ = webrtc::DesktopRect::MakeWH(width, height);

  if (type_ != kScreen) {
    webrtc::GetWindowRect(reinterpret_cast<HWND>(source_id_), &rect_);
  }

  __try
#endif
  {
    width = w_ > 0 ? w_ : width;
    height = h_ > 0 ? h_ : height;
    if (!i420_buffer_ || !i420_buffer_.get() ||
        i420_buffer_->width() * i420_buffer_->height() != width * height) {
      i420_buffer_ = webrtc::I420Buffer::Create(width, height);
    }

    libyuv::ConvertToI420(frame->data(), 0, i420_buffer_->MutableDataY(),
                          i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
                          i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
                          i420_buffer_->StrideV(), x_, y_,
#ifdef WEBRTC_WIN
                          rect_.width(), rect_.height(),
#else
                          width, height,
#endif
                          width, height, libyuv::kRotate0, libyuv::FOURCC_ARGB);

    OnFrame(webrtc::VideoFrame(i420_buffer_, 0, webrtc::TimeMillis(),
                               webrtc::kVideoRotation_0));
  }
#ifdef WEBRTC_WIN
  __except (filterException(GetExceptionCode(), GetExceptionInformation())) {
  }
#endif
}

void RTCDesktopCapturerImpl::OnWgcFrame(const webrtc::DesktopFrame& frame) {
  // A WGC frame has the window's current size, which can differ from its
  // window rectangle, and its rows may be padded (stride > width * 4), so it
  // is converted as it is.
  const int frame_width = frame.size().width();
  const int frame_height = frame.size().height();
  const int x = std::min<int>(x_, frame_width);
  const int y = std::min<int>(y_, frame_height);
  const int width =
      w_ > 0 ? std::min<int>(w_, frame_width - x) : frame_width - x;
  const int height =
      h_ > 0 ? std::min<int>(h_, frame_height - y) : frame_height - y;
  if (width <= 0 || height <= 0) {
    return;
  }
  if (!i420_buffer_ || i420_buffer_->width() != width ||
      i420_buffer_->height() != height) {
    i420_buffer_ = webrtc::I420Buffer::Create(width, height);
  }
  const uint8_t* src = frame.data() + y * frame.stride() +
                       x * webrtc::DesktopFrame::kBytesPerPixel;
  libyuv::ARGBToI420(src, frame.stride(), i420_buffer_->MutableDataY(),
                     i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
                     i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
                     i420_buffer_->StrideV(), width, height);
  OnFrame(webrtc::VideoFrame(i420_buffer_, 0, webrtc::TimeMillis(),
                             webrtc::kVideoRotation_0));
}

void RTCDesktopCapturerImpl::CaptureFrame() {
  RTC_DCHECK_RUN_ON(thread_.get());
  if (capture_state_ == CS_RUNNING) {
    capturer_->CaptureFrame();
    thread_->PostDelayedHighPrecisionTask(
        [this]() { CaptureFrame(); },
        webrtc::TimeDelta::Millis(capture_delay_));
  }
}

}  // namespace libwebrtc
