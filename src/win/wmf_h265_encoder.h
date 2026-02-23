// Copyright 2024 libwebrtc project authors. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIB_WEBRTC_WIN_WMF_H265_ENCODER_H_
#define LIB_WEBRTC_WIN_WMF_H265_ENCODER_H_

#include <d3d11.h>
#include <mfapi.h>
#include <mferror.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <wrl/client.h>

#include <memory>
#include <vector>

#include "api/video/video_frame.h"
#include "api/video_codecs/video_codec.h"
#include "api/video_codecs/video_encoder.h"
#include "common_video/h265/h265_bitstream_parser.h"
#include "rtc_base/synchronization/mutex.h"

namespace webrtc {

class WmfH265Encoder : public VideoEncoder {
 public:
  explicit WmfH265Encoder(const VideoCodec& codec);
  ~WmfH265Encoder() override;

  static std::unique_ptr<WmfH265Encoder> Create(const VideoCodec& codec);
  static bool IsSupported();

  int InitEncode(const VideoCodec* codec_settings,
                 const Settings& settings) override;
  int Encode(const VideoFrame& input_image,
             const std::vector<VideoFrameType>* frame_types) override;
  int RegisterEncodeCompleteCallback(EncodedImageCallback* callback) override;
  void SetRates(const RateControlParameters& parameters) override;
  EncoderInfo GetEncoderInfo() const override;
  int Release() override;

 private:
  HRESULT InitMediaFoundation();
  HRESULT CreateHEVCEncoder();
  HRESULT ConfigureInputType();
  HRESULT ConfigureOutputType();
  HRESULT ConfigureRateControl();
  HRESULT ProcessInput(const VideoFrame& frame);
  HRESULT ProcessOutput(int64_t timestamp_us, int64_t ntp_time_ms,
                        uint32_t rtp_timestamp);
  int32_t NextNaluPosition(uint8_t* buffer, size_t buffer_size,
                           uint8_t* sc_length);

  EncodedImageCallback* callback_ = nullptr;
  VideoCodec codec_settings_;
  int32_t width_ = 0;
  int32_t height_ = 0;
  uint32_t target_bitrate_bps_ = 0;
  uint32_t max_bitrate_bps_ = 0;
  uint32_t frame_rate_ = 30;
  bool initialized_ = false;
  bool mf_started_ = false;
  bool com_initialized_ = false;

  Microsoft::WRL::ComPtr<IMFTransform> encoder_;
  Microsoft::WRL::ComPtr<IMFMediaType> input_type_;
  Microsoft::WRL::ComPtr<IMFMediaType> output_type_;

  DWORD input_stream_id_ = 0;
  DWORD output_stream_id_ = 0;

  std::vector<uint8_t> nv12_buffer_;
  H265BitstreamParser bitstream_parser_;
};

}  // namespace webrtc

#endif  // LIB_WEBRTC_WIN_WMF_H265_ENCODER_H_
