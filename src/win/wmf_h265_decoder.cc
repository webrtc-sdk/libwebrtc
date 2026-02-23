// Copyright 2024 libwebrtc project authors. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause

#include "src/win/wmf_h265_decoder.h"

#include <mftransform.h>

#include <algorithm>

#include "api/video/i420_buffer.h"
#include "api/video/video_frame_buffer.h"
#include "media/base/media_constants.h"
#include "modules/video_coding/include/video_error_codes.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/string_utils.h"
#include "third_party/libyuv/include/libyuv/convert.h"

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mf.lib")

namespace webrtc {

WmfH265Decoder::WmfH265Decoder() {}

WmfH265Decoder::~WmfH265Decoder() {
  Release();
}

std::unique_ptr<WmfH265Decoder> WmfH265Decoder::Create() {
  if (!IsSupported()) {
    RTC_LOG(LS_ERROR) << "WMF H.265 decoder is not supported on this system.";
    return nullptr;
  }
  return std::make_unique<WmfH265Decoder>();
}

bool WmfH265Decoder::IsSupported() {
  // Ensure COM is initialized on this thread (MF requires it)
  HRESULT com_hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
  bool should_uninit_com = SUCCEEDED(com_hr);

  HRESULT hr = MFStartup(MF_VERSION);
  if (FAILED(hr)) {
    if (should_uninit_com) CoUninitialize();
    return false;
  }

  // Enumerate HEVC decoders
  MFT_REGISTER_TYPE_INFO input_type = {MFMediaType_Video,
                                       MFVideoFormat_HEVC};
  IMFActivate** activate = nullptr;
  UINT32 count = 0;

  hr = MFTEnumEx(MFT_CATEGORY_VIDEO_DECODER,
                 MFT_ENUM_FLAG_SYNCMFT | MFT_ENUM_FLAG_ASYNCMFT |
                     MFT_ENUM_FLAG_HARDWARE | MFT_ENUM_FLAG_SORTANDFILTER,
                 &input_type, nullptr, &activate, &count);

  bool supported = SUCCEEDED(hr) && count > 0;

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

HRESULT WmfH265Decoder::InitMediaFoundation() {
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

HRESULT WmfH265Decoder::CreateHEVCDecoder() {
  MFT_REGISTER_TYPE_INFO input_type = {MFMediaType_Video,
                                       MFVideoFormat_HEVC};
  IMFActivate** activate = nullptr;
  UINT32 count = 0;

  HRESULT hr = MFTEnumEx(
      MFT_CATEGORY_VIDEO_DECODER,
      MFT_ENUM_FLAG_SYNCMFT | MFT_ENUM_FLAG_ASYNCMFT |
          MFT_ENUM_FLAG_HARDWARE | MFT_ENUM_FLAG_SORTANDFILTER,
      &input_type, nullptr, &activate, &count);

  if (FAILED(hr) || count == 0) {
    RTC_LOG(LS_ERROR) << "No HEVC decoder MFT found. hr=" << static_cast<int>(hr);
    if (activate) CoTaskMemFree(activate);
    return E_FAIL;
  }

  // Log available decoders
  for (UINT32 i = 0; i < count; i++) {
    LPWSTR friendly_name = nullptr;
    UINT32 name_len = 0;
    activate[i]->GetAllocatedString(MFT_FRIENDLY_NAME_Attribute,
                                    &friendly_name, &name_len);
    if (friendly_name) {
      RTC_LOG(LS_INFO) << "Found HEVC decoder: "
                       << webrtc::ToUtf8(friendly_name, name_len);
      CoTaskMemFree(friendly_name);
    }
  }

  // Activate the first (highest priority) decoder
  hr = activate[0]->ActivateObject(IID_PPV_ARGS(&decoder_));

  for (UINT32 i = 0; i < count; i++) {
    activate[i]->Release();
  }
  CoTaskMemFree(activate);

  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR) << "Failed to activate HEVC decoder MFT: " << static_cast<int>(hr);
    return hr;
  }

  // Set low-latency mode if supported
  Microsoft::WRL::ComPtr<IMFAttributes> attributes;
  hr = decoder_->GetAttributes(&attributes);
  if (SUCCEEDED(hr)) {
    attributes->SetUINT32(MF_LOW_LATENCY, TRUE);
  }

  RTC_LOG(LS_INFO) << "HEVC decoder MFT activated successfully.";
  return S_OK;
}

HRESULT WmfH265Decoder::ConfigureInputType() {
  HRESULT hr = MFCreateMediaType(&input_type_);
  if (FAILED(hr)) return hr;

  input_type_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
  input_type_->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_HEVC);

  if (width_ > 0 && height_ > 0) {
    MFSetAttributeSize(input_type_.Get(), MF_MT_FRAME_SIZE, width_, height_);
  }

  hr = decoder_->SetInputType(input_stream_id_, input_type_.Get(), 0);
  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR) << "SetInputType failed: " << static_cast<int>(hr);
    return hr;
  }

  return S_OK;
}

HRESULT WmfH265Decoder::ConfigureOutputType() {
  // Find NV12 output type
  DWORD type_index = 0;
  bool found_nv12 = false;

  while (true) {
    Microsoft::WRL::ComPtr<IMFMediaType> available_type;
    HRESULT hr = decoder_->GetOutputAvailableType(output_stream_id_,
                                                   type_index, &available_type);
    if (FAILED(hr)) break;

    GUID subtype;
    hr = available_type->GetGUID(MF_MT_SUBTYPE, &subtype);
    if (SUCCEEDED(hr) && (subtype == MFVideoFormat_NV12 ||
                          subtype == MFVideoFormat_I420 ||
                          subtype == MFVideoFormat_IYUV)) {
      output_type_ = available_type;
      found_nv12 = true;

      hr = decoder_->SetOutputType(output_stream_id_, output_type_.Get(), 0);
      if (SUCCEEDED(hr)) {
        RTC_LOG(LS_INFO) << "HEVC decoder output type set (type index "
                         << type_index << ")";
        break;
      }
    }
    type_index++;
  }

  if (!found_nv12) {
    RTC_LOG(LS_ERROR) << "Could not find suitable output type for HEVC decoder";
    return E_FAIL;
  }

  return S_OK;
}

bool WmfH265Decoder::Configure(const Settings& settings) {
  Release();

  width_ = settings.codec_type() == kVideoCodecH265 ? settings.max_render_resolution().Width() : 0;
  height_ = settings.codec_type() == kVideoCodecH265 ? settings.max_render_resolution().Height() : 0;

  HRESULT hr = InitMediaFoundation();
  if (FAILED(hr)) return false;

  hr = CreateHEVCDecoder();
  if (FAILED(hr)) return false;

  // Get stream IDs
  DWORD input_count = 0, output_count = 0;
  hr = decoder_->GetStreamCount(&input_count, &output_count);
  if (SUCCEEDED(hr) && input_count > 0 && output_count > 0) {
    std::vector<DWORD> input_ids(input_count), output_ids(output_count);
    hr = decoder_->GetStreamIDs(input_count, input_ids.data(), output_count,
                                output_ids.data());
    if (SUCCEEDED(hr)) {
      input_stream_id_ = input_ids[0];
      output_stream_id_ = output_ids[0];
    } else {
      input_stream_id_ = 0;
      output_stream_id_ = 0;
    }
  }

  hr = ConfigureInputType();
  if (FAILED(hr)) return false;

  hr = ConfigureOutputType();
  if (FAILED(hr)) return false;

  hr = decoder_->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, 0);
  hr = decoder_->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0);
  hr = decoder_->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, 0);

  initialized_ = true;
  RTC_LOG(LS_INFO) << "WMF H.265 decoder initialized.";
  return true;
}

HRESULT WmfH265Decoder::ProcessInput(const uint8_t* data, size_t size,
                                      int64_t timestamp_us) {
  Microsoft::WRL::ComPtr<IMFSample> sample;
  HRESULT hr = MFCreateSample(&sample);
  if (FAILED(hr)) return hr;

  Microsoft::WRL::ComPtr<IMFMediaBuffer> buffer;
  hr = MFCreateMemoryBuffer(static_cast<DWORD>(size), &buffer);
  if (FAILED(hr)) return hr;

  BYTE* buffer_data = nullptr;
  hr = buffer->Lock(&buffer_data, nullptr, nullptr);
  if (FAILED(hr)) return hr;

  memcpy(buffer_data, data, size);

  hr = buffer->Unlock();
  if (FAILED(hr)) return hr;

  hr = buffer->SetCurrentLength(static_cast<DWORD>(size));
  if (FAILED(hr)) return hr;

  hr = sample->AddBuffer(buffer.Get());
  if (FAILED(hr)) return hr;

  // Set sample time in 100ns units
  LONGLONG sample_time = timestamp_us * 10;
  sample->SetSampleTime(sample_time);

  hr = decoder_->ProcessInput(input_stream_id_, sample.Get(), 0);
  return hr;
}

HRESULT WmfH265Decoder::ProcessOutput(int64_t timestamp_us,
                                       int64_t ntp_time_ms,
                                       uint32_t rtp_timestamp) {
  MFT_OUTPUT_DATA_BUFFER output_data = {};
  output_data.dwStreamID = output_stream_id_;

  MFT_OUTPUT_STREAM_INFO stream_info = {};
  HRESULT hr = decoder_->GetOutputStreamInfo(output_stream_id_, &stream_info);
  if (FAILED(hr)) return hr;

  bool provides_samples =
      (stream_info.dwFlags & MFT_OUTPUT_STREAM_PROVIDES_SAMPLES);

  Microsoft::WRL::ComPtr<IMFSample> output_sample;
  if (!provides_samples) {
    hr = MFCreateSample(&output_sample);
    if (FAILED(hr)) return hr;

    Microsoft::WRL::ComPtr<IMFMediaBuffer> output_buffer;
    DWORD buffer_size = std::max(stream_info.cbSize,
        static_cast<DWORD>((width_ > 0 ? width_ : 1920) *
                           (height_ > 0 ? height_ : 1080) * 3));
    hr = MFCreateMemoryBuffer(buffer_size, &output_buffer);
    if (FAILED(hr)) return hr;

    hr = output_sample->AddBuffer(output_buffer.Get());
    if (FAILED(hr)) return hr;

    output_data.pSample = output_sample.Get();
  }

  DWORD status = 0;
  hr = decoder_->ProcessOutput(0, 1, &output_data, &status);

  if (hr == MF_E_TRANSFORM_NEED_MORE_INPUT) {
    return hr;
  }

  if (hr == MF_E_TRANSFORM_STREAM_CHANGE) {
    // Output format changed, reconfigure
    ConfigureOutputType();
    // Update cached dimensions from new output type
    if (output_type_) {
      UINT32 new_w = 0, new_h = 0;
      MFGetAttributeSize(output_type_.Get(), MF_MT_FRAME_SIZE, &new_w, &new_h);
      if (new_w > 0 && new_h > 0) {
        width_ = static_cast<int32_t>(new_w);
        height_ = static_cast<int32_t>(new_h);
      }
    }
    if (output_data.pEvents) output_data.pEvents->Release();
    return hr;
  }

  if (FAILED(hr)) {
    if (output_data.pEvents) output_data.pEvents->Release();
    return hr;
  }

  IMFSample* result_sample =
      provides_samples ? output_data.pSample : output_sample.Get();
  if (!result_sample) {
    if (output_data.pEvents) output_data.pEvents->Release();
    return E_FAIL;
  }

  // Get actual frame dimensions from output type
  UINT32 out_width = 0, out_height = 0;
  if (output_type_) {
    MFGetAttributeSize(output_type_.Get(), MF_MT_FRAME_SIZE, &out_width,
                       &out_height);
  }
  if (out_width == 0 || out_height == 0) {
    out_width = width_ > 0 ? width_ : 1920;
    out_height = height_ > 0 ? height_ : 1080;
  }

  // Extract decoded frame
  Microsoft::WRL::ComPtr<IMFMediaBuffer> result_buffer;
  hr = result_sample->ConvertToContiguousBuffer(&result_buffer);
  if (FAILED(hr)) {
    if (provides_samples && output_data.pSample)
      output_data.pSample->Release();
    if (output_data.pEvents) output_data.pEvents->Release();
    return hr;
  }

  BYTE* data = nullptr;
  DWORD data_length = 0;
  hr = result_buffer->Lock(&data, nullptr, &data_length);
  if (FAILED(hr)) {
    if (provides_samples && output_data.pSample)
      output_data.pSample->Release();
    if (output_data.pEvents) output_data.pEvents->Release();
    return hr;
  }

  // Get stride info
  LONG stride = out_width;
  if (output_type_) {
    UINT32 default_stride = 0;
    HRESULT stride_hr =
        output_type_->GetUINT32(MF_MT_DEFAULT_STRIDE, &default_stride);
    if (SUCCEEDED(stride_hr)) {
      stride = static_cast<LONG>(default_stride);
    }
  }

  // Check output subtype
  GUID out_subtype = MFVideoFormat_NV12;
  if (output_type_) {
    output_type_->GetGUID(MF_MT_SUBTYPE, &out_subtype);
  }

  // Convert to I420
  rtc::scoped_refptr<I420Buffer> i420_buffer =
      I420Buffer::Create(out_width, out_height);

  if (out_subtype == MFVideoFormat_NV12) {
    libyuv::NV12ToI420(data, stride,
                       data + stride * out_height, stride,
                       i420_buffer->MutableDataY(), i420_buffer->StrideY(),
                       i420_buffer->MutableDataU(), i420_buffer->StrideU(),
                       i420_buffer->MutableDataV(), i420_buffer->StrideV(),
                       out_width, out_height);
  } else {
    // Assume I420/IYUV
    int y_size = stride * out_height;
    int uv_stride = (stride + 1) / 2;
    int uv_size = uv_stride * ((out_height + 1) / 2);
    libyuv::I420Copy(data, stride, data + y_size, uv_stride,
                     data + y_size + uv_size, uv_stride,
                     i420_buffer->MutableDataY(), i420_buffer->StrideY(),
                     i420_buffer->MutableDataU(), i420_buffer->StrideU(),
                     i420_buffer->MutableDataV(), i420_buffer->StrideV(),
                     out_width, out_height);
  }

  result_buffer->Unlock();

  if (provides_samples && output_data.pSample) {
    output_data.pSample->Release();
  }
  if (output_data.pEvents) {
    output_data.pEvents->Release();
  }

  // Deliver decoded frame
  if (callback_) {
    VideoFrame decoded_frame =
        VideoFrame::Builder()
            .set_video_frame_buffer(i420_buffer)
            .set_timestamp_rtp(rtp_timestamp)
            .set_ntp_time_ms(ntp_time_ms)
            .set_timestamp_us(timestamp_us)
            .build();

    callback_->Decoded(decoded_frame);
  }

  return S_OK;
}

int32_t WmfH265Decoder::Decode(const EncodedImage& input_image,
                                bool missing_frames,
                                int64_t render_time_ms) {
  if (!initialized_ || !callback_) {
    return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
  }

  if (input_image.data() == nullptr || input_image.size() == 0) {
    return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
  }

  int64_t timestamp_us =
      static_cast<int64_t>(input_image.RtpTimestamp()) * 1000 / 90;

  HRESULT hr = ProcessInput(input_image.data(), input_image.size(),
                            timestamp_us);
  if (FAILED(hr)) {
    RTC_LOG(LS_ERROR) << "Decode ProcessInput failed: " << static_cast<int>(hr);
    return WEBRTC_VIDEO_CODEC_ERROR;
  }

  // Drain all available output
  while (true) {
    hr = ProcessOutput(timestamp_us, input_image.ntp_time_ms_,
                       input_image.RtpTimestamp());
    if (hr == MF_E_TRANSFORM_NEED_MORE_INPUT) {
      break;
    }
    if (hr == MF_E_TRANSFORM_STREAM_CHANGE) {
      continue;  // Retry with new output type
    }
    if (FAILED(hr)) {
      break;
    }
  }

  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t WmfH265Decoder::RegisterDecodeCompleteCallback(
    DecodedImageCallback* callback) {
  callback_ = callback;
  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t WmfH265Decoder::Release() {
  if (decoder_) {
    decoder_->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, 0);
    decoder_->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, 0);
    decoder_.Reset();
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

const char* WmfH265Decoder::ImplementationName() const {
  return "WMF_H265";
}

VideoDecoder::DecoderInfo WmfH265Decoder::GetDecoderInfo() const {
  DecoderInfo info;
  info.implementation_name = "WMF_H265";
  info.is_hardware_accelerated = true;
  return info;
}

}  // namespace webrtc
