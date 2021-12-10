// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef OWT_BASE_MEDIAUTILS_H_
#define OWT_BASE_MEDIAUTILS_H_

#include "absl/types/optional.h"
#include "api/video_codecs/sdp_video_format.h"
#include <string>
#include "src/win/commontypes.h"

namespace owt {
namespace base {

/// AV1 profiles as negotiated in SDP profile field.
enum class AV1Profile : int {
  kMain = 0,     ///< 4:0:0(monochrome) or 4:2:0, 8/10-bit
  kHigh,         ///< 4:0:0/4:2:0/4:4:4, 8/10-bit
  kProfessional  ///< 4:0:0/4:2:0/4:2:2/4:4:4, 8/10/12-bit
};

/// AV1 levels as negotiated in SDP level-idx field.
/// Calculated by level(x.y)-> level-idx(z): z = (x - 2) * 4 + y;
enum class AV1Level : int {
  kLevel2 =
      0,  ///< 426x240@30fps. All 2.x profiles: max 8 tiles, 4 tile columns.
  kLevel2_1 = 1,  ///< 640x360@30fps
  kLevel3 =
      4,  ///< 854x480@30fps. All 3.x profiles: max 16 tiles, 6 tile columns.
  kLevel3_1 = 5,  ///< 720p@30fps. Max 16 tiles, 6 tile columns.
  kLevel4 = 8,  ///< 1080p@30fps. All 4.x profiles: max 32 tiles, 8 tile columns
  kLevel4_1 = 9,  ///< 1080p@60fps
  kLevel5 = 12,   ///< 4k@30fps. All 5.x profiles: max 64 tiles, 8 tile columns.
  kLevel5_1 = 13,  ///< 4k@60fps
  kLevel5_2 = 14,  ///< 4k@120fps
  kLevel5_3 = 15,  ///< 4k@120fps
  kLevel6 = 16,  ///< 8k@30fps. All 6.x profiles: max 128 tiles, 16 tile columns
  kLevel6_1 = 17,  ///< 8k@60fps
  kLevel6_2 = 18,  ///< 8k@120fps
  kLevel6_3 = 19,  ///< 8k@120fps
  kLevelReserved   ///< Levels not defined yet.
};

/// AV1 tiers as negotiated in SDP tier field.
/// This equals to the seq_tier accrording to AV1 spec. Value 1
/// is only allowed for level above 4.0(inclusive).
enum class AV1Tier : int { kTier0 = 0, kTier1 };

/// AVC profiles
enum class H264Profile : int {
  kConstrainedBaseline = 0,
  kBaseline,
  kMain,
  kHigh,
  kUnknown
};

/// VP9 profiles.
enum class VP9Profile : int {
  kProfile0 = 0,  ///< 4:2:0, 8-bit
  kProfile1,      ///< 4:2:2 or 4:4:4, 8-bit
  kProfile2,      ///< 4:2:0, 10-bit or 12-bit
  kProfile3       ///< 4:2:2 or 4:4:4, 10-bit or 12-bit
};

/// Profile space as negotiated in SDP profile-space field.
/// Default to 0. (Range from 0 to 3).
enum class H265ProfileSpace : int {
  KDefault = 0,
  kReserved1,
  kReserved2,
  kReserved3
};

/// Profile space as negotiated in SDP profile-id field.
/// Not all possible profiles are listed here.
enum class H265ProfileId : int {
  kUnknown = 0,
  kMain,
  kMain10,
  kMainStillPicture,
  kMainRExt,  // Range extension
  kScc  // Screen content extension.
};

/// Level ID as negotiated in SDP level-id field.
/// Calculated by: level(x.y) * 30
enum class H265Level : int {
  kUnknown = 0,
  kLevel1 = 30,
  kLevel2 = 60,
  kLevle2_1 = 63,
  kLevel3 = 90,
  kLevel3_1 = 93,
  kLevel4 = 120,
  kLevel4_1 = 123,
  kLevel5 = 150,
  kLevel5_1 = 153,
  kLevel5_2 = 156,
  kLevel6 = 180,
  kLevel6_1 = 183,
  kLevel6_2 = 186,
  kLevel8_5 = 255
};

/// Tier as negotiated in SDP tier-flag field.
enum class H265Tier : int {
  kMain = 0,
  kHigh,
};

enum class SamplingMode : int {
  kNv12 = 0,
  kARGB,
  kP010,  // Planar, 4:2:0 10-bit.
  kY410,  // Packed, 4:4:4 10-bit.
  kP016,  // Planar, 4:2:0 16-bit.
  kY416,  // Packed, 4:4:4 16-bit.
};

/// Encoder BRC mode.
enum class BRCMode : int {
  kUnknown = 0,
  kCBR = 1,
  kCQP = 2,
  kVBR = 3,
  kICQ = 4,
  kAVBR = 5,
  kVCM = 6,
  kExternal = 7
};

struct VP9CodecCapability {
  VP9Profile profile;
};

struct H264CodecCapability {
  H264Profile profile;
};

struct VP8CodecCapability {
};

struct AV1CodecCapability {
  AV1Profile profile;
};

struct H265CodecCapability {
  H265ProfileId profile;
};

union CodecSpecificInfoUnion {
  VP9CodecCapability VP9;
#ifndef DISABLE_H265
  H265CodecCapability H265;
#endif
  H264CodecCapability H264;
  AV1CodecCapability AV1;
  VP8CodecCapability VP8;
  ~CodecSpecificInfoUnion() {}
};


class MediaUtils {
 public:
  static std::string GetResolutionName(const Resolution& resolution);
  static std::string AudioCodecToString(const AudioCodec& audio_codec);
  static std::string VideoCodecToString(const VideoCodec& video_codec);
  static AudioCodec GetAudioCodecFromString(const std::string& codec_name);
  static VideoCodec GetVideoCodecFromString(const std::string& codec_name);
  static absl::optional<unsigned int> GetH264TemporalLayers();
  static bool GetH264TemporalInfo(uint8_t* buffer,
                                  size_t buffer_length,
                                  int& temporal_id,
                                  int& priority_id,
                                  bool& is_idr);
  static absl::optional<AV1Profile> ParseSdpForAV1Profile(
      const webrtc::SdpVideoFormat::Parameters& params);
  static absl::optional<H265ProfileId> ParseSdpForH265Profile(
      const webrtc::SdpVideoFormat::Parameters& params);
};
}
}
#endif  // OWT_BASE_MEDIAUTILS_H_
