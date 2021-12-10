// Copyright (C) <2020> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#include <string>
#include <vector>
#include "mfxcommon.h"
#include "mfxstructures.h"
#include "src/win/mediacapabilities.h"
#include "rtc_base/logging.h"

namespace owt {
namespace base {

std::mutex MediaCapabilities::get_singleton_mutex_;
MediaCapabilities* MediaCapabilities::singleton_ = nullptr;

std::vector<VideoEncoderCapability>
MediaCapabilities::SupportedCapabilitiesForVideoEncoder(
    std::vector<owt::base::VideoCodec>& codec_types) {
  // According to https://github.com/intel/media-driver, some pre-condition
  // that we apply to encoder capability before we actually query encoder:
  //
  // a. When HW encoding with LowPower enabled:
  //
  // - a.1. AVC.  On ICL/KBL/CFL/RKL/TGL, AVC encoding supports following input
  // formats: NV12/YUY2/YUYV/YVYU/UYVY/AYUV/ARGB besides already supported
  // NV12 on SKL/BXT/APL/KBL. (No LP for BDW). Max to 4K.
  // - a.2. HEVC 8-bit. Only supported on ICL/TGL/RKL and format limitation
  // would be NV12/AYUV only. Max to 8K.
  // - a.3. HEVC 10-bit. Only supported on ICL/TGL/RKL and format limitation
  // would be P010/Y410. Max to 8K.
  // - a.4. VP9 8-bit. Supported on ICL/TGL/RKL and format limitation would be
  // NV12/AYUV. Max to 8K.
  // - a.5. VP9 10-bit. Supported on ICL/TGL/RKL and format limitation would be
  // P010/Y410. Max to 8k.
  //
  // b. When HW encoding with PAK + Shader(kernel+VME). They are always with
  // input format NV12 except HEVC 8-bit, which allows AYUV. Typically we will
  // turn off this encoding mode in OWT, with exceptions below:
  //
  // - b.1. AVC. Platforms below BDW(inclusive). Maximum 4K.
  //
  // Although hardware support it, we will not enable HW encoding for VP8/HEVC
  // with PAK + Shader.
  std::vector<VideoEncoderCapability> capabilities;

  bool support_h264 = false, h264_lp = false, h264_argb = false;
  bool support_vp9_8 = false, support_vp9_10 = false;
  bool is_discrete_graphics = false;

  // Check platform type.
  if (inited_) {
    unsigned short platform_code = mfx_platform_.CodeName;
    if (platform_code >= MFX_PLATFORM_HASWELL) {
      support_h264 = true;
      if (platform_code > MFX_PLATFORM_BROADWELL)
        h264_lp = true;
      if (platform_code >= MFX_PLATFORM_KABYLAKE) {
        // Spec says KBL/CFL/ICL/TGL. Apply to all after KBL.
        h264_argb = true;
      }
#if (MFX_VERSION >= 1027)
      if (platform_code >= MFX_PLATFORM_ICELAKE) {
        support_vp9_8 = true;
        support_vp9_10 = true;
      }
#endif
#if (MFX_VERSION >= 1031)
      if (mfx_platform_.MediaAdapterType == MFX_MEDIA_DISCRETE)
        is_discrete_graphics = true;
#endif
      // Query platform capability for specific codec. Only check for
      // VP9/HEVC/AVC at this stage, as AV1 HW encoding is not enabled.
      mfxStatus sts = MFX_ERR_NONE;
      mfxVideoParam video_param;

      for (auto& codec : codec_types) {
        if (codec == owt::base::VideoCodec::kVp9) {
          memset(&video_param, 0, sizeof(video_param));
          video_param.mfx.CodecId = MFX_CODEC_VP9;
          video_param.mfx.CodecProfile = MFX_PROFILE_VP9_0;
          sts = mfx_encoder_->Query(nullptr, &video_param);
          if (sts != MFX_ERR_NONE)
            support_vp9_8 &= false;

          memset(&video_param, 0, sizeof(video_param));
          video_param.mfx.CodecId = MFX_CODEC_VP9;
          video_param.mfx.CodecProfile = MFX_PROFILE_VP9_2;
          sts = mfx_encoder_->Query(nullptr, &video_param);
          if (sts != MFX_ERR_NONE)
            support_vp9_10 &= false;

          // VP9 will always be low power. And for MSDK, temporal scalability
          // and tiles don't work togehter. Also be noted we only support
          // input of size not smaller than 256x144.
          if (support_vp9_8) {
            VideoEncoderCapability vp9_8_cap;
            vp9_8_cap.codec_type = owt::base::VideoCodec::kVp9;
            vp9_8_cap.has_trusted_rate_controller = true;
            vp9_8_cap.hardware_accelerated = true;
            vp9_8_cap.low_power = true;
            vp9_8_cap.max_temporal_layers = 3;
            vp9_8_cap.max_spatial_layers = 1;
            vp9_8_cap.codec_specific.VP9.profile = VP9Profile::kProfile0;
            vp9_8_cap.supported_brc_modes.push_back(BRCMode::kCQP);
            vp9_8_cap.supported_brc_modes.push_back(BRCMode::kCBR);
            vp9_8_cap.supported_brc_modes.push_back(BRCMode::kICQ);
            vp9_8_cap.supported_brc_modes.push_back(BRCMode::kVBR);
            // We don't add AYUV into supported list.
            vp9_8_cap.sampling_modes.push_back(SamplingMode::kNv12);
            capabilities.push_back(vp9_8_cap);
          }
          if (support_vp9_10) {
            VideoEncoderCapability vp9_10_cap;
            vp9_10_cap.codec_type = owt::base::VideoCodec::kVp9;
            vp9_10_cap.has_trusted_rate_controller = true;
            vp9_10_cap.hardware_accelerated = true;
            vp9_10_cap.low_power = true;
            vp9_10_cap.max_temporal_layers = 3;
            vp9_10_cap.max_spatial_layers = 1;
            vp9_10_cap.codec_specific.VP9.profile = VP9Profile::kProfile2;
            vp9_10_cap.supported_brc_modes.push_back(BRCMode::kCQP);
            vp9_10_cap.supported_brc_modes.push_back(BRCMode::kCBR);
            vp9_10_cap.supported_brc_modes.push_back(BRCMode::kICQ);
            vp9_10_cap.supported_brc_modes.push_back(BRCMode::kVBR);
            // TODO: check if Y410 is really supported.
            vp9_10_cap.sampling_modes.push_back(SamplingMode::kP010);
            vp9_10_cap.sampling_modes.push_back(SamplingMode::kY410);
            capabilities.push_back(vp9_10_cap);
          }
        }
        else if (codec == owt::base::VideoCodec::kH264) {
          memset(&video_param, 0, sizeof(video_param));
          video_param.mfx.CodecId = MFX_CODEC_AVC;
          // Don't check profiles. We know we can support from CB up to High.
          sts = mfx_encoder_->Query(nullptr, &video_param);
          if (sts != MFX_ERR_NONE)
            support_h264 &= false;

          if (support_h264) {
            VideoEncoderCapability avc_cap;
            avc_cap.codec_type = owt::base::VideoCodec::kH264;
            avc_cap.has_trusted_rate_controller = true;
            avc_cap.low_power = h264_lp;
            avc_cap.max_temporal_layers = 3;
            avc_cap.max_spatial_layers = 1;
            avc_cap.supported_brc_modes.push_back(BRCMode::kCBR);
            avc_cap.supported_brc_modes.push_back(BRCMode::kVBR);
            avc_cap.supported_brc_modes.push_back(BRCMode::kCQP);
            avc_cap.sampling_modes.push_back(SamplingMode::kNv12);
            if (h264_argb)
              avc_cap.sampling_modes.push_back(SamplingMode::kARGB);
            capabilities.push_back(avc_cap);
          }
        }
      }
    }
  }
  return capabilities;
}

std::vector<VideoDecoderCapability>
MediaCapabilities::SupportedCapabilitiesForVideoDecoder(
    std::vector<owt::base::VideoCodec>& codec_types) {
  std::vector<VideoDecoderCapability> capabilities;
  if (inited_) {
    mfxStatus sts = MFX_ERR_NONE;
    mfxVideoParam video_param;

    unsigned short platform_code = mfx_platform_.CodeName;
    for (auto& codec : codec_types) {
      if (codec == owt::base::VideoCodec::kVp9) {
        if (platform_code < MFX_PLATFORM_KABYLAKE)
          continue;

        memset(&video_param, 0, sizeof(video_param));
        video_param.mfx.CodecId = MFX_CODEC_VP9;

        sts = mfx_decoder_->Query(nullptr, &video_param);
        if (sts == MFX_ERR_NONE) {
          VideoDecoderCapability vp9_cap;
          vp9_cap.codec_type = owt::base::VideoCodec::kVp9;
          vp9_cap.hardware_accelerated = true;
          vp9_cap.max_resolution = {7680, 4320};
          // Starting from KBL we support both 8-bit and 10-bit,so
          // not speficying profiles here. BXT/APL only supports
          // 8-bit but not enabled for Windows SDK.
          capabilities.push_back(vp9_cap);
        }
      } else if (codec == owt::base::VideoCodec::kH264) {
        memset(&video_param, 0, sizeof(video_param));
        video_param.mfx.CodecId = MFX_CODEC_AVC;

        sts = mfx_decoder_->Query(nullptr, &video_param);
        if (sts == MFX_ERR_NONE) {
          VideoDecoderCapability avc_cap;
          avc_cap.codec_type = owt::base::VideoCodec::kH264;
          avc_cap.hardware_accelerated = true;
          avc_cap.max_resolution = {3840, 2160};
          capabilities.push_back(avc_cap);
        }
      }
      else if (codec == owt::base::VideoCodec::kAv1) {
        // Disallow potential AV1 SW decoder.
#if (MFX_VERSION < 1031)
        continue;
#else
        if (platform_code < MFX_PLATFORM_TIGERLAKE)
          continue;
        else {
          memset(&video_param, 0, sizeof(video_param));
          video_param.mfx.CodecId = MFX_CODEC_AV1;

          sts = mfx_decoder_->Query(nullptr, &video_param);
          if (sts == MFX_ERR_NONE) {
            VideoDecoderCapability av1_cap;
            av1_cap.codec_type = owt::base::VideoCodec::kAv1;
            av1_cap.hardware_accelerated = true;
            av1_cap.max_resolution = {7680, 4320};
            // We support all 3 profiles so not specifying them here.
            capabilities.push_back(av1_cap);
          }
        }
#endif
      } else if (codec == owt::base::VideoCodec::kVp8) {
        memset(&video_param, 0, sizeof(video_param));
        video_param.mfx.CodecId = MFX_CODEC_VP8;

        sts = mfx_decoder_->Query(nullptr, &video_param);
        if (sts == MFX_ERR_NONE) {
          // Consider removing this from supported list?
          VideoDecoderCapability vp8_cap;
          vp8_cap.codec_type = owt::base::VideoCodec::kVp8;
          vp8_cap.hardware_accelerated = true;
          // Starting from KBL we support both 8-bit and 10-bit, so
          // not specifying profiles here.
          vp8_cap.max_resolution = {3840, 2160};
          capabilities.push_back(vp8_cap);
        }
      }
    }
  }

  return capabilities;
}

MediaCapabilities* MediaCapabilities::Get() {
  std::lock_guard<std::mutex> lock(get_singleton_mutex_);

  if (singleton_ == nullptr) {
    singleton_ = new MediaCapabilities();

    if (singleton_ && !singleton_->Init()) {
      delete singleton_;
      singleton_ = nullptr;
    }
  }

  return singleton_;
}

MediaCapabilities::MediaCapabilities() {}

MediaCapabilities::~MediaCapabilities() {

  if (mfx_encoder_) {
    mfx_encoder_->Close();
    mfx_encoder_.reset();
  }
  if (mfx_decoder_) {
    mfx_decoder_->Close();
    mfx_decoder_.reset();
  }
  if (msdk_factory_ && mfx_session_) {
    msdk_factory_->DestroySession(mfx_session_);
  }

}

bool MediaCapabilities::Init() {

  bool res = false;
  msdk_factory_ = owt::base::MSDKFactory::Get();
  if (!msdk_factory_)
    goto failed;

  mfx_session_ = msdk_factory_->CreateSession();
  if (!mfx_session_)
    goto failed;

  // Create the underlying MFXVideoDECODE and MFXVideoENCODE
  // instances.
  mfx_encoder_.reset(new MFXVideoENCODE(*mfx_session_));
  if (!mfx_encoder_)
    goto failed;

  mfx_decoder_.reset(new MFXVideoDECODE(*mfx_session_));
  if (!mfx_decoder_)
    goto failed;

  res = msdk_factory_->QueryPlatform(mfx_session_, &mfx_platform_);
  if (res)
    inited_ = true;
failed:
  return res;

}



}  // namespace base
}  // namespace owt
