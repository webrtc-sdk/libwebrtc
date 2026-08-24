// Copyright 2024 libwebrtc project authors. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause

#include "src/win/wmf_h265_encoder.h"

#include <codecapi.h>
#include <mftransform.h>
#include <wmcodecdsp.h>

#include <algorithm>

#include "api/video/encoded_image.h"
#include "api/video/i420_buffer.h"
#include "api/video/video_codec_type.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "media/base/media_constants.h"
#include "modules/video_coding/include/video_codec_interface.h"
#include "modules/video_coding/include/video_error_codes.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/string_utils.h"
#include "third_party/libyuv/include/libyuv/convert.h"

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mf.lib")

namespace webrtc {

WmfH265Encoder::WmfH265Encoder(const VideoCodec& codec)
    : codec_settings_(codec) {}

WmfH265Encoder::~WmfH265Encoder() {
  Release();
}

std::unique_ptr<WmfH265Encoder> WmfH265Encoder::Create(
    const VideoCodec& codec) {
  if (!IsSupported()) {
    RTC_LOG(LS_ERROR) << "WMF H.265 encoder is not supported on this system.";
    return nullptr;
  }
  return std::make_unique<WmfH265Encoder>(codec);
}

bool WmfH265Encoder::IsSupported() {
  // Ensure COM is initialized on this thread (MF requires it)
  HRESULT com_hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
  bool should_uninit_com = SUCCEEDED(com_hr);

  // Check if we can activate a HEVC encoder MFT
  HRESULT hr = MFStartup(MF_VERSION);
  if (FAILED(hr)) {
    if (should_uninit_com) CoUninitialize();
    return false;
  }

  // Enumerate HEVC encoders
  MFT_REGISTER_TYPE_INFO output_type = {MFMediaType_Video,
                                        MFVideoFormat_HEVC};
  IMFActivate** activate = nullptr;
  UINT32 count = 0;

  hr = MFTEnumEx(MFT_CATEGORY_VIDEO_ENCODER,
                 MFT_ENUM_FLAG_SYNCMFT | MFT_ENUM_FLAG_ASYNCMFT |
                     MFT_ENUM_FLAG_HARDWARE | MFT_ENUM_FLAG_SORTANDFILTER,
                 nullptr, &output_type, &activate, &count);

  bool supported = SUCCEEDED(hr) && count > 0;

  // Clean up
  if (activate) {
    for (UINT32 i = 0; i < count; i++) {
      activate[i]->Release();
    }
    CoTaskMemFree(activate);
  }

  MFShutdown();
  if (should_uninit_com) CoUninitialize();
  return supported;
}

HRESULT WmfH265Encoder::InitMediaFoundation() {
  // Ensure COM is initialized on the calling thread
  HRESULT com_hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
  if (SUCCEEDED(com_hr)) {
    com_initialized_ = true;
  } else if (com_hr != RPC_E_CHANGED_MODE) {
    RTC_LOG(LS_ERROR) << "CoInitializeEx failed: " << static_cast<int>(com_hr);
    return com_hr;
  }

  HRESULT hr = MFStartup(MF_VERSION);
  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR) << "MFStartup failed: " << static_cast<int>(hr);
    if (com_initialized_) {
      CoUninitialize();
      com_initialized_ = false;
    }
    return hr;
  }
  mf_started_ = true;
  return S_OK;
}

HRESULT WmfH265Encoder::CreateHEVCEncoder() {
  // Enumerate and activate HEVC encoders (prefer hardware)
  MFT_REGISTER_TYPE_INFO output_type = {MFMediaType_Video,
                                        MFVideoFormat_HEVC};
  IMFActivate** activate = nullptr;
  UINT32 count = 0;

  HRESULT hr = MFTEnumEx(
      MFT_CATEGORY_VIDEO_ENCODER,
      MFT_ENUM_FLAG_SYNCMFT | MFT_ENUM_FLAG_ASYNCMFT |
          MFT_ENUM_FLAG_HARDWARE | MFT_ENUM_FLAG_SORTANDFILTER,
      nullptr, &output_type, &activate, &count);

  if (FAILED(hr) || count == 0) {
    RTC_LOG(LS_ERROR) << "No HEVC encoder MFT found. hr=" << static_cast<int>(hr);
    if (activate) CoTaskMemFree(activate);
    return E_FAIL;
  }

  // Log available encoders
  for (UINT32 i = 0; i < count; i++) {
    LPWSTR friendly_name = nullptr;
    UINT32 name_len = 0;
    activate[i]->GetAllocatedString(MFT_FRIENDLY_NAME_Attribute,
                                    &friendly_name, &name_len);
    if (friendly_name) {
      RTC_LOG(LS_INFO) << "Found HEVC encoder: "
                       << webrtc::ToUtf8(friendly_name, name_len);
      CoTaskMemFree(friendly_name);
    }
  }

  // Activate the first (highest priority) encoder
  hr = activate[0]->ActivateObject(IID_PPV_ARGS(&encoder_));

  for (UINT32 i = 0; i < count; i++) {
    activate[i]->Release();
  }
  CoTaskMemFree(activate);

  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR) << "Failed to activate HEVC encoder MFT: " << static_cast<int>(hr);
    return hr;
  }
  // Set low-latency mode for real-time encoding
  Microsoft::WRL::ComPtr<IMFAttributes> attributes;
  hr = encoder_->GetAttributes(&attributes);
  if (SUCCEEDED(hr)) {
    attributes->SetUINT32(MF_LOW_LATENCY, TRUE);
  }

  RTC_LOG(LS_INFO) << "HEVC encoder MFT activated successfully.";
  return S_OK;
}

HRESULT WmfH265Encoder::ConfigureRateControl() {
  Microsoft::WRL::ComPtr<ICodecAPI> codec_api;
  HRESULT hr = encoder_.As(&codec_api);
  if (FAILED(hr)) {
    RTC_LOG(LS_WARNING) << "ICodecAPI not available, skipping rate control setup.";
    return S_OK;  // Non-fatal: MFT will use its default
  }

  // Set CBR mode — required for WebRTC real-time streaming
  VARIANT var;
  VariantInit(&var);
  var.vt = VT_UI4;
  var.ulVal = eAVEncCommonRateControlMode_CBR;
  hr = codec_api->SetValue(&CODECAPI_AVEncCommonRateControlMode, &var);
  if (FAILED(hr)) {
    RTC_LOG(LS_WARNING) << "Failed to set CBR rate control: "
                        << static_cast<int>(hr);
  }

  return S_OK;
}

HRESULT WmfH265Encoder::ConfigureOutputType() {
  HRESULT hr = MFCreateMediaType(&output_type_);
  if (FAILED(hr)) return hr;

  output_type_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
  output_type_->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_HEVC);
  MFSetAttributeSize(output_type_.Get(), MF_MT_FRAME_SIZE, width_, height_);
  MFSetAttributeRatio(output_type_.Get(), MF_MT_FRAME_RATE, frame_rate_, 1);
  output_type_->SetUINT32(MF_MT_AVG_BITRATE, target_bitrate_bps_);
  output_type_->SetUINT32(MF_MT_INTERLACE_MODE,
                          MFVideoInterlace_Progressive);
  MFSetAttributeRatio(output_type_.Get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1);

  // Set HEVC profile to Main
  output_type_->SetUINT32(MF_MT_MPEG2_PROFILE, 1);  // Main profile
  output_type_->SetUINT32(MF_MT_MPEG2_LEVEL, 120);   // Level 4.0

  hr = encoder_->SetOutputType(output_stream_id_, output_type_.Get(), 0);
  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR) << "SetOutputType failed: " << static_cast<int>(hr);
    return hr;
  }

  return S_OK;
}

HRESULT WmfH265Encoder::ConfigureInputType() {
  HRESULT hr = MFCreateMediaType(&input_type_);
  if (FAILED(hr)) return hr;

  input_type_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
  input_type_->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);
  MFSetAttributeSize(input_type_.Get(), MF_MT_FRAME_SIZE, width_, height_);
  MFSetAttributeRatio(input_type_.Get(), MF_MT_FRAME_RATE, frame_rate_, 1);
  input_type_->SetUINT32(MF_MT_INTERLACE_MODE,
                         MFVideoInterlace_Progressive);
  MFSetAttributeRatio(input_type_.Get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1);

  hr = encoder_->SetInputType(input_stream_id_, input_type_.Get(), 0);
  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR) << "SetInputType failed: " << static_cast<int>(hr);
    return hr;
  }

  return S_OK;
}

int WmfH265Encoder::InitEncode(const VideoCodec* codec_settings,
                                const Settings& settings) {
  if (!codec_settings) {
    return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
  }

  Release();

  codec_settings_ = *codec_settings;
  width_ = codec_settings->width;
  height_ = codec_settings->height;
  target_bitrate_bps_ = codec_settings->startBitrate * 1000;
  max_bitrate_bps_ = codec_settings->maxBitrate * 1000;
  frame_rate_ = codec_settings->maxFramerate;
  if (frame_rate_ == 0) frame_rate_ = 30;

  HRESULT hr = InitMediaFoundation();
  if (FAILED(hr)) {
    return WEBRTC_VIDEO_CODEC_ERROR;
  }

  hr = CreateHEVCEncoder();
  if (FAILED(hr)) {
    return WEBRTC_VIDEO_CODEC_ERROR;
  }

  // Get stream IDs
  DWORD input_count = 0, output_count = 0;
  hr = encoder_->GetStreamCount(&input_count, &output_count);
  if (SUCCEEDED(hr) && input_count > 0 && output_count > 0) {
    // Try getting explicit IDs
    std::vector<DWORD> input_ids(input_count), output_ids(output_count);
    hr = encoder_->GetStreamIDs(input_count, input_ids.data(), output_count,
                                output_ids.data());
    if (SUCCEEDED(hr)) {
      input_stream_id_ = input_ids[0];
      output_stream_id_ = output_ids[0];
    } else {
      // If E_NOTIMPL, stream IDs are sequential starting from 0
      input_stream_id_ = 0;
      output_stream_id_ = 0;
    }
  }

  // Must set output type before input type for most MF encoders
  hr = ConfigureOutputType();
  if (FAILED(hr)) {
    return WEBRTC_VIDEO_CODEC_ERROR;
  }

  hr = ConfigureInputType();
  if (FAILED(hr)) {
    return WEBRTC_VIDEO_CODEC_ERROR;
  }

  // Set rate control after media types are configured
  hr = ConfigureRateControl();
  if (FAILED(hr)) {
    return WEBRTC_VIDEO_CODEC_ERROR;
  }

  // Pre-allocate NV12 conversion buffer
  nv12_buffer_.resize(width_ * height_ * 3 / 2);

  // Start processing
  hr = encoder_->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, 0);
  hr = encoder_->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0);
  hr = encoder_->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, 0);
  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR) << "Failed to start encoder streaming: " << static_cast<int>(hr);
    return WEBRTC_VIDEO_CODEC_ERROR;
  }

  initialized_ = true;
  RTC_LOG(LS_INFO) << "WMF H.265 encoder initialized: " << width_ << "x"
                   << height_ << " @ " << frame_rate_ << "fps, "
                   << target_bitrate_bps_ / 1000 << " kbps";

  return WEBRTC_VIDEO_CODEC_OK;
}

HRESULT WmfH265Encoder::ProcessInput(const VideoFrame& frame) {
  rtc::scoped_refptr<I420BufferInterface> i420_buffer =
      frame.video_frame_buffer()->ToI420();

  int y_stride = i420_buffer->StrideY();
  int u_stride = i420_buffer->StrideU();
  int v_stride = i420_buffer->StrideV();

  // Create NV12 buffer
  int nv12_size = width_ * height_ * 3 / 2;

  // Convert I420 to NV12 using pre-allocated buffer
  libyuv::I420ToNV12(
      i420_buffer->DataY(), y_stride, i420_buffer->DataU(), u_stride,
      i420_buffer->DataV(), v_stride, nv12_buffer_.data(), width_,
      nv12_buffer_.data() + width_ * height_, width_, width_, height_);

  // Create MF sample
  Microsoft::WRL::ComPtr<IMFSample> sample;
  HRESULT hr = MFCreateSample(&sample);
  if (FAILED(hr)) return hr;

  Microsoft::WRL::ComPtr<IMFMediaBuffer> buffer;
  hr = MFCreateMemoryBuffer(nv12_size, &buffer);
  if (FAILED(hr)) return hr;

  BYTE* buffer_data = nullptr;
  hr = buffer->Lock(&buffer_data, nullptr, nullptr);
  if (FAILED(hr)) return hr;

  memcpy(buffer_data, nv12_buffer_.data(), nv12_size);

  hr = buffer->Unlock();
  if (FAILED(hr)) return hr;

  hr = buffer->SetCurrentLength(nv12_size);
  if (FAILED(hr)) return hr;

  hr = sample->AddBuffer(buffer.Get());
  if (FAILED(hr)) return hr;

  // Set sample time (100-nanosecond units)
  LONGLONG sample_time = frame.timestamp_us() * 10;
  sample->SetSampleTime(sample_time);
  sample->SetSampleDuration(static_cast<LONGLONG>(10000000.0 / frame_rate_));

  hr = encoder_->ProcessInput(input_stream_id_, sample.Get(), 0);
  return hr;
}

int32_t WmfH265Encoder::NextNaluPosition(uint8_t* buffer,
                                          size_t buffer_size,
                                          uint8_t* sc_length) {
  if (buffer_size < 3) return -1;

  *sc_length = 0;
  for (size_t i = 0; i < buffer_size - 3; i++) {
    if (buffer[i] == 0 && buffer[i + 1] == 0) {
      if (buffer[i + 2] == 1) {
        *sc_length = 3;
        return static_cast<int32_t>(i);
      }
      if (i < buffer_size - 4 && buffer[i + 2] == 0 && buffer[i + 3] == 1) {
        *sc_length = 4;
        return static_cast<int32_t>(i);
      }
    }
  }
  return -1;
}

HRESULT WmfH265Encoder::ProcessOutput(int64_t timestamp_us,
                                       int64_t ntp_time_ms,
                                       uint32_t rtp_timestamp) {
  MFT_OUTPUT_DATA_BUFFER output_data = {};
  output_data.dwStreamID = output_stream_id_;

  // Check if we need to provide the output sample
  MFT_OUTPUT_STREAM_INFO stream_info = {};
  HRESULT hr = encoder_->GetOutputStreamInfo(output_stream_id_, &stream_info);
  if (FAILED(hr)) return hr;

  bool provides_samples =
      (stream_info.dwFlags & MFT_OUTPUT_STREAM_PROVIDES_SAMPLES);

  Microsoft::WRL::ComPtr<IMFSample> output_sample;
  if (!provides_samples) {
    hr = MFCreateSample(&output_sample);
    if (FAILED(hr)) return hr;

    Microsoft::WRL::ComPtr<IMFMediaBuffer> output_buffer;
    DWORD buffer_size =
        std::max(stream_info.cbSize, static_cast<DWORD>(width_ * height_ * 2));
    hr = MFCreateMemoryBuffer(buffer_size, &output_buffer);
    if (FAILED(hr)) return hr;

    hr = output_sample->AddBuffer(output_buffer.Get());
    if (FAILED(hr)) return hr;

    output_data.pSample = output_sample.Get();
  }

  DWORD status = 0;
  hr = encoder_->ProcessOutput(0, 1, &output_data, &status);

  if (hr == MF_E_TRANSFORM_NEED_MORE_INPUT) {
    return hr;  // Not an error, just need more input
  }

  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR) << "ProcessOutput failed: " << static_cast<int>(hr);
    if (output_data.pEvents) output_data.pEvents->Release();
    return hr;
  }

  IMFSample* result_sample =
      provides_samples ? output_data.pSample : output_sample.Get();
  if (!result_sample) {
    if (output_data.pEvents) output_data.pEvents->Release();
    return E_FAIL;
  }

  // Extract encoded data
  Microsoft::WRL::ComPtr<IMFMediaBuffer> result_buffer;
  hr = result_sample->ConvertToContiguousBuffer(&result_buffer);
  if (FAILED(hr)) {
    if (output_data.pEvents) output_data.pEvents->Release();
    return hr;
  }

  BYTE* data = nullptr;
  DWORD data_length = 0;
  hr = result_buffer->Lock(&data, nullptr, &data_length);
  if (FAILED(hr)) {
    if (output_data.pEvents) output_data.pEvents->Release();
    return hr;
  }

  if (data_length > 0 && callback_) {
    // Determine frame type by scanning NALUs
    bool is_keyframe = false;
    uint8_t sc_length = 0;
    size_t offset = 0;

    while (offset < data_length) {
      int nalu_pos =
          NextNaluPosition(data + offset, data_length - offset, &sc_length);
      if (nalu_pos < 0) break;

      size_t nalu_start = offset + nalu_pos + sc_length;
      if (nalu_start < data_length) {
        // H.265 NALU type is in bits 1-6 of the first byte
        uint8_t nalu_type = (data[nalu_start] >> 1) & 0x3F;
        // IRAP VCL NALUs only: BLA_W_LP(16) through RSV_IRAP_VCL23(23)
        // VPS(32)/SPS(33)/PPS(34) are non-VCL and should NOT mark keyframe
        if (nalu_type >= 16 && nalu_type <= 23) {
          is_keyframe = true;
        }
      }
      offset = nalu_start + 1;
    }

    // Create encoded image
    EncodedImage encoded_image;
    encoded_image.SetEncodedData(
        EncodedImageBuffer::Create(data, data_length));
    encoded_image._encodedWidth = width_;
    encoded_image._encodedHeight = height_;
    // Preserve the RTP timestamp from the upstream WebRTC frame
    // to avoid drift between encoder and RTP sender.
    encoded_image.SetRtpTimestamp(rtp_timestamp);
    encoded_image.capture_time_ms_ = timestamp_us / 1000;
    encoded_image.ntp_time_ms_ = ntp_time_ms;
    encoded_image._frameType =
        is_keyframe ? VideoFrameType::kVideoFrameKey
                    : VideoFrameType::kVideoFrameDelta;
    encoded_image.SetSpatialIndex(0);

    // Parse bitstream for QP
    bitstream_parser_.ParseBitstream(
        rtc::ArrayView<const uint8_t>(data, data_length));
    
    std::optional<int> qp = bitstream_parser_.GetLastSliceQp();
    if (qp.has_value()) {
      encoded_image.qp_ = *qp;
    } 
    CodecSpecificInfo codec_info;
    codec_info.codecType = kVideoCodecH265;

    callback_->OnEncodedImage(encoded_image, &codec_info);
  }

  result_buffer->Unlock();

  if (provides_samples && output_data.pSample) {
    output_data.pSample->Release();
  }
  if (output_data.pEvents) {
    output_data.pEvents->Release();
  }

  return S_OK;
}

int WmfH265Encoder::Encode(const VideoFrame& input_image,
                            const std::vector<VideoFrameType>* frame_types) {
  if (!initialized_ || !callback_) {
    return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
  }

  // Check for keyframe request
  bool force_key_frame = false;
  if (frame_types) {
    for (const auto& frame_type : *frame_types) {
      if (frame_type == VideoFrameType::kVideoFrameKey) {
        force_key_frame = true;
        break;
      }
    }
  }

  if (force_key_frame) {
    // Request IDR from MFT via CODECAPI
    Microsoft::WRL::ComPtr<ICodecAPI> codec_api;
    HRESULT hr = encoder_.As(&codec_api);
    if (SUCCEEDED(hr)) {
      VARIANT var;
      VariantInit(&var);
      var.vt = VT_UI4;
      var.ulVal = 1;
      codec_api->SetValue(&CODECAPI_AVEncVideoForceKeyFrame, &var);
    }
  }

  // Process input
  HRESULT hr = ProcessInput(input_image);
  if (hr == MF_E_NOTACCEPTING) {
    // MFT input buffer full — drain output first, then retry
    while (true) {
      HRESULT out_hr = ProcessOutput(input_image.timestamp_us(),
                                     input_image.ntp_time_ms(),
                                     input_image.rtp_timestamp());
      if (out_hr == MF_E_TRANSFORM_NEED_MORE_INPUT || FAILED(out_hr)) break;
    }
    hr = ProcessInput(input_image);
  }
  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR) << "ProcessInput failed: " << static_cast<int>(hr);
    return WEBRTC_VIDEO_CODEC_ERROR;
  }

  // Try to get output
  while (true) {
    hr = ProcessOutput(input_image.timestamp_us(), input_image.ntp_time_ms(),
                        input_image.rtp_timestamp());
    if (hr == MF_E_TRANSFORM_NEED_MORE_INPUT) {
      break;  // Need more input before getting output
    }
    if (FAILED(hr)) {
      break;
    }
  }

  return WEBRTC_VIDEO_CODEC_OK;
}

int WmfH265Encoder::RegisterEncodeCompleteCallback(
    EncodedImageCallback* callback) {
  callback_ = callback;
  return WEBRTC_VIDEO_CODEC_OK;
}

void WmfH265Encoder::SetRates(const RateControlParameters& parameters) {
  if (!initialized_ || !encoder_) return;

  target_bitrate_bps_ = parameters.bitrate.get_sum_bps();
  frame_rate_ = static_cast<uint32_t>(parameters.framerate_fps);
  if (frame_rate_ == 0) frame_rate_ = 30;

  // Update bitrate via CODECAPI
  Microsoft::WRL::ComPtr<ICodecAPI> codec_api;
  HRESULT hr = encoder_.As(&codec_api);
  if (SUCCEEDED(hr)) {
    VARIANT var;
    VariantInit(&var);
    var.vt = VT_UI4;
    var.ulVal = target_bitrate_bps_;
    codec_api->SetValue(&CODECAPI_AVEncCommonMeanBitRate, &var);
  }
}

VideoEncoder::EncoderInfo WmfH265Encoder::GetEncoderInfo() const {
  EncoderInfo info;
  info.implementation_name = "WMF_H265";
  info.is_hardware_accelerated = true;
  info.supports_native_handle = false;
  info.supports_simulcast = false;
  info.requested_resolution_alignment = 2;  // NV12 requires even dimensions
  return info;
}

int WmfH265Encoder::Release() {
  if (encoder_) {
    encoder_->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, 0);
    encoder_->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, 0);
    encoder_.Reset();
  }

  input_type_.Reset();
  output_type_.Reset();

  if (mf_started_) {
    MFShutdown();
    mf_started_ = false;
  }

  if (com_initialized_) {
    CoUninitialize();
    com_initialized_ = false;
  }

  initialized_ = false;
  return WEBRTC_VIDEO_CODEC_OK;
}

}  // namespace webrtc
