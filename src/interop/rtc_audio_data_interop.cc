#include "rtc_audio_data.h"
#include "interop_api.h"

using namespace libwebrtc;

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioData_Create(
    unsigned int index,
    const unsigned char* data,
    unsigned int bits_per_sample,
    int sample_rate_hz,
    unsigned int num_channels,
    rtcAudioDataHandle* pOutRetVal
) noexcept
{
  CHECK_OUT_POINTER(pOutRetVal);
  
  scoped_refptr<RTCAudioData> p = RTCAudioData::Create(
    static_cast<uint32_t>(index),
    static_cast<const uint8_t*>(data),
    static_cast<uint32_t>(bits_per_sample),
    sample_rate_hz,
    static_cast<uint32_t>(num_channels)
  );
  *pOutRetVal = static_cast<rtcAudioDataHandle>(p.release());
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioData_GetIndex(
    rtcAudioDataHandle handle,
    unsigned int* pRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pRetVal, unsigned int, RTCAudioData, index);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioData_SetIndex(
    rtcAudioDataHandle handle,
    unsigned int index
) noexcept
{
  DECLARE_SET_VALUE(handle, index, uint32_t, RTCAudioData, set_index);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioData_GetData(
    rtcAudioDataHandle handle,
    const unsigned char** ppRetVal
) noexcept
{
  CHECK_POINTER(ppRetVal);
  CHECK_NATIVE_HANDLE(handle);

  scoped_refptr<RTCAudioData> p = static_cast<RTCAudioData*>(handle);
  *ppRetVal = static_cast<const unsigned char*>(p->data());
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioData_GetDataSize(
    rtcAudioDataHandle handle,
    int* pRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pRetVal, int, RTCAudioData, data_size);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioData_GetBitsPerSample(
    rtcAudioDataHandle handle,
    int* pRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pRetVal, int, RTCAudioData, bits_per_sample);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioData_GetSamplesPerChannel(
    rtcAudioDataHandle handle,
    int* pRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pRetVal, int, RTCAudioData, samples_per_channel);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioData_GetSampleRateHz(
    rtcAudioDataHandle handle,
    int* pRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pRetVal, int, RTCAudioData, sample_rate_hz);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioData_GetNumChannels(
    rtcAudioDataHandle handle,
    int* pRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pRetVal, int, RTCAudioData, num_channels);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioData_ScaleFrom(
    rtcAudioDataHandle handle,
    rtcAudioDataHandle source_data,
    int* pRetVal
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(source_data, rtcResultU4::kInvalidParameter);

  scoped_refptr<RTCAudioData> p = static_cast<RTCAudioData*>(handle);
  scoped_refptr<RTCAudioData> pSourceData = static_cast<RTCAudioData*>(source_data);
  int retVal = p->ScaleFrom(pSourceData);
  if (pRetVal) { *pRetVal = retVal; }
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioData_Clear(
    rtcAudioDataHandle handle,
    rtcAudioDataToneFrequency frequency,
    int* pRetVal
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);

  scoped_refptr<RTCAudioData> p = static_cast<RTCAudioData*>(handle);
  int retVal = p->Clear(static_cast<RTCAudioDataToneFrequency>(frequency));
  if (pRetVal) { *pRetVal = retVal; }
  return rtcResultU4::kSuccess;
}
