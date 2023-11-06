// Copyright (C) <2020> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef OWT_BASE_WIN_MEDIACAPABILITIES_H_
#define OWT_BASE_WIN_MEDIACAPABILITIES_H_

#include <mutex>
#include <vector>

#include "base_allocator.h"
#include "mfxvideo++.h"
#include "src/win/commontypes.h"
#include "src/win/mediautils.h"
#include "src/win/msdkvideobase.h"

// Utilty class for checking media codec hardware acceleration status
// on current platform. if MediaSDK is enabled, will use MediaSDK for
// the check. Otherwise(TODO) will use MediaFoundation.

namespace owt {
namespace base {

struct VideoEncoderCapability {
  owt::base::VideoCodec codec_type;
  bool has_trusted_rate_controller =
      true;  ///< When set to false, frame dropper is on.
  bool hardware_accelerated =
      false;  ///< Video encoding is hardware accelerated.
  bool discreted_graphics =
      false;              ///< Encoder runs on discreted or integrated graphics.
  bool low_power = true;  ///< Encoder runs on VDEnc.
  uint8_t max_temporal_layers = 1;  ///< Maximum supported temporal layers.
  uint8_t max_spatial_layers = 1;   ///< Maximum supported spatial layers.
  CodecSpecificInfoUnion codec_specific;
  std::vector<BRCMode> supported_brc_modes;  ///< BRC modes supported.
  std::vector<SamplingMode> sampling_modes;
};

struct VideoDecoderCapability {
  owt::base::VideoCodec codec_type;
  bool hardware_accelerated;
  // Use by decoder factory to generate a subset of
  // profiles supported for each codec.
  owt::base::Resolution max_resolution;
  CodecSpecificInfoUnion codec_specific;
};

class MediaCapabilities {
 public:
  ~MediaCapabilities();
  std::vector<VideoEncoderCapability> SupportedCapabilitiesForVideoEncoder(
      std::vector<owt::base::VideoCodec>& codec_types);
  std::vector<VideoDecoderCapability> SupportedCapabilitiesForVideoDecoder(
      std::vector<owt::base::VideoCodec>& codec_types);
  static MediaCapabilities* Get();

 private:
  bool Init();
  MediaCapabilities();
  static MediaCapabilities* singleton_;
  static std::mutex get_singleton_mutex_;
  MFXVideoSession* mfx_session_;
  std::unique_ptr<MFXVideoENCODE> mfx_encoder_;
  std::unique_ptr<MFXVideoDECODE> mfx_decoder_;
  mfxPlatform mfx_platform_;
  owt::base::MSDKFactory* msdk_factory_;
  bool inited_;
};
}  // namespace base
}  // namespace owt
#endif  // OWT_BASE_WIN_MEDIACAPABILITIES_H_
