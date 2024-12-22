#include "rtc_audio_device.h"
#include "interop_api.h"

using namespace libwebrtc;

/*
 * ---------------------------------------------------------------------- 
 * RTCAudioDevice interop methods
 * ---------------------------------------------------------------------- 
 */

int LIB_WEBRTC_CALL
RTCAudioDevice_PlayoutDevices(
    rtcAudioDeviceHandle audiDevice
) noexcept
{
    CHECK_POINTER_EX(audiDevice, 0);    
    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    return (int)pAudioDevice->PlayoutDevices();
} // end RTCAudioDevice_PlayoutDevices

int LIB_WEBRTC_CALL
RTCAudioDevice_RecordingDevices(
    rtcAudioDeviceHandle audiDevice
) noexcept
{
    CHECK_POINTER_EX(audiDevice, 0);
    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    return (int)pAudioDevice->RecordingDevices();
} // end RTCAudioDevice_RecordingDevices

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_PlayoutDeviceName(
    rtcAudioDeviceHandle audiDevice,
    int index,
    char* pOutName,
    int cchOutName,
    char* pOutGuid,
    int cchOutGuid
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);
    CHECK_POINTER(pOutName);
    CHECK_POINTER(pOutGuid);
    if (cchOutName <= 0 && cchOutGuid <= 0) {
        return rtcResultU4::kBufferTooSmall;
    }
    ZERO_MEMORY(pOutName, cchOutName);
    ZERO_MEMORY(pOutGuid, cchOutGuid);

    char szName[RTCAudioDevice::kAdmMaxDeviceNameSize] = {0};
    char szGuid[RTCAudioDevice::kAdmMaxGuidSize] = {0};
    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);

    rtcResultU4 result = (rtcResultU4)pAudioDevice->PlayoutDeviceName((uint16_t)index, szName, szGuid);
    if (result != rtcResultU4::kSuccess) {
        return result;
    }

    int nSrcNameLen = ((int)strlen(szName));
    if (nSrcNameLen > cchOutName) {
        result = rtcResultU4::kBufferTooSmall;
    }
    int nSrcGuidLen = ((int)strlen(szGuid));
    if (nSrcGuidLen > cchOutGuid) {
        result = rtcResultU4::kBufferTooSmall;
    }

    int nNameLen = std::min(cchOutName, nSrcNameLen);
    if (nNameLen > 0) {
        strncpy(pOutName, (const char*)szName, nNameLen);
        pOutName[nNameLen] = '\0';
    }
    
    int nGuidLen = std::min(cchOutGuid, nSrcGuidLen);
    if (nGuidLen > 0) {
        strncpy(pOutGuid, (const char*)szGuid, nGuidLen);
        pOutGuid[nGuidLen] = '\0';
    }

    return result;
} // end RTCAudioDevice_PlayoutDeviceName

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_RecordingDeviceName(
    rtcAudioDeviceHandle audiDevice,
    int index,
    char* pOutName,
    int cchOutName,
    char* pOutGuid,
    int cchOutGuid
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);
    CHECK_POINTER(pOutName);
    CHECK_POINTER(pOutGuid);
    if (cchOutName <= 0 && cchOutGuid <= 0) {
        return rtcResultU4::kBufferTooSmall;
    }
    ZERO_MEMORY(pOutName, cchOutName);
    ZERO_MEMORY(pOutGuid, cchOutGuid);
    cchOutName--;
    cchOutGuid--;

    char szName[RTCAudioDevice::kAdmMaxDeviceNameSize] = {0};
    char szGuid[RTCAudioDevice::kAdmMaxGuidSize] = {0};
    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);

    rtcResultU4 result = (rtcResultU4)pAudioDevice->RecordingDeviceName((uint16_t)index, szName, szGuid);
    if (result != rtcResultU4::kSuccess) {
        return result;
    }

    int nSrcNameLen = ((int)strlen(szName));
    if (nSrcNameLen > cchOutName) {
        result = rtcResultU4::kBufferTooSmall;
    }
    int nSrcGuidLen = ((int)strlen(szGuid));
    if (nSrcGuidLen > cchOutGuid) {
        result = rtcResultU4::kBufferTooSmall;
    }

    int nNameLen = std::min(cchOutName, nSrcNameLen);
    if (nNameLen > 0) {
        strncpy(pOutName, (const char*)szName, nNameLen);
        pOutName[nNameLen] = '\0';
    }
    
    int nGuidLen = std::min(cchOutGuid, nSrcGuidLen);
    if (nGuidLen > 0) {
        strncpy(pOutGuid, (const char*)szGuid, nGuidLen);
        pOutGuid[nGuidLen] = '\0';
    }

    return result;
} // end RTCAudioDevice_RecordingDeviceName

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_SetPlayoutDevice(
    rtcAudioDeviceHandle audiDevice,
    int index
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);

    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    int nCount = (int)pAudioDevice->PlayoutDevices();
    if (index < 0 || index >= nCount) {
        return rtcResultU4::kOutOfRange;
    }
    return (rtcResultU4)pAudioDevice->SetPlayoutDevice((uint16_t)index);
} // end RTCAudioDevice_SetPlayoutDevice

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_SetRecordingDevice(
    rtcAudioDeviceHandle audiDevice,
    int index
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);

    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    int nCount = (int)pAudioDevice->RecordingDevices();
    if (index < 0 || index >= nCount) {
        return rtcResultU4::kOutOfRange;
    }
    return (rtcResultU4)pAudioDevice->SetRecordingDevice((uint16_t)index);
} // end RTCAudioDevice_SetRecordingDevice

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_RegisterDeviceChangeCallback(
    rtcAudioDeviceHandle audiDevice,
    rtcAudioDeviceChangeDelegate deviceChangeCallback
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);

    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    RTCAudioDevice::OnDeviceChangeCallback cb;
    if (deviceChangeCallback == nullptr) {
        cb = nullptr;
    }
    else {
        cb = [deviceChangeCallback](void) {
            if (deviceChangeCallback) {
                deviceChangeCallback();
            }
        };
    }
    rtcResultU4 result = (rtcResultU4)pAudioDevice->OnDeviceChange(cb);
    return result;
} // end RTCAudioDevice_RegisterDeviceChangeCallback

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_SetMicrophoneVolume(
    rtcAudioDeviceHandle audiDevice,
    unsigned int volume
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);
    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    rtcResultU4 result = (rtcResultU4)pAudioDevice->SetMicrophoneVolume(volume);
    return result;
} // end RTCAudioDevice_SetMicrophoneVolume

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_GetMicrophoneVolume(
    rtcAudioDeviceHandle audiDevice,
    unsigned int* volume
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);
    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    rtcResultU4 result = (rtcResultU4)pAudioDevice->MicrophoneVolume(*volume);
    return result;
} // end RTCAudioDevice_GetMicrophoneVolume

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_SetSpeakerVolume(
    rtcAudioDeviceHandle audiDevice,
    unsigned int volume
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);
    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    rtcResultU4 result = (rtcResultU4)pAudioDevice->SetSpeakerVolume(volume);
    return result;
} // end RTCAudioDevice_SetSpeakerVolume

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_GetSpeakerVolume(
    rtcAudioDeviceHandle audiDevice,
    unsigned int* volume
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);
    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    rtcResultU4 result = (rtcResultU4)pAudioDevice->SpeakerVolume(*volume);
    return result;
} // end RTCAudioDevice_GetSpeakerVolume
