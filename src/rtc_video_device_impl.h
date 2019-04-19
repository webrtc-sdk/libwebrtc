#ifndef LIB_WEBRTC_VIDEO_DEVICE_IMPL_HXX
#define LIB_WEBRTC_VIDEO_DEVICE_IMPL_HXX

#include "rtc_video_device.h"

#include "media/base/videocapturer.h"
#include "media/base/videocapturerfactory.h"
#include "media/engine/webrtcvideocapturerfactory.h"
#include "modules/video_capture/video_capture.h"

#include <memory>

namespace libwebrtc {

class RTCVideoCapturerImpl : public RTCVideoCapturer {
 public:
  RTCVideoCapturerImpl(std::unique_ptr<cricket::VideoCapturer> video_capturer)
      : video_capturer_(std::move(video_capturer)) {}
  cricket::VideoCapturer* video_capturer() { return video_capturer_.release(); }
 private:
  std::unique_ptr<cricket::VideoCapturer> video_capturer_;
};

class RTCVideoDeviceImpl : public RTCVideoDevice {
 public:
  RTCVideoDeviceImpl();

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
  cricket::WebRtcVideoDeviceCapturerFactory factory_;
};

};  // namespace libwebrtc

#endif  // LIB_WEBRTC_VIDEO_DEVICE_IMPL_HXX
