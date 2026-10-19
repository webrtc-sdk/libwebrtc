#ifndef LIB_WEBRTC_RTC_DESKTOP_DEVICE_HXX
#define LIB_WEBRTC_RTC_DESKTOP_DEVICE_HXX

#include "rtc_types.h"
#include <map>
#include <string>

namespace libwebrtc {

class MediaSource;
class RTCDesktopCapturer;
class RTCDesktopMediaList;

/**
 * @brief Options for a desktop capturer. The defaults behave like
 *        CreateDesktopCapturer(source, showCursor).
 */
struct RTCDesktopCapturerOptions {
  /** Draw the mouse cursor into the captured frames. */
  bool show_cursor = true;

  /**
   * Bring a shared window to the front when capture starts. Start() returns
   * CS_FAILED when the window can't be brought forward (Windows may refuse
   * to change the foreground window). Turn it off to share a window that
   * stays in the background; this needs a capturer that can read covered
   * windows, such as Windows.Graphics.Capture.
   */
  bool focus_window = true;

  /**
   * Windows only: capture windows with Windows.Graphics.Capture (WGC) where
   * the system supports it (Windows 10 1903 or later). WGC reads the window
   * from the compositor, so a covered or background window keeps sending
   * frames. Elsewhere the default window capturer is used.
   */
  bool allow_wgc_window_capturer = false;

  /**
   * Windows, WGC only: whether Windows draws its capture border around the
   * shared window. When off, the capturer asks for borderless capture
   * (GraphicsCaptureAccess::RequestAccessAsync with
   * GraphicsCaptureAccessKind::Borderless) and removes the border only if
   * that is allowed and the system supports it (Windows 11); otherwise the
   * border stays. RTCDesktopCapturer::IsCaptureBorderHidden() tells which.
   */
  bool wgc_border_required = true;
};

class RTCDesktopDevice : public RefCountInterface {
 public:
  virtual scoped_refptr<RTCDesktopCapturer> CreateDesktopCapturer(
      scoped_refptr<MediaSource> source, bool showCursor = true) = 0;
  virtual scoped_refptr<RTCDesktopMediaList> GetDesktopMediaList(
      DesktopType type) = 0;
  /**
   * @brief Creates a capturer for source with options. Added last, and not
   *        as an overload, so the existing vtable slots keep their order.
   */
  virtual scoped_refptr<RTCDesktopCapturer> CreateDesktopCapturerWithOptions(
      scoped_refptr<MediaSource> source,
      const RTCDesktopCapturerOptions& options) = 0;

 protected:
  virtual ~RTCDesktopDevice() {}
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_VIDEO_DEVICE_HXX