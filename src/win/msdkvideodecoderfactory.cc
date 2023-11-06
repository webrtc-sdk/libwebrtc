// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#include "src/win/msdkvideodecoderfactory.h"

#include <vector>

#include "absl/strings/match.h"
#include "media/base/codec.h"
#include "modules/video_coding/codecs/av1/libaom_av1_decoder.h"
#include "modules/video_coding/codecs/h264/include/h264.h"
#include "modules/video_coding/codecs/vp8/include/vp8.h"
#include "modules/video_coding/codecs/vp9/include/vp9.h"
#include "rtc_base/checks.h"
#include "src/win/codecutils.h"
#include "src/win/msdkvideodecoder.h"

namespace owt {
namespace base {

MSDKVideoDecoderFactory::MSDKVideoDecoderFactory() {
  supported_codec_types_.clear();

  bool is_vp8_hw_supported = false, is_vp9_hw_supported = false;
  bool is_h264_hw_supported = false, is_av1_hw_supported = false;
  MediaCapabilities* media_capability = MediaCapabilities::Get();
  std::vector<owt::base::VideoCodec> codecs_to_check;
  codecs_to_check.push_back(owt::base::VideoCodec::kH264);
  codecs_to_check.push_back(owt::base::VideoCodec::kVp9);
  codecs_to_check.push_back(owt::base::VideoCodec::kAv1);
  codecs_to_check.push_back(owt::base::VideoCodec::kVp8);
  std::vector<VideoDecoderCapability> capabilities =
      media_capability->SupportedCapabilitiesForVideoDecoder(codecs_to_check);

  for (auto& capability : capabilities) {
    if (capability.codec_type == owt::base::VideoCodec::kH264 &&
        !is_h264_hw_supported) {
      is_h264_hw_supported = true;
      supported_codec_types_.push_back(webrtc::kVideoCodecH264);
    } else if (capability.codec_type == owt::base::VideoCodec::kVp9 &&
               !is_vp9_hw_supported) {
      is_vp9_hw_supported = true;
      supported_codec_types_.push_back(webrtc::kVideoCodecVP9);
    } else if (capability.codec_type == owt::base::VideoCodec::kVp8 &&
               !is_vp8_hw_supported) {
      is_vp8_hw_supported = true;
      supported_codec_types_.push_back(webrtc::kVideoCodecVP8);
    } else if (capability.codec_type == owt::base::VideoCodec::kAv1 &&
               !is_av1_hw_supported) {
      is_av1_hw_supported = true;
      supported_codec_types_.push_back(webrtc::kVideoCodecAV1);
    }
  }
}

MSDKVideoDecoderFactory::~MSDKVideoDecoderFactory() {}

std::unique_ptr<webrtc::VideoDecoder>
MSDKVideoDecoderFactory::CreateVideoDecoder(
    const webrtc::SdpVideoFormat& format) {
  bool vp9_hw = false, vp8_hw = false, av1_hw = false, h264_hw = false;
  for (auto& codec : supported_codec_types_) {
    if (codec == webrtc::kVideoCodecAV1)
      av1_hw = false;
    else if (codec == webrtc::kVideoCodecH264)
      h264_hw = true;
    else if (codec == webrtc::kVideoCodecVP8)
      vp8_hw = false;
    else if (codec == webrtc::kVideoCodecVP9)
      vp9_hw = false;
  }
  if (absl::EqualsIgnoreCase(format.name, cricket::kVp9CodecName) && !vp9_hw) {
    return webrtc::VP9Decoder::Create();
  } else if (absl::EqualsIgnoreCase(format.name, cricket::kVp8CodecName) &&
             !vp8_hw) {
    RTC_LOG(LS_ERROR)
        << "Not supporting HW VP8 decoder. Requesting SW decoding.";
    return webrtc::VP8Decoder::Create();
  } else if (absl::EqualsIgnoreCase(format.name, cricket::kH264CodecName) &&
             !h264_hw) {
    return webrtc::H264Decoder::Create();
  } else if (absl::EqualsIgnoreCase(format.name, cricket::kAv1CodecName) &&
             !av1_hw) {
    return webrtc::CreateLibaomAv1Decoder();
  }

  return MSDKVideoDecoder::Create(cricket::VideoCodec(format));
}

std::vector<webrtc::SdpVideoFormat>
MSDKVideoDecoderFactory::GetSupportedFormats() const {
  std::vector<webrtc::SdpVideoFormat> supported_codecs;
  supported_codecs.push_back(webrtc::SdpVideoFormat(cricket::kVp8CodecName));
  for (const webrtc::SdpVideoFormat& format : webrtc::SupportedVP9Codecs())
    supported_codecs.push_back(format);
  for (const webrtc::SdpVideoFormat& format :
       owt::base::CodecUtils::SupportedH264Codecs())
    supported_codecs.push_back(format);
  if (webrtc::kIsLibaomAv1DecoderSupported) {
    supported_codecs.push_back(webrtc::SdpVideoFormat(cricket::kAv1CodecName));
  }
  return supported_codecs;
}

}  // namespace base
}  // namespace owt
