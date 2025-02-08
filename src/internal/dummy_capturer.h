/**
 * File provided for Reference Use Only by isoft (c) 2025.
 * Copyright (c) isoft. All rights reserved.
 * 
 */

#ifndef LIB_WEBRTC_DUMMY_CAPTURER_IMPL_HXX
#define LIB_WEBRTC_DUMMY_CAPTURER_IMPL_HXX

#include "api/scoped_refptr.h"
#include "api/video/i420_buffer.h"
#include "api/video/video_frame.h"
#include "api/video/video_source_interface.h"
#include "include/base/refcount.h"
#include "media/base/video_adapter.h"
#include "media/base/video_broadcaster.h"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"
#include "pc/video_track_source.h"
#include "rtc_base/thread.h"
#include "rtc_types.h"
#include "src/internal/video_capturer.h"
#include "src/rtc_dummy_video_capturer_impl.h"
#include "third_party/libyuv/include/libyuv.h"

namespace libwebrtc {

class DummyCapturerTrackSource : public webrtc::VideoTrackSource {
 public:
  static rtc::scoped_refptr<DummyCapturerTrackSource> Create(
      scoped_refptr<RTCDummyVideoCapturer> capturer) {
    if (capturer) {
      return rtc::make_ref_counted<DummyCapturerTrackSource>(capturer);
    }
    return nullptr;
  }

 public:
  explicit DummyCapturerTrackSource(scoped_refptr<RTCDummyVideoCapturer> capturer)
      : VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {}
  virtual ~DummyCapturerTrackSource() { capturer_->Stop(); }

 private:
  rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
    return static_cast<RTCDummyVideoCapturerImpl*>(capturer_.get());
  }

  scoped_refptr<RTCDummyVideoCapturer> capturer_;
};

}  // namespace libwebrtc

#endif // LIB_WEBRTC_DUMMY_CAPTURER_IMPL_HXX
