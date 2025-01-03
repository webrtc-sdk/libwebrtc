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
    return static_cast<int>(pAudioDevice->PlayoutDevices());
} // end RTCAudioDevice_PlayoutDevices

int LIB_WEBRTC_CALL
RTCAudioDevice_RecordingDevices(
    rtcAudioDeviceHandle audiDevice
) noexcept
{
    CHECK_POINTER_EX(audiDevice, 0);
    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    return static_cast<int>(pAudioDevice->RecordingDevices());
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

    size_t len;
    string strName(szName);
    string strGuid(szGuid);

    len = strName.copy_to(pOutName, static_cast<size_t>(cchOutName));
    if (strName.size() > len) {
        result = rtcResultU4::kBufferTooSmall;
    }

    len = strGuid.copy_to(pOutGuid, static_cast<size_t>(cchOutGuid));
    if (strGuid.size() > len) {
        result = rtcResultU4::kBufferTooSmall;
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

    char szName[RTCAudioDevice::kAdmMaxDeviceNameSize] = {0};
    char szGuid[RTCAudioDevice::kAdmMaxGuidSize] = {0};
    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);

    rtcResultU4 result = (rtcResultU4)pAudioDevice->RecordingDeviceName((uint16_t)index, szName, szGuid);
    if (result != rtcResultU4::kSuccess) {
        return result;
    }

    size_t len;
    string strName(szName);
    string strGuid(szGuid);

    len = strName.copy_to(pOutName, static_cast<size_t>(cchOutName));
    if (strName.size() > len) {
        result = rtcResultU4::kBufferTooSmall;
    }

    len = strGuid.copy_to(pOutGuid, static_cast<size_t>(cchOutGuid));
    if (strGuid.size() > len) {
        result = rtcResultU4::kBufferTooSmall;
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
    if (index < 1) {
        return rtcResultU4::kOutOfRange;
    }

    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    int nCount = (int)pAudioDevice->PlayoutDevices();
    if (index >= nCount) {
        return rtcResultU4::kOutOfRange;
    }
    return static_cast<rtcResultU4>(pAudioDevice->SetPlayoutDevice((uint16_t)index));
} // end RTCAudioDevice_SetPlayoutDevice

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_SetRecordingDevice(
    rtcAudioDeviceHandle audiDevice,
    int index
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);
    if (index < 1) {
        return rtcResultU4::kOutOfRange;
    }

    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    int nCount = (int)pAudioDevice->RecordingDevices();
    if (index >= nCount) {
        return rtcResultU4::kOutOfRange;
    }
    return static_cast<rtcResultU4>(pAudioDevice->SetRecordingDevice((uint16_t)index));
} // end RTCAudioDevice_SetRecordingDevice

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_RegisterDeviceChangeCallback(
    rtcAudioDeviceHandle audiDevice,
    rtcAudioDeviceCallbacks* callbacks
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);

    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    RTCAudioDevice::OnDeviceChangeCallback cb;
    if (callbacks == nullptr || callbacks->DeviceChanged == nullptr) {
        cb = nullptr;
    }
    else {
        rtcAudioDeviceCallbacks cb2 = *callbacks;
        cb = [cb2](void) {
            if (cb2.DeviceChanged) {
                cb2.DeviceChanged(cb2.UserData);
            }
        };
    }

    return static_cast<rtcResultU4>(pAudioDevice->OnDeviceChange(cb));
} // end RTCAudioDevice_RegisterDeviceChangeCallback

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_UnregisterDeviceChangeCallback(
    rtcAudioDeviceHandle audiDevice
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);

    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    RTCAudioDevice::OnDeviceChangeCallback cb = nullptr;
    return static_cast<rtcResultU4>(pAudioDevice->OnDeviceChange(cb));
} // end RTCAudioDevice_UnregisterDeviceChangeCallback

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_SetMicrophoneVolume(
    rtcAudioDeviceHandle audiDevice,
    unsigned int volume
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);
    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    return static_cast<rtcResultU4>(pAudioDevice->SetMicrophoneVolume(volume));
} // end RTCAudioDevice_SetMicrophoneVolume

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_GetMicrophoneVolume(
    rtcAudioDeviceHandle audiDevice,
    unsigned int* volume
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);
    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    return static_cast<rtcResultU4>(pAudioDevice->MicrophoneVolume(*volume));
} // end RTCAudioDevice_GetMicrophoneVolume

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_SetSpeakerVolume(
    rtcAudioDeviceHandle audiDevice,
    unsigned int volume
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);
    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    return static_cast<rtcResultU4>(pAudioDevice->SetSpeakerVolume(volume));
} // end RTCAudioDevice_SetSpeakerVolume

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioDevice_GetSpeakerVolume(
    rtcAudioDeviceHandle audiDevice,
    unsigned int* volume
) noexcept
{
    CHECK_NATIVE_HANDLE(audiDevice);
    scoped_refptr<RTCAudioDevice> pAudioDevice = static_cast<RTCAudioDevice*>(audiDevice);
    return static_cast<rtcResultU4>(pAudioDevice->SpeakerVolume(*volume));
} // end RTCAudioDevice_GetSpeakerVolume
