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

#include <map>
#include <memory>
#include <string>
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
  HRESULT ProcessOutput();
  // Async (hardware) MFT helpers. Hardware HEVC encoders are asynchronous
  // MFTs: they must be unlocked and driven through their event queue instead
  // of the plain ProcessInput()/ProcessOutput() polling loop.
  HRESULT PumpEvent(bool wait);
  HRESULT EncodeAsync(const VideoFrame& frame);
  int32_t NextNaluPosition(uint8_t* buffer, size_t buffer_size,
                           uint8_t* sc_length);

  // Timing metadata for a frame handed to the MFT, keyed by the sample time
  // so that output samples can be matched back to their input frame.
  struct FrameTiming {
    int64_t timestamp_us = 0;
    int64_t ntp_time_ms = 0;
    uint32_t rtp_timestamp = 0;
  };

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
  Microsoft::WRL::ComPtr<IMFMediaEventGenerator> event_generator_;
  Microsoft::WRL::ComPtr<IMFMediaType> input_type_;
  Microsoft::WRL::ComPtr<IMFMediaType> output_type_;

  // True when the activated MFT is an asynchronous (hardware) transform.
  bool is_async_ = false;
  // Number of METransformNeedInput events received but not yet consumed.
  int pending_input_requests_ = 0;
  std::map<LONGLONG, FrameTiming> frame_timing_;
  FrameTiming last_timing_;

  DWORD input_stream_id_ = 0;
  DWORD output_stream_id_ = 0;

  std::vector<uint8_t> nv12_buffer_;
  H265BitstreamParser bitstream_parser_;
};

}  // namespace webrtc

#endif  // LIB_WEBRTC_WIN_WMF_H265_ENCODER_H_
