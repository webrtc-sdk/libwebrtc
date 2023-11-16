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

#ifndef LIBWEBRTC_RTC_DESKTOP_MEDIA_LIST_IMPL_HXX
#define LIBWEBRTC_RTC_DESKTOP_MEDIA_LIST_IMPL_HXX

#include "api/video/i420_buffer.h"
#include "api/video/video_frame.h"
#include "modules/desktop_capture/desktop_capture_options.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "rtc_base/thread.h"
#include "rtc_desktop_capturer_impl.h"
#include "rtc_desktop_media_list.h"

namespace libwebrtc {

class RTCDesktopMediaListImpl;

class MediaSourceImpl : public MediaSource {
 public:
  MediaSourceImpl(RTCDesktopMediaListImpl* mediaList,
                  webrtc::DesktopCapturer::Source src, DesktopType type)
      : source(src), mediaList_(mediaList), type_(type) {}
  virtual ~MediaSourceImpl() {}

  webrtc::DesktopCapturer::Source source;

  virtual string id() const override {
    return string(std::to_string(source.id));
  }

  // source id
  webrtc::DesktopCapturer::SourceId source_id() const { return source.id; }

  // source name
  string name() const override { return string(source.title); }

  // Returns the thumbnail of the source, jpeg format.
  portable::vector<unsigned char> thumbnail() const override {
    return thumbnail_;
  }

  DesktopType type() const override { return type_; }

  bool UpdateThumbnail() override;

  void SaveCaptureResult(webrtc::DesktopCapturer::Result result,
                         std::unique_ptr<webrtc::DesktopFrame> frame);

 private:
  std::vector<unsigned char> thumbnail_;
  rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_;
  RTCDesktopMediaListImpl* mediaList_;
  DesktopType type_;
};

class RTCDesktopMediaListImpl : public RTCDesktopMediaList {
 public:
  enum CaptureState { CS_RUNNING, CS_STOPPED, CS_FAILED };

 public:
  RTCDesktopMediaListImpl(DesktopType type, rtc::Thread* signaling_thread);

  virtual ~RTCDesktopMediaListImpl();

  void RegisterMediaListObserver(MediaListObserver* observer) override {
    observer_ = observer;
  }

  void DeRegisterMediaListObserver() override { observer_ = nullptr; }

  DesktopType type() const override { return type_; }

  int32_t UpdateSourceList(bool force_reload = false,
                           bool get_thumbnail = true) override;

  int GetSourceCount() const override;

  scoped_refptr<MediaSource> GetSource(int index) override;

  bool GetThumbnail(scoped_refptr<MediaSource> source,
                    bool notify = false) override;

 private:
  class CallbackProxy : public webrtc::DesktopCapturer::Callback {
   public:
    CallbackProxy() {}
    void SetCallback(
        std::function<void(webrtc::DesktopCapturer::Result result,
                           std::unique_ptr<webrtc::DesktopFrame> frame)>
            on_capture_result) {
      on_capture_result_ = on_capture_result;
    }

   private:
    void OnCaptureResult(webrtc::DesktopCapturer::Result result,
                         std::unique_ptr<webrtc::DesktopFrame> frame) override {
      if (on_capture_result_) on_capture_result_(result, std::move(frame));
    }
    std::function<void(webrtc::DesktopCapturer::Result result,
                       std::unique_ptr<webrtc::DesktopFrame> frame)>
        on_capture_result_ = nullptr;
  };

 private:
  std::unique_ptr<CallbackProxy> callback_;
  webrtc::DesktopCaptureOptions options_;
  std::unique_ptr<webrtc::DesktopCapturer> capturer_;
  std::unique_ptr<rtc::Thread> thread_;
  std::vector<scoped_refptr<MediaSourceImpl>> sources_;
  MediaListObserver* observer_ = nullptr;
  DesktopType type_;
  rtc::Thread* signaling_thread_ = nullptr;
};

}  // namespace libwebrtc

#endif  // LIBWEBRTC_RTC_DESKTOP_MEDIA_LIST_IMPL_HXX
