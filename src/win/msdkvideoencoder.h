// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef OWT_BASE_WIN_MSDKVIDEOENCODER_H_
#define OWT_BASE_WIN_MSDKVIDEOENCODER_H_

#include <atomic>
#include <cstddef>
#include <memory>
#include <vector>

#include "api/video_codecs/video_codec.h"
#include "api/video_codecs/video_encoder.h"
#include "base_allocator.h"
#include "media/base/codec.h"
#include "mfxplugin++.h"
#include "mfxvideo++.h"
#include "mfxvideo.h"
#include "modules/video_coding/codecs/h264/include/h264.h"
#include "modules/video_coding/utility/ivf_file_writer.h"
#include "rtc_base/thread.h"
#include "sysmem_allocator.h"

namespace owt {
namespace base {

enum MemType {
  MSDK_SYSTEM_MEMORY = 0x00,
  MSDK_D3D9_MEMORY = 0x01,
  MSDK_D3D11_MEMORY = 0x02,
};

/// Encoder with Intel MediaSDK as the backend.
class MSDKVideoEncoder : public webrtc::VideoEncoder {
 public:
  explicit MSDKVideoEncoder(const cricket::VideoCodec& codec);
  virtual ~MSDKVideoEncoder();

  static std::unique_ptr<MSDKVideoEncoder> Create(cricket::VideoCodec format);
  int InitEncode(const webrtc::VideoCodec* codec_settings, int number_of_cores,
                 size_t max_payload_size) override;
  int Encode(const webrtc::VideoFrame& input_image,
             const std::vector<webrtc::VideoFrameType>* frame_types) override;
  int RegisterEncodeCompleteCallback(
      webrtc::EncodedImageCallback* callback) override;
  void SetRates(const RateControlParameters& parameters) override;
  void OnPacketLossRateUpdate(float packet_loss_rate) override;
  void OnRttUpdate(int64_t rtt_ms) override;
  void OnLossNotification(const LossNotification& loss_notification) override;
  EncoderInfo GetEncoderInfo() const override;
  int Release() override;

 private:
  int InitEncodeOnEncoderThread(const webrtc::VideoCodec* codec_settings,
                                int number_of_cores, size_t max_payload_size);
  int32_t NextNaluPosition(uint8_t* buffer, size_t buffer_size,
                           uint8_t* sc_length);
  mfxU16 MSDKGetFreeSurface(mfxFrameSurface1* pSurfacesPool, mfxU16 nPoolSize);
  mfxU16 MSDKGetFreeSurfaceIndex(mfxFrameSurface1* pSurfacesPool,
                                 mfxU16 nPoolSize);
  void WipeMfxBitstream(mfxBitstream* pBitstream);

  webrtc::EncodedImageCallback* callback_;
  int32_t bitrate_;  // Bitrate in bits per second.
  int32_t width_;
  int32_t height_;
  uint32_t frame_rate;
  webrtc::VideoCodecType codec_type_;
  cricket::VideoCodec rtp_codec_parameters_;
  uint8_t num_temporal_layers_ = 1;

  MFXVideoSession* m_mfx_session_;
  std::unique_ptr<MFXVideoENCODE> m_pmfx_enc_;
  std::shared_ptr<SysMemFrameAllocator> m_pmfx_allocator_;
  mfxVideoParam m_mfx_enc_params_;

  // TODO(johny): MSDK will remove the version macro usage for headers.
  // Turn this on when appropriate.
#if (MFX_VERSION >= MFX_VERSION_NEXT)
  mfxExtAV1Param av1_ext_param_;
  AV1Profile av1_profile_ = owt::base::AV1Profile::kMain;
#endif

  std::vector<mfxExtBuffer*> m_enc_ext_params_;
  mfxFrameAllocResponse m_enc_response_;
  mfxFrameSurface1* m_penc_surfaces_;  // frames array for encoder
  mfxU32 m_frames_processed_;
  std::unique_ptr<rtc::Thread> encoder_thread_;
  std::atomic<bool> inited_;

  std::unique_ptr<webrtc::IvfFileWriter> dump_writer_;
  bool enable_bitstream_dump_ = false;
  std::string encoder_dump_file_name_;
};
}  // namespace base
}  // namespace owt
#endif  // OWT_BASE_WIN_MSDKVIDEOENCODER_H_
