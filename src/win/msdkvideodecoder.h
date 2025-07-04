// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0
#ifndef OWT_BASE_WIN_MSDKVIDEODECODER_H_
#define OWT_BASE_WIN_MSDKVIDEODECODER_H_

#include <codecapi.h>
#include <combaseapi.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dxva2api.h>

#include <memory>
#include <utility>
#include <vector>

#include "media/base/codec.h"
#include "modules/video_coding/include/video_codec_interface.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/synchronization/mutex.h"
#include "rtc_base/thread.h"

// Solve conflict in FOURCC def with libyuv
#ifdef FOURCC
#undef FOURCC
#endif
#include "src/win/d3d11_allocator.h"
#include "src/win/d3dnativeframe.h"
#include "src/win/msdkvideobase.h"

#if defined(WEBRTC_WIN)
struct D3D11ImageHandle {
  ID3D11Device* d3d11_device;
  ID3D11Texture2D* texture;  // The DX texture or texture array.
  int texture_array_index;  // When >=0, indicate the index within texture array
};
#endif

namespace owt {
namespace base {

//
// MSDK Video Decoder declaration.
//
class MSDKVideoDecoder : public webrtc::VideoDecoder {
 public:
  enum State {
    kUnitialized,
    kNormal,
    kResetting,
    kStopped,
    kFlushing,
  };
  explicit MSDKVideoDecoder();
  virtual ~MSDKVideoDecoder();

  static std::unique_ptr<MSDKVideoDecoder> Create(webrtc::VideoCodec format);

  bool Configure(const Settings& settings) override;

  int32_t Decode(const webrtc::EncodedImage& inputImage, bool missingFrames,
                 int64_t renderTimeMs = -1) override;

  int32_t RegisterDecodeCompleteCallback(
      webrtc::DecodedImageCallback* callback) override;

  int32_t Release() override;

  const char* ImplementationName() const override;

 private:
  int32_t InitDecodeOnCodecThread();
  void CheckOnCodecThread();
  bool CreateD3D11Device();
  int32_t Reset();

  mfxStatus ExtendMfxBitstream(mfxBitstream* pBitstream, mfxU32 nSize);
  void WipeMfxBitstream(mfxBitstream* pBitstream);
  void ReadFromInputStream(mfxBitstream* pBitstream, const uint8_t* data,
                           size_t len);
  mfxU16 DecGetFreeSurface(mfxFrameSurface1* pSurfacesPool, mfxU16 nPoolSize);
  mfxU16 DecGetFreeSurfaceIndex(mfxFrameSurface1* pSurfacesPool,
                                mfxU16 nPoolSize);

  // Type of video codec.
  webrtc::VideoCodecType codec_type_;
  // Begin MSDK variables
  MFXVideoSession* m_mfx_session_;
  std::unique_ptr<MFXVideoDECODE> m_pmfx_dec_;
  std::shared_ptr<D3D11FrameAllocator> m_pmfx_allocator_;
  mfxVideoParam m_pmfx_video_params_;
  mfxBitstream m_mfx_bs_;
  mfxFrameAllocResponse m_mfx_response_;
  mfxFrameSurface1* m_pinput_surfaces_;
  mfxPluginUID m_plugin_id_;
  bool m_video_param_extracted;
  uint32_t m_dec_bs_offset_;
  // End of MSDK variables

  CComPtr<ID3D11Device> d3d11_device_;
  CComPtr<ID3D11DeviceContext> d3d11_device_context_;
  CComPtr<ID3D11VideoDevice> d3d11_video_device_;
  CComPtr<ID3D11VideoContext> d3d11_video_context_;
  CComQIPtr<IDXGIAdapter> m_padapter_;
  CComPtr<IDXGIFactory2> m_pdxgi_factory_;
  // Store current decoded frame.
  std::unique_ptr<D3D11ImageHandle> surface_handle_;

  bool inited_;
  int width_;
  int height_;
  std::unique_ptr<webrtc::Thread>
      decoder_thread_;  // Thread on which the decoder will be working on.

  webrtc::VideoDecoder::Settings settings_;
  webrtc::DecodedImageCallback* callback_;
  webrtc::Mutex timestampCS_;
  std::vector<int64_t> ntp_time_ms_;
  std::vector<int32_t> timestamps_;
};
}  // namespace base
}  // namespace owt
#endif  // OWT_BASE_WIN_MSDKVIDEODECODER_H_
