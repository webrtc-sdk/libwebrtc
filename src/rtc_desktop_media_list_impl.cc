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
 
#include "rtc_desktop_media_list_impl.h"
#include "rtc_base/checks.h"
#include "third_party/libyuv/include/libyuv.h"
#ifdef WEBRTC_WIN
#include "modules/desktop_capture/win/window_capture_utils.h"
#endif
extern "C" {
#if defined(USE_SYSTEM_LIBJPEG)
#include <jpeglib.h>
#else
// Include directory supplied by gn
#include "jpeglib.h"  // NOLINT
#endif
}

#include <iostream>
#include <fstream>

namespace libwebrtc {

RTCDesktopMediaListImpl::RTCDesktopMediaListImpl(DesktopType type, rtc::Thread* signaling_thread)
    :thread_(rtc::Thread::Create()),type_(type),signaling_thread_(signaling_thread) {
  RTC_DCHECK(thread_);
  thread_->Start();
  options_ = webrtc::DesktopCaptureOptions::CreateDefault();
  options_.set_detect_updated_region(true);
#ifdef WEBRTC_WIN
  options_.set_allow_directx_capturer(true);
#endif
#ifdef WEBRTC_LINUX
  if (type == kScreen) {
    options_.set_allow_pipewire(true);
  }
#endif
  callback_ = std::make_unique<CallbackProxy>();
  thread_->Invoke<void>(RTC_FROM_HERE, [this, type] {
    if (type == kScreen) {
      capturer_ = webrtc::DesktopCapturer::CreateScreenCapturer(options_);
    } else { 
      capturer_ = webrtc::DesktopCapturer::CreateWindowCapturer(options_); 
    }
    capturer_->Start(callback_.get());
  });
}

RTCDesktopMediaListImpl::~RTCDesktopMediaListImpl() {
  thread_->Stop();
}

int32_t RTCDesktopMediaListImpl::UpdateSourceList(bool force_reload, bool get_thumbnail) {

  if(force_reload) {
    for( auto source : sources_) {
      if(observer_) {
        auto source_ptr = source.get();
        signaling_thread_->Invoke<void>(
          RTC_FROM_HERE,[&, source_ptr]() {
            observer_->OnMediaSourceRemoved(source_ptr);
          });
      }
    }
    sources_.clear();
  }

  webrtc::DesktopCapturer::SourceList new_sources;
  thread_->Invoke<void>(RTC_FROM_HERE, [this,&new_sources] {
    capturer_->GetSourceList(&new_sources);
  });

  typedef std::set<webrtc::DesktopCapturer::SourceId> SourceSet;
  SourceSet new_source_set;
  for (size_t i = 0; i < new_sources.size(); ++i) {
    if(type_ == kScreen && new_sources[i].title.length() == 0) {
        new_sources[i].title = std::string("Screen " + std::to_string(i+1));
    }
    new_source_set.insert(new_sources[i].id);
  }
  // Iterate through the old sources to find the removed sources.
  for (size_t i = 0; i < sources_.size(); ++i) {
    if (new_source_set.find(sources_[i]->source_id()) == new_source_set.end()) {
      if(observer_) {
        auto source = (*(sources_.begin() + i)).get();
        signaling_thread_->Invoke<void>(
          RTC_FROM_HERE,[&, source]() {
            observer_->OnMediaSourceRemoved(source);
          });
      }
      sources_.erase(sources_.begin() + i);
      --i;
    }
  }
  // Iterate through the new sources to find the added sources.
  if (new_sources.size() > sources_.size()) {
    SourceSet old_source_set;
    for (size_t i = 0; i < sources_.size(); ++i) {
      old_source_set.insert(sources_[i]->source_id());
    }
    for (size_t i = 0; i < new_sources.size(); ++i) {
      if (old_source_set.find(new_sources[i].id) == old_source_set.end()) {
        auto source =
            new RefCountedObject<MediaSourceImpl>(this, new_sources[i], type_);
        sources_.insert(sources_.begin() + i, source);
        GetThumbnail(source, true);
        if(observer_) {
          signaling_thread_->Invoke<void>(
            RTC_FROM_HERE,[&, source]() {
              observer_->OnMediaSourceAdded(source);
            });
        }
      }
    }
  }

  RTC_DCHECK_EQ(new_sources.size(), sources_.size());

  // Find the moved/changed sources.
  size_t pos = 0;
  while (pos < sources_.size()) {
    if (!(sources_[pos]->source_id() == new_sources[pos].id)) {
      // Find the source that should be moved to |pos|, starting from |pos + 1|
      // of |sources_|, because entries before |pos| should have been sorted.
      size_t old_pos = pos + 1;
      for (; old_pos < sources_.size(); ++old_pos) {
        if (sources_[old_pos]->source_id() == new_sources[pos].id)
          break;
      }
      RTC_DCHECK(sources_[old_pos]->source_id() == new_sources[pos].id);

      // Move the source from |old_pos| to |pos|.
      auto temp = sources_[old_pos];
      sources_.erase(sources_.begin() + old_pos);
      sources_.insert(sources_.begin() + pos, temp);
      //if(observer_) observer_->OnMediaSourceMoved:old_pos newIndex:pos];
    }

    if (sources_[pos]->source.title != new_sources[pos].title) {
      sources_[pos]->source.title = new_sources[pos].title;
      if(observer_) {
        auto source = sources_[pos].get();
        signaling_thread_->Invoke<void>(
          RTC_FROM_HERE,[&, source]() {
            observer_->OnMediaSourceNameChanged(source);
          });
      }
    }
    ++pos;
  }

  if(get_thumbnail) {
    for( auto source : sources_) {
       GetThumbnail(source.get(), true);
    }
  }
  return sources_.size();
}

bool RTCDesktopMediaListImpl::GetThumbnail(scoped_refptr<MediaSource> source, bool notify) {
    thread_->PostTask([this, source, notify] {
      MediaSourceImpl* source_impl = static_cast<MediaSourceImpl*>(source.get());
      if(capturer_->SelectSource(source_impl->source_id())){
        callback_->SetCallback([&](webrtc::DesktopCapturer::Result result,
                             std::unique_ptr<webrtc::DesktopFrame> frame) {
          auto old_thumbnail = source_impl->thumbnail();
          source_impl->SaveCaptureResult(result, std::move(frame));
         if(observer_ && notify) {
          signaling_thread_->Invoke<void>(
              RTC_FROM_HERE, [&, source_impl]() {
              observer_->OnMediaSourceThumbnailChanged(source_impl);
            });
          }
        });
        capturer_->CaptureFrame();
      }
  });
  return true;
}

int RTCDesktopMediaListImpl::GetSourceCount() const {
    return sources_.size();
}
  
scoped_refptr<MediaSource> RTCDesktopMediaListImpl::GetSource(int index) {
    return sources_[index];
}

bool MediaSourceImpl::UpdateThumbnail() {
  return mediaList_->GetThumbnail(this);
}

#ifdef WEBRTC_WIN
extern int filterException(int code, PEXCEPTION_POINTERS ex);
#endif

void MediaSourceImpl::SaveCaptureResult(
    webrtc::DesktopCapturer::Result result,
                                        std::unique_ptr<webrtc::DesktopFrame> frame) {
  if (result != webrtc::DesktopCapturer::Result::SUCCESS) {
    return;
  }

  int width = frame->size().width();
  int height = frame->size().height();
#ifdef WEBRTC_WIN
  webrtc::DesktopRect rect_ = webrtc::DesktopRect::MakeWH(width, height);

  if (type_ != kScreen) {
    webrtc::GetWindowRect(reinterpret_cast<HWND>(source_id()), &rect_);
  }

  __try
#endif
  {

    if (!i420_buffer_ || !i420_buffer_.get() ||
        i420_buffer_->width() * i420_buffer_->height() != width * height) {
      i420_buffer_ = webrtc::I420Buffer::Create(width, height);
    }

    libyuv::ConvertToI420(frame->data(), 0, i420_buffer_->MutableDataY(),
                          i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
                          i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
                          i420_buffer_->StrideV(), 0, 0, 
#ifdef WEBRTC_WIN
                          rect_.width(),
                          rect_.height(),
#else
                          width,
                          height,
#endif
                          width, height, libyuv::kRotate0,
                          libyuv::FOURCC_ARGB);

      webrtc::VideoFrame input_frame(i420_buffer_, 0, 0,
                                   webrtc::kVideoRotation_0);


     const int kColorPlanes = 3;  // R, G and B.
      size_t rgb_len = input_frame.height() * input_frame.width() * kColorPlanes;
      std::unique_ptr<uint8_t[]> rgb_buf(new uint8_t[rgb_len]);

      // kRGB24 actually corresponds to FourCC 24BG which is 24-bit BGR.
      if (ConvertFromI420(input_frame, webrtc::VideoType::kRGB24, 0, rgb_buf.get()) < 0) {
        RTC_LOG(LS_ERROR) << "Could not convert input frame to RGB.";
        return;
      }
      int quality = 80;
      unsigned char* out_buffer = NULL;
      unsigned long out_size = 0;
      // Invoking LIBJPEG
      struct jpeg_compress_struct cinfo;
      struct jpeg_error_mgr jerr;
      JSAMPROW row_pointer[1];
      cinfo.err = jpeg_std_error(&jerr);
      jpeg_create_compress(&cinfo);

      jpeg_mem_dest(&cinfo, &out_buffer, &out_size);

      int width = input_frame.width();
      int height = input_frame.height();

      cinfo.image_width = width;
      cinfo.image_height = height;
      cinfo.input_components = kColorPlanes;
      cinfo.in_color_space = JCS_EXT_BGR;
      jpeg_set_defaults(&cinfo);
      jpeg_set_quality(&cinfo, quality, TRUE);

      jpeg_start_compress(&cinfo, TRUE);
      int row_stride = width * kColorPlanes;
      while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &rgb_buf.get()[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
      }

      jpeg_finish_compress(&cinfo);
      jpeg_destroy_compress(&cinfo);

      thumbnail_.resize(out_size);

      std::copy(out_buffer
            , out_buffer + out_size
            , thumbnail_.begin());

      free(out_buffer);
  }
#ifdef WEBRTC_WIN
  __except (filterException(GetExceptionCode(), GetExceptionInformation())) {
  }
#endif
}

void RTCDesktopMediaListImpl::OnMessage(rtc::Message* msg) {
  
}

}  // namespace libwebrtc
