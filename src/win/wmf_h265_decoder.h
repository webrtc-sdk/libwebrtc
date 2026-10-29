// Copyright 2024 libwebrtc project authors. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIB_WEBRTC_WIN_WMF_H265_DECODER_H_
#define LIB_WEBRTC_WIN_WMF_H265_DECODER_H_

#include <d3d11.h>
#include <mfapi.h>
#include <mferror.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <wrl/client.h>

#include <memory>
#include <vector>

#include "api/video/i420_buffer.h"
#include "api/video/video_frame.h"
#include "api/video_codecs/video_codec.h"
#include "api/video_codecs/video_decoder.h"

namespace webrtc {

class WmfH265Decoder : public VideoDecoder {
 public:
  WmfH265Decoder();
  ~WmfH265Decoder() override;

  static std::unique_ptr<WmfH265Decoder> Create();
  static bool IsSupported();

  bool Configure(const Settings& settings) override;
  int32_t Decode(const EncodedImage& input_image, bool missing_frames,
                 int64_t render_time_ms) override;
  int32_t RegisterDecodeCompleteCallback(
      DecodedImageCallback* callback) override;
  int32_t Release() override;
  DecoderInfo GetDecoderInfo() const override;
  const char* ImplementationName() const override;

 private:
  HRESULT InitMediaFoundation();
  HRESULT CreateHEVCDecoder();
  HRESULT ConfigureInputType();
  HRESULT ConfigureOutputType();
  HRESULT ProcessInput(const uint8_t* data, size_t size, int64_t timestamp_us);
  HRESULT ProcessOutput(int64_t timestamp_us, int64_t ntp_time_ms,
                        uint32_t rtp_timestamp);

  DecodedImageCallback* callback_ = nullptr;
  bool initialized_ = false;
  bool mf_started_ = false;
  bool com_initialized_ = false;
  int32_t width_ = 0;
  int32_t height_ = 0;

  Microsoft::WRL::ComPtr<IMFTransform> decoder_;
  Microsoft::WRL::ComPtr<IMFMediaType> input_type_;
  Microsoft::WRL::ComPtr<IMFMediaType> output_type_;

  DWORD input_stream_id_ = 0;
  DWORD output_stream_id_ = 0;
};

}  // namespace webrtc

#endif  // LIB_WEBRTC_WIN_WMF_H265_DECODER_H_
