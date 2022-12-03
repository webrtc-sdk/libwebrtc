// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef OWT_BASE_WIN_MSDKVIDEOBASE_H_
#define OWT_BASE_WIN_MSDKVIDEOBASE_H_

#pragma warning(disable : 4201)
#include <d3d9.h>
#include <d3d11.h>
#include <dxva2api.h>
#include <dxva.h>
#include <mfxdefs.h>
#include <mfxvideo++.h>
#include <mfxplugin++.h>
#include <mfxvp8.h>
#include <memory>
#include <mutex>
#include "msdkcommon.h"
#include "src/win/d3d_allocator.h"
#include "src/win/d3d11_allocator.h"
#include "src/win/sysmem_allocator.h"

namespace owt {
namespace base {

/// Base class for handling sessions and surface allocation
class MSDKFactory {
 public:
  ~MSDKFactory();

  static MSDKFactory* Get();

  MFXVideoSession* CreateSession(bool use_d3d11 = true);

  void DestroySession(MFXVideoSession* session);
  
  MFXVideoSession* GetMainSession();

  bool QueryPlatform(MFXVideoSession* session, mfxPlatform* platform);
  bool LoadDecoderPlugin(uint32_t codec_id, MFXVideoSession* session, mfxPluginUID* plugin_id);
  bool LoadEncoderPlugin(uint32_t codec_id, MFXVideoSession* session, mfxPluginUID* plugin_id);
  void UnloadMSDKPlugin(MFXVideoSession* session, mfxPluginUID* plugin_id);

  static std::shared_ptr<D3DFrameAllocator> CreateFrameAllocator(IDirect3DDeviceManager9* d3d_manager);
  static std::shared_ptr<SysMemFrameAllocator> CreateFrameAllocator();
  static std::shared_ptr<D3D11FrameAllocator> CreateD3D11FrameAllocator(ID3D11Device* d3d11_device);
  void MFETimeout(uint32_t timeout);
  uint32_t MFETimeout();
  struct MSDKAdapter {
    // Returns the number of adapter associated with MSDK session, 0 for SW
    // session
    static mfxU32 GetNumber(mfxSession session, mfxIMPL implVia = 0) {
      mfxU32 adapterNum = 0;             // default
      mfxIMPL impl = MFX_IMPL_SOFTWARE;  // default in case no HW IMPL is found

      // We don't care for error codes in further code; if something goes wrong
      // we fall back to the default adapter
      if (session) {
        MFXQueryIMPL(session, &impl);
      } else {
        // An auxiliary session, internal for this function
        mfxSession auxSession;
        memset(&auxSession, 0, sizeof(auxSession));

        mfxVersion ver = {
            {1, 1}};  // Minimum API version which supports multiple devices
        MFXInit(MFX_IMPL_HARDWARE_ANY | implVia, &ver, &auxSession);
        MFXQueryIMPL(auxSession, &impl);
        MFXClose(auxSession);
      }

      // Extract the base implementation type
      mfxIMPL baseImpl = MFX_IMPL_BASETYPE(impl);

      const struct {
        // Actual implementation
        mfxIMPL impl;
        // Adapter's number
        mfxU32 adapterID;

      } implTypes[] = {{MFX_IMPL_HARDWARE, 0},
                       {MFX_IMPL_SOFTWARE, 0},
                       {MFX_IMPL_HARDWARE2, 1},
                       {MFX_IMPL_HARDWARE3, 2},
                       {MFX_IMPL_HARDWARE4, 3}};

      // Get corresponding adapter number
      for (mfxU8 i = 0; i < sizeof(implTypes) / sizeof(*implTypes); i++) {
        if (implTypes[i].impl == baseImpl) {
          adapterNum = implTypes[i].adapterID;
          break;
        }
      }

      return adapterNum;
    }
 };
 protected:
  MSDKFactory();
  bool Init();
  MFXVideoSession* InternalCreateSession(bool use_d3d11 = true);

 private:
  static MSDKFactory* singleton;
  static std::mutex get_singleton_mutex;
  MFXVideoSession* main_session;  
  uint32_t mfe_timeout;
};
}  // namespace base
}  // namespace owt
#endif  // OWT_BASE_WIN_MSDKVIDEOBASE_H_
