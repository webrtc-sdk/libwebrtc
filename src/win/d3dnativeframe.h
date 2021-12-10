// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0
#ifndef OWT_BASE_WIN_D3DNATIVEFRAME_H
#define OWT_BASE_WIN_D3DNATIVEFRAME_H
#include <d3d9.h>
#include <dxva2api.h>
#include "common_video/include/video_frame_buffer.h"
namespace owt {
namespace base {
// structure that containts d3d9 device manager
// and d3d9 surface that contains decoded frame.
struct NativeD3DSurfaceHandle {
  IDirect3DSurface9* surface_;
  IDirect3DDeviceManager9* dev_manager_;
  UINT dev_manager_reset_token_;
  int width_;   // width of the frame passing from decoder
  int height_;  // height of the frame passing from decoder
};

// We keep D3D11ImageHandle definition in videorendererinterface,
// as we may allow external renderer to accept the same handle.
}  // namespace base
}  // namespace owt
#endif  // OWT_BASE_WIN_D3DNATIVEFRAME_H
