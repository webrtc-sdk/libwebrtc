#ifndef LIB_WEBRTC_VIDEO_DEVICE_IMPL_HXX
#define LIB_WEBRTC_VIDEO_DEVICE_IMPL_HXX

#include "rtc_video_device.h"

#include "modules/video_capture/video_capture.h"
#include "src/internal/vcm_capturer.h"
#include "src/internal/video_capturer.h"
#include "rtc_base/thread.h"

#include <memory>

namespace libwebrtc {

class RTCVideoCapturerImpl : public RTCVideoCapturer {
 public:
  RTCVideoCapturerImpl(
      std::unique_ptr<webrtc::internal::VideoCapturer> video_capturer)
      : video_capturer_(std::move(video_capturer)) {}
  std::unique_ptr<webrtc::internal::VideoCapturer> video_capturer() {
    return std::move(video_capturer_);
  }

 private:
  std::unique_ptr<webrtc::internal::VideoCapturer> video_capturer_;
};

class RTCVideoDeviceImpl : public RTCVideoDevice {
 public:
  RTCVideoDeviceImpl(rtc::Thread* signaling_thread);

 public:
  uint32_t NumberOfDevices() override;

  int32_t GetDeviceName(uint32_t deviceNumber,
                        char* deviceNameUTF8,
                        uint32_t deviceNameLength,
                        char* deviceUniqueIdUTF8,
                        uint32_t deviceUniqueIdUTF8Length,
                        char* productUniqueIdUTF8 = 0,
                        uint32_t productUniqueIdUTF8Length = 0) override;

  scoped_refptr<RTCVideoCapturer> Create(const char* name,
                                         uint32_t index) override;

 private:
  std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> device_info_;
  rtc::Thread* signaling_thread_ = nullptr;
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_VIDEO_DEVICE_IMPL_HXX
