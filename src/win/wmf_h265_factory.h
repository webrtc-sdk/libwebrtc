// Copyright 2024 libwebrtc project authors. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIB_WEBRTC_WIN_WMF_H265_FACTORY_H_
#define LIB_WEBRTC_WIN_WMF_H265_FACTORY_H_

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "api/video_codecs/sdp_video_format.h"
#include "api/video_codecs/video_decoder.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_encoder.h"
#include "api/video_codecs/video_encoder_factory.h"

namespace webrtc {

// Creates a video encoder factory that adds H.265 hardware encoding
// (via Windows Media Foundation) on top of the builtin encoder factory.
std::unique_ptr<VideoEncoderFactory>
CreateWmfH265EncoderFactory(std::unique_ptr<VideoEncoderFactory> builtin);

// Creates a video decoder factory that adds H.265 hardware decoding
// (via Windows Media Foundation) on top of the builtin decoder factory.
std::unique_ptr<VideoDecoderFactory>
CreateWmfH265DecoderFactory(std::unique_ptr<VideoDecoderFactory> builtin);

}  // namespace webrtc

#endif  // LIB_WEBRTC_WIN_WMF_H265_FACTORY_H_
