// Copyright (C) <2019> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef OWT_BASE_CODECUTILS_H_
#define OWT_BASE_CODECUTILS_H_

#include <vector>

#include "api/video/video_codec_type.h"
#include "api/video_codecs/sdp_video_format.h"

namespace owt {
namespace base {
class CodecUtils {
 public:
  static std::vector<webrtc::SdpVideoFormat> SupportedH264Codecs();
  static webrtc::VideoCodecType ConvertSdpFormatToCodecType(
      webrtc::SdpVideoFormat format);
};
}  // namespace base
}  // namespace owt
#endif  // OWT_BASE_CODECUTILS_H_
