// Copyright 2024 libwebrtc project authors. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause

#include "src/win/wmf_h265_factory.h"

#include "api/environment/environment.h"
#include "api/video_codecs/sdp_video_format.h"
#include "media/base/media_constants.h"
#include "media/engine/simulcast_encoder_adapter.h"
#include "rtc_base/logging.h"
#include "src/win/wmf_h265_decoder.h"
#include "src/win/wmf_h265_encoder.h"

namespace webrtc {

namespace {

// H.265 SDP formats to advertise
std::vector<SdpVideoFormat> GetSupportedH265Formats() {
  return {SdpVideoFormat(kH265CodecName,
                         {{"profile-id", "1"},    // Main profile
                          {"tier-flag", "0"},      // Main tier
                          {"level-id", "120"}})};  // Level 4.0
}

// ------- Encoder factory -------

class WmfH265VideoEncoderFactory : public VideoEncoderFactory {
 public:
  explicit WmfH265VideoEncoderFactory(
      std::unique_ptr<VideoEncoderFactory> builtin)
      : builtin_(std::move(builtin)),
        h265_supported_(WmfH265Encoder::IsSupported()) {
    if (h265_supported_) {
      RTC_LOG(LS_INFO) << "WMF H.265 encoder is available.";
    } else {
      RTC_LOG(LS_WARNING) << "WMF H.265 encoder is NOT available.";
    }
  }

  std::unique_ptr<VideoEncoder> Create(const Environment& env,
                                       const SdpVideoFormat& format) override {
    if (h265_supported_ &&
        absl::EqualsIgnoreCase(format.name, kH265CodecName)) {
      VideoCodec codec;
      codec.codecType = kVideoCodecH265;
      auto encoder = WmfH265Encoder::Create(codec);
      if (encoder) {
        return encoder;
      }
      RTC_LOG(LS_WARNING)
          << "Failed to create WMF H.265 encoder, no fallback available.";
      return nullptr;
    }
    return builtin_->Create(env, format);
  }

  std::vector<SdpVideoFormat> GetSupportedFormats() const override {
    auto formats = builtin_->GetSupportedFormats();
    if (h265_supported_) {
      auto h265_formats = GetSupportedH265Formats();
      formats.insert(formats.end(), h265_formats.begin(),
                     h265_formats.end());
    }
    return formats;
  }

  CodecSupport QueryCodecSupport(
      const SdpVideoFormat& format,
      std::optional<std::string> scalability_mode) const override {
    if (h265_supported_ &&
        absl::EqualsIgnoreCase(format.name, kH265CodecName)) {
      return {.is_supported = true, .is_power_efficient = true};
    }
    return builtin_->QueryCodecSupport(format, scalability_mode);
  }

 private:
  std::unique_ptr<VideoEncoderFactory> builtin_;
  bool h265_supported_;
};

// ------- Decoder factory -------

class WmfH265VideoDecoderFactory : public VideoDecoderFactory {
 public:
  explicit WmfH265VideoDecoderFactory(
      std::unique_ptr<VideoDecoderFactory> builtin)
      : builtin_(std::move(builtin)),
        h265_supported_(WmfH265Decoder::IsSupported()) {
    if (h265_supported_) {
      RTC_LOG(LS_INFO) << "WMF H.265 decoder is available.";
    } else {
      RTC_LOG(LS_WARNING) << "WMF H.265 decoder is NOT available.";
    }
  }

  std::unique_ptr<VideoDecoder> Create(const Environment& env,
                                       const SdpVideoFormat& format) override {
    if (h265_supported_ &&
        absl::EqualsIgnoreCase(format.name, kH265CodecName)) {
      auto decoder = WmfH265Decoder::Create();
      if (decoder) {
        return decoder;
      }
      RTC_LOG(LS_WARNING)
          << "Failed to create WMF H.265 decoder, no fallback available.";
      return nullptr;
    }
    return builtin_->Create(env, format);
  }

  std::vector<SdpVideoFormat> GetSupportedFormats() const override {
    auto formats = builtin_->GetSupportedFormats();
    if (h265_supported_) {
      auto h265_formats = GetSupportedH265Formats();
      formats.insert(formats.end(), h265_formats.begin(),
                     h265_formats.end());
    }
    return formats;
  }

  CodecSupport QueryCodecSupport(
      const SdpVideoFormat& format,
      bool reference_scaling) const override {
    if (h265_supported_ &&
        absl::EqualsIgnoreCase(format.name, kH265CodecName)) {
      return {.is_supported = true, .is_power_efficient = true};
    }
    return builtin_->QueryCodecSupport(format, reference_scaling);
  }

 private:
  std::unique_ptr<VideoDecoderFactory> builtin_;
  bool h265_supported_;
};

}  // namespace

std::unique_ptr<VideoEncoderFactory> CreateWmfH265EncoderFactory(
    std::unique_ptr<VideoEncoderFactory> builtin) {
  return std::make_unique<WmfH265VideoEncoderFactory>(std::move(builtin));
}

std::unique_ptr<VideoDecoderFactory> CreateWmfH265DecoderFactory(
    std::unique_ptr<VideoDecoderFactory> builtin) {
  return std::make_unique<WmfH265VideoDecoderFactory>(std::move(builtin));
}

}  // namespace webrtc
