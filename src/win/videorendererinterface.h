// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0
#ifndef OWT_BASE_VIDEORENDERERINTERFACE_H_
#define OWT_BASE_VIDEORENDERERINTERFACE_H_
#include <memory>
#include "src/win/commontypes.h"
#if defined(WEBRTC_WIN)
#include <d3d11.h>
#include <windows.h>
#endif
#if defined(WEBRTC_LINUX)
#if defined(WEBRTC_USE_X11)
#include <X11/Xlib.h>
#endif
#endif
namespace owt {
namespace base {
enum class VideoBufferType {
  kI420,
  kARGB,
  kD3D11,  // Format self-described.
};
enum class VideoRendererType {
  kI420,
  kARGB,
  kD3D11,  // Format self-described.
};

#if defined(WEBRTC_WIN)
struct D3D11ImageHandle {
  ID3D11Device* d3d11_device;
  ID3D11Texture2D* texture;     // The DX texture or texture array.
  int texture_array_index;      // When >=0, indicate the index within texture array 
};
#endif
/// Video buffer and its information
struct VideoBuffer {
  /// Video buffer
  uint8_t* buffer;
  /// Resolution for the Video buffer
  Resolution resolution;
  // Buffer type
  VideoBufferType type;
  ~VideoBuffer() { delete[] buffer; }
};
/// VideoRenderWindow wraps a native Window handle
#if defined(WEBRTC_WIN)
class VideoRenderWindow {
 public:
  VideoRenderWindow() : wnd_(nullptr) {}
  virtual ~VideoRenderWindow() {}
  /**
    Set the render window handle for VideoRenderWindow instance.
    @param wnd Window handle that will be used for rendering.
  */
  void SetWindowHandle(HWND wnd) { wnd_ = wnd; }
  /**
    Get the window handle that will be used for rendering.
    @return Returns the window handle.
  */
  HWND GetWindowHandle() { return wnd_; }
 private:
  HWND wnd_;
};
#endif

#if defined(WEBRTC_LINUX)
#if defined(WEBRTC_USE_X11)
class VideoRenderWindow {
 public:
  VideoRenderWindow() : wnd_(0) {}
  virtual ~VideoRenderWindow() {}
  /**
    Set the render window handle for VideoRenderWindow instance.
    @param wnd Window handle that will be used for rendering.
  */
  void SetWindowHandle(Window wnd) { wnd_ = wnd; }
  /**
    Get the window handle that will be used for rendering.
    @return Returns the window handle.
  */
  Window GetWindowHandle() { return wnd_; }
 private:
  Window wnd_;
};
#endif
#endif
#if defined(WEBRTC_LINUX)
typedef void (*PFN_RETURN_BUFFER)(void *data, uint32_t bufid);
typedef void* VADisplay;        /* window system dependent */
typedef unsigned int VASurfaceID;

/// libva surface that contains a decoded image for rendering on
/// target window system.
struct VaSurface {
  /// va display associated with decoder.
  VADisplay display;
  /// va surface ID.
  VASurfaceID surface;
  /// Width of image.
  int width;
  /// Height of image.
  int height;
  /// Unique frame number assigned by decoder.
  int frameno;
  /// Index of buffer in surface pool.
  int bufferid;
  /// Pointer to side data.
  void *data;
  /// Buffer free callback when surface is no longer used by renderer.
  void *pfnReturnBuffer;
  ~VaSurface() {}
};

/// Video renderer interface for Linux using va based decoding.
class VideoRendererVaInterface {
 public:
  virtual void RenderFrame(std::unique_ptr<VaSurface> surface) = 0;
  virtual ~VideoRendererVaInterface() {}
};
#endif
/// Interface for rendering VideoFrames in ARGB/I420 format from a VideoTrack.
class VideoRendererInterface {
 public:
  /// Passes video buffer to renderer.
  virtual void RenderFrame(std::unique_ptr<VideoBuffer> buffer) {}
  virtual ~VideoRendererInterface() {}
  /// Render type that indicates the VideoBufferType the renderer would receive.
  virtual VideoRendererType Type() = 0;
};
}  // namespace base
}  // namespace owt
#endif  // OWT_BASE_VIDEORENDERERINTERFACE_H_
