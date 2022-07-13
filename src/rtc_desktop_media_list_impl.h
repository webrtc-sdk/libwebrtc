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

namespace libwebrtc {

class RTCDesktopMediaListImpl;
class MediaSource;

 class MediaListObserver {
    public:
        virtual void OnMediaSourceAdded(MediaSource *source) = 0;
        virtual void OnMediaSourceRemoved(MediaSource *source) = 0;
        virtual void OnMediaSourceNameChanged(MediaSource *source) = 0;
        virtual void OnMediaSourceThumbnailChanged(MediaSource *source) = 0;

    protected:
        virtual ~MediaListObserver() {}
};

class MediaSource {
 public:
  MediaSource( RTCDesktopMediaListImpl *mediaList, webrtc::DesktopCapturer::Source src, DesktopType type)
      : source(src), mediaList_(mediaList), type_(type) {}
  virtual ~MediaSource() {}

  webrtc::DesktopCapturer::Source source;

  // source id
  webrtc::DesktopCapturer::SourceId id() const { return source.id; }

  // source name
  std::string name() const { return source.title; }

  // Returns the thumbnail of the source, jpeg format.
  std::vector<unsigned char> thumbnail() const { return thumbnail_; }

  DesktopType type() const { return type_; }

  bool UpdateThumbnail();

  void SaveCaptureResult(webrtc::DesktopCapturer::Result result,
                         std::unique_ptr<webrtc::DesktopFrame> frame);

 private:
  std::vector<unsigned char> thumbnail_;
  RTCDesktopMediaListImpl *mediaList_;
  DesktopType type_;
};

class RTCDesktopMediaListImpl : public rtc::MessageHandler {
public:
  enum CaptureState { CS_RUNNING, CS_STOPPED, CS_FAILED};
 public:
  RTCDesktopMediaListImpl(DesktopType type, MediaListObserver* observer);

  virtual ~RTCDesktopMediaListImpl();

  virtual int32_t UpdateSourceList(bool force_reload = false, bool get_thumbnail = true);

  virtual int GetSourceCount() const;
  
  virtual MediaSource* GetSource(int index);

  virtual bool GetThumbnail(MediaSource *source);

 protected:
  virtual void OnMessage(rtc::Message* msg) override;

 private:
    class CallbackProxy : public webrtc::DesktopCapturer::Callback {
        public:
         CallbackProxy(){}
          void SetCallback(std::function<void(webrtc::DesktopCapturer::Result result,
                               std::unique_ptr<webrtc::DesktopFrame> frame)> on_capture_result) {
                                on_capture_result_ = on_capture_result;
                               }
        private:
         void OnCaptureResult(webrtc::DesktopCapturer::Result result,
                               std::unique_ptr<webrtc::DesktopFrame> frame) override {
                                    if(on_capture_result_) on_capture_result_(result, std::move(frame));
                               }
        std::function<void(webrtc::DesktopCapturer::Result result,
                               std::unique_ptr<webrtc::DesktopFrame> frame)> on_capture_result_ = nullptr;
    };
 private:
  std::unique_ptr<CallbackProxy> callback_;
  webrtc::DesktopCaptureOptions options_;
  std::unique_ptr<webrtc::DesktopCapturer> capturer_;
  std::unique_ptr<rtc::Thread> thread_;
  std::vector<std::shared_ptr<MediaSource>> sources_;
  MediaListObserver* observer_;
  DesktopType type_;
};

}  // namespace libwebrtc

#endif  // LIBWEBRTC_RTC_DESKTOP_MEDIA_LIST_IMPL_HXX
