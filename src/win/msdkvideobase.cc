// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#include "msdkvideobase.h"
#include "atlbase.h"
#include "d3d11_allocator.h"
#include "d3d_allocator.h"
#include "mfxdefs.h"

#include "rtc_base/logging.h"

namespace owt {
namespace base {

std::mutex MSDKFactory::get_singleton_mutex;
MSDKFactory* MSDKFactory::singleton = nullptr;

static bool AreGuidsEqual(const mfxPluginUID* guid_first,
                          const mfxPluginUID* guid_second) {
  for (size_t i = 0; i < sizeof(mfxPluginUID); i++) {
    if (guid_first->Data[i] != guid_second->Data[i])
      return false;
  }
  return true;
}

static bool isValidPluginUID(const mfxPluginUID* uid) {
  return (AreGuidsEqual(uid, &MFX_PLUGINID_HEVCD_HW) ||
          AreGuidsEqual(uid, &MFX_PLUGINID_HEVCE_HW) ||
          AreGuidsEqual(uid, &MFX_PLUGINID_VP8D_HW) ||
          AreGuidsEqual(uid, &MFX_PLUGINID_VP9D_HW) ||
          AreGuidsEqual(uid, &MFX_PLUGINID_HEVCE_GACC));
}

MSDKFactory::MSDKFactory() : main_session(nullptr), mfe_timeout(0) {}

bool MSDKFactory::Init() {
  main_session = InternalCreateSession();

  if (!main_session) {
    return false;
  }
  return true;
}

MSDKFactory::~MSDKFactory() {}

void MSDKFactory::MFETimeout(uint32_t timeout) {
  mfe_timeout = timeout < 100 ? timeout : 100;
}

uint32_t MSDKFactory::MFETimeout() {
  return mfe_timeout;
}

MSDKFactory* MSDKFactory::Get() {
  std::lock_guard<std::mutex> lock(get_singleton_mutex);

  if (singleton == nullptr) {
    singleton = new MSDKFactory();

    if (singleton && !singleton->Init()) {
      delete singleton;
      singleton = nullptr;
    }
  }

  return singleton;
}

MFXVideoSession* MSDKFactory::InternalCreateSession(bool use_d3d11) {
  mfxStatus sts = MFX_ERR_NONE;
  mfxIMPL impl = MFX_IMPL_HARDWARE_ANY;
  if (use_d3d11)
    impl |= MFX_IMPL_VIA_D3D11;
  mfxVersion version = {{3, 1}};

  MFXVideoSession* session = new MFXVideoSession();
  if (!session)
    return nullptr;

  sts = session->Init(impl, &version);
  if (sts != MFX_ERR_NONE) {
    delete session;
    return nullptr;
  }

  // For Linux you may want to set VA display here.
  return session;
}

MFXVideoSession* MSDKFactory::CreateSession(bool use_d3d11) {
  mfxStatus sts = MFX_ERR_NONE;
  MFXVideoSession* session = nullptr;

  session = InternalCreateSession(use_d3d11);

  if (!session) {
    return nullptr;
  }

  sts = main_session->JoinSession(*session);
  if (sts != MFX_ERR_NONE) {
    session->Close();
    delete session;
    return nullptr;
  }

  return session;
}

void MSDKFactory::DestroySession(MFXVideoSession* session) {
  if (session) {
    session->DisjoinSession();
    session->Close();
    delete session;
  }
}

bool MSDKFactory::LoadDecoderPlugin(uint32_t codec_id,
                                    MFXVideoSession* session,
                                    mfxPluginUID* plugin_id) {
  mfxStatus sts = MFX_ERR_NONE;

  switch (codec_id) {
    case MFX_CODEC_HEVC:
      // Actually MSDK does not support this plugin any more. Consider
      // not loading the plugin in decoder implementation.
      sts = MFXVideoUSER_Load(*session, &MFX_PLUGINID_HEVCD_HW, 1);
      if (sts != MFX_ERR_NONE) {
        return false;
      }
      *plugin_id = MFX_PLUGINID_HEVCD_HW;
      break;
    case MFX_CODEC_AVC:
      break;
    case MFX_CODEC_VP8:
      sts = MFXVideoUSER_Load(*session, &MFX_PLUGINID_VP8D_HW, 1);
      if (sts != MFX_ERR_NONE) {
        return false;
      }
      *plugin_id = MFX_PLUGINID_VP8D_HW;
      break;
    case MFX_CODEC_VP9:
      sts = MFXVideoUSER_Load(*session, &MFX_PLUGINID_VP9D_HW, 1);
      if (sts != MFX_ERR_NONE) {
        return false;
      }
      *plugin_id = MFX_PLUGINID_VP9D_HW;
      break;
    default:
      break;
  }
  return true;
}

bool MSDKFactory::LoadEncoderPlugin(uint32_t codec_id,
                                    MFXVideoSession* session,
                                    mfxPluginUID* plugin_id) {
  mfxStatus sts = MFX_ERR_NONE;
  switch (codec_id) {
    case MFX_CODEC_HEVC:
      sts = MFXVideoUSER_Load(*session, &MFX_PLUGINID_HEVCE_GACC, 1);
      if (sts != MFX_ERR_NONE) {
        return false;
      }
      *plugin_id = MFX_PLUGINID_HEVCE_GACC;
      break;
    case MFX_CODEC_AVC:
      break;
    default:
      break;
  }
  return true;
}

void MSDKFactory::UnloadMSDKPlugin(MFXVideoSession* session,
                                   mfxPluginUID* plugin_id) {
  if (isValidPluginUID(plugin_id)) {
    MFXVideoUSER_UnLoad(*session, plugin_id);
  }
}

bool MSDKFactory::QueryPlatform(MFXVideoSession* session,
                                mfxPlatform* platform) {
  if (!session || !platform)
    return false;

  mfxStatus sts = MFX_ERR_NONE;
  sts = MFXVideoCORE_QueryPlatform(*session, platform);
  if (sts != MFX_ERR_NONE) {
    RTC_LOG(LS_ERROR) << "Failed to qeury platform info.";
    return false;
  }

  return true;
}

std::shared_ptr<D3DFrameAllocator> MSDKFactory::CreateFrameAllocator(
    IDirect3DDeviceManager9* d3d_manager) {
  mfxStatus sts = MFX_ERR_NONE;
  D3DAllocatorParams param;

  param.pManager = d3d_manager;
  std::shared_ptr<D3DFrameAllocator> pAllocator =
      std::make_shared<D3DFrameAllocator>();
  sts = pAllocator->Init(&param);
  if (sts != MFX_ERR_NONE) {
    return nullptr;
  }

  return pAllocator;
}

std::shared_ptr<SysMemFrameAllocator> MSDKFactory::CreateFrameAllocator() {
  mfxStatus sts = MFX_ERR_NONE;

  std::shared_ptr<SysMemFrameAllocator> pAllocator =
      std::make_shared<SysMemFrameAllocator>();
  sts = pAllocator->Init(nullptr);
  if (sts != MFX_ERR_NONE) {
    return nullptr;
  }

  return pAllocator;
}

std::shared_ptr<D3D11FrameAllocator> MSDKFactory::CreateD3D11FrameAllocator(
    ID3D11Device* d3d11_device) {
  if (!d3d11_device) {
    RTC_LOG(LS_ERROR) << "Invalid D3D11 device provided for frame allocator.";
    return nullptr;
  }
  mfxStatus sts = MFX_ERR_NONE;
  std::shared_ptr<D3D11FrameAllocator> pAllocator =
      std::make_shared<D3D11FrameAllocator>();
  D3D11AllocatorParams param;
  param.pDevice = d3d11_device;
  sts = pAllocator->Init(&param);
  if (sts != MFX_ERR_NONE) {
    return nullptr;
  }

  return pAllocator;
}

}  // namespace base
}  // namespace owt