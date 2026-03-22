#include "interop_api.h"
#include "rtc_frame_cryptor.h"
#include "src/rtc_frame_cryptor_impl.h"
#include "src/rtc_peerconnection_factory_impl.h"
#include "src/rtc_rtp_receiver_impl.h"
#include "src/rtc_rtp_sender_impl.h"

#include <memory>
#include <vector>
#include <algorithm>

using namespace libwebrtc;

// Helper function to convert portable vector
static std::vector<uint8_t> ToStdVector(const unsigned char* data, int len) {
    if (!data || len <= 0) return {};
    return std::vector<uint8_t>(data, data + len);
}

// Helper function to copy data to output buffer
static rtcResultU4 CopyToOutputBuffer(const std::vector<uint8_t>& data,
                                      unsigned char* out_buffer,
                                      int* out_len) {
    if (!out_len) return rtcResultU4::kInvalidParameter;
    
    if (out_buffer && *out_len > 0) {
        size_t copy_len = std::min(static_cast<size_t>(*out_len), data.size());
        if (copy_len > 0) {
            memcpy(out_buffer, data.data(), copy_len);
        }
        if (copy_len < data.size()) {
            *out_len = static_cast<int>(data.size());
            return rtcResultU4::kBufferTooSmall;
        }
    }
    
    *out_len = static_cast<int>(data.size());
    return rtcResultU4::kSuccess;
}

// FrameCryptorObserver adapter for C callbacks
class FrameCryptorObserverAdapter : public RTCFrameCryptorObserver {
public:
    FrameCryptorObserverAdapter(const rtcFrameCryptorObserverCallbacks* callbacks)
        : callbacks_(*callbacks) {}
    
    virtual void OnFrameCryptionStateChanged(
        const string participant_id,
        RTCFrameCryptionState state) override {
        
        if (callbacks_.StateChanged) {
            // Convert RTCFrameCryptionState to rtcFrameCryptionState
            rtcFrameCryptionState c_state;
            switch (state) {
                case RTCFrameCryptionState::kNew:
                    c_state = rtcFrameCryptionState::kNew;
                    break;
                case RTCFrameCryptionState::kOk:
                    c_state = rtcFrameCryptionState::kOk;
                    break;
                case RTCFrameCryptionState::kEncryptionFailed:
                    c_state = rtcFrameCryptionState::kEncryptionFailed;
                    break;
                case RTCFrameCryptionState::kDecryptionFailed:
                    c_state = rtcFrameCryptionState::kDecryptionFailed;
                    break;
                case RTCFrameCryptionState::kMissingKey:
                    c_state = rtcFrameCryptionState::kMissingKey;
                    break;
                case RTCFrameCryptionState::kKeyRatcheted:
                    c_state = rtcFrameCryptionState::kKeyRatcheted;
                    break;
                case RTCFrameCryptionState::kInternalError:
                    c_state = rtcFrameCryptionState::kInternalError;
                    break;
                default:
                    c_state = rtcFrameCryptionState::kNew;
            }
            
            callbacks_.StateChanged(callbacks_.UserData,
                                   participant_id.c_string(),
                                   c_state);
        }
    }
    
private:
    rtcFrameCryptorObserverCallbacks callbacks_;
};

// Helper function to convert algorithm
static libwebrtc::Algorithm ConvertAlgorithm(rtcFrameCryptionAlgorithm algorithm) {
    switch (algorithm) {
        case rtcFrameCryptionAlgorithm::kAesGcm:
            return libwebrtc::Algorithm::kAesGcm;
        case rtcFrameCryptionAlgorithm::kAesCbc:
            return libwebrtc::Algorithm::kAesCbc;
        default:
            return libwebrtc::Algorithm::kAesGcm;
    }
}

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
KeyProvider_Create(const rtcKeyProviderOptions* options,
                   rtcKeyProviderHandle* pOutRetVal) noexcept {
    CHECK_OUT_POINTER(pOutRetVal);
    
    if (!options) {
        return rtcResultU4::kInvalidParameter;
    }
    
    // Без использования try/catch, так как исключения отключены
    auto opts = std::make_unique<libwebrtc::KeyProviderOptions>();
    opts->shared_key = options->shared_key == rtcBool32::kTrue;
    opts->ratchet_salt = ToStdVector(options->ratchet_salt, 
                                    options->ratchet_salt_len);
    opts->uncrypted_magic_bytes = ToStdVector(options->uncrypted_magic_bytes,
                                             options->uncrypted_magic_bytes_len);
    opts->ratchet_window_size = options->ratchet_window_size;
    opts->failure_tolerance = options->failure_tolerance;
    opts->key_ring_size = static_cast<int>(options->key_ring_size);
    opts->discard_frame_when_cryptor_not_ready = 
        options->discard_frame_when_cryptor_not_ready == rtcBool32::kTrue;
    
    libwebrtc::scoped_refptr<libwebrtc::KeyProvider> provider = 
        libwebrtc::KeyProvider::Create(opts.get());
    if (!provider) {
        return rtcResultU4::kUnknownError;
    }
    
    *pOutRetVal = static_cast<rtcKeyProviderHandle>(provider.release());
    return rtcResultU4::kSuccess;
}

LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
KeyProvider_SetSharedKey(rtcKeyProviderHandle handle,
                         int index,
                         const unsigned char* key,
                         int key_len) noexcept {
    CHECK_POINTER_EX(handle, rtcBool32::kFalse);
    
    libwebrtc::scoped_refptr<libwebrtc::KeyProvider> provider = 
        static_cast<libwebrtc::KeyProvider*>(handle);
    
    std::vector<uint8_t> key_vec = ToStdVector(key, key_len);
    bool result = provider->SetSharedKey(index, key_vec);
    return result ? rtcBool32::kTrue : rtcBool32::kFalse;
}

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
KeyProvider_RatchetSharedKey(rtcKeyProviderHandle handle,
                             int key_index,
                             unsigned char* pOutKey,
                             int* pOutKeyLen) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER(pOutKeyLen);
    
    libwebrtc::scoped_refptr<libwebrtc::KeyProvider> provider = 
        static_cast<libwebrtc::KeyProvider*>(handle);
    
    std::vector<uint8_t> result = provider->RatchetSharedKey(key_index).std_vector();
    return CopyToOutputBuffer(result, pOutKey, pOutKeyLen);
}

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
KeyProvider_ExportSharedKey(rtcKeyProviderHandle handle,
                            int key_index,
                            unsigned char* pOutKey,
                            int* pOutKeyLen) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER(pOutKeyLen);
    
    libwebrtc::scoped_refptr<libwebrtc::KeyProvider> provider = 
        static_cast<libwebrtc::KeyProvider*>(handle);
    
    std::vector<uint8_t> result = provider->ExportSharedKey(key_index).std_vector();
    return CopyToOutputBuffer(result, pOutKey, pOutKeyLen);
}

LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
KeyProvider_SetKey(rtcKeyProviderHandle handle,
                   const char* participant_id,
                   int index,
                   const unsigned char* key,
                   int key_len) noexcept {
    CHECK_POINTER_EX(handle, rtcBool32::kFalse);
    CHECK_POINTER_EX(participant_id, rtcBool32::kFalse);
    
    libwebrtc::scoped_refptr<libwebrtc::KeyProvider> provider = 
        static_cast<libwebrtc::KeyProvider*>(handle);
    
    std::vector<uint8_t> key_vec = ToStdVector(key, key_len);
    bool result = provider->SetKey(participant_id, index, key_vec);
    return result ? rtcBool32::kTrue : rtcBool32::kFalse;
}

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
KeyProvider_RatchetKey(rtcKeyProviderHandle handle,
                       const char* participant_id,
                       int key_index,
                       unsigned char* pOutKey,
                       int* pOutKeyLen) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER_EX(participant_id, rtcResultU4::kInvalidParameter);
    CHECK_POINTER(pOutKeyLen);
    
    libwebrtc::scoped_refptr<libwebrtc::KeyProvider> provider = 
        static_cast<libwebrtc::KeyProvider*>(handle);
    
    std::vector<uint8_t> result = provider->RatchetKey(participant_id, 
                                                      key_index).std_vector();
    return CopyToOutputBuffer(result, pOutKey, pOutKeyLen);
}

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
KeyProvider_ExportKey(rtcKeyProviderHandle handle,
                      const char* participant_id,
                      int key_index,
                      unsigned char* pOutKey,
                      int* pOutKeyLen) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER_EX(participant_id, rtcResultU4::kInvalidParameter);
    CHECK_POINTER(pOutKeyLen);
    
    libwebrtc::scoped_refptr<libwebrtc::KeyProvider> provider = 
        static_cast<libwebrtc::KeyProvider*>(handle);
    
    std::vector<uint8_t> result = provider->ExportKey(participant_id, 
                                                     key_index).std_vector();
    return CopyToOutputBuffer(result, pOutKey, pOutKeyLen);
}

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
KeyProvider_SetSifTrailer(rtcKeyProviderHandle handle,
                          const unsigned char* trailer,
                          int trailer_len) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    
    libwebrtc::scoped_refptr<libwebrtc::KeyProvider> provider = 
        static_cast<libwebrtc::KeyProvider*>(handle);
    
    std::vector<uint8_t> trailer_vec = ToStdVector(trailer, trailer_len);
    provider->SetSifTrailer(trailer_vec);
    return rtcResultU4::kSuccess;
}

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCFrameCryptor_CreateForSender(rtcPeerConnectionFactoryHandle factory,
                                const char* participant_id,
                                rtcRtpSenderHandle sender,
                                rtcFrameCryptionAlgorithm algorithm,
                                rtcKeyProviderHandle key_provider,
                                rtcFrameCryptorHandle* pOutRetVal) noexcept {
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_NATIVE_HANDLE(factory);
    CHECK_POINTER_EX(participant_id, rtcResultU4::kInvalidParameter);
    CHECK_NATIVE_HANDLE(sender);
    CHECK_NATIVE_HANDLE(key_provider);
    
    libwebrtc::scoped_refptr<libwebrtc::RTCPeerConnectionFactory> pc_factory = 
        static_cast<libwebrtc::RTCPeerConnectionFactory*>(factory);
    libwebrtc::scoped_refptr<libwebrtc::RTCRtpSender> rtp_sender = 
        static_cast<libwebrtc::RTCRtpSender*>(sender);
    libwebrtc::scoped_refptr<libwebrtc::KeyProvider> provider = 
        static_cast<libwebrtc::KeyProvider*>(key_provider);
    
    libwebrtc::scoped_refptr<libwebrtc::RTCFrameCryptor> cryptor = 
        libwebrtc::FrameCryptorFactory::frameCryptorFromRtpSender(
            pc_factory,
            participant_id,
            rtp_sender,
            ConvertAlgorithm(algorithm),
            provider);
    
    if (!cryptor) {
        return rtcResultU4::kUnknownError;
    }
    
    *pOutRetVal = static_cast<rtcFrameCryptorHandle>(cryptor.release());
    return rtcResultU4::kSuccess;
}

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCFrameCryptor_CreateForReceiver(rtcPeerConnectionFactoryHandle factory,
                                  const char* participant_id,
                                  rtcRtpReceiverHandle receiver,
                                  rtcFrameCryptionAlgorithm algorithm,
                                  rtcKeyProviderHandle key_provider,
                                  rtcFrameCryptorHandle* pOutRetVal) noexcept {
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_NATIVE_HANDLE(factory);
    CHECK_POINTER_EX(participant_id, rtcResultU4::kInvalidParameter);
    CHECK_NATIVE_HANDLE(receiver);
    CHECK_NATIVE_HANDLE(key_provider);
    
    libwebrtc::scoped_refptr<libwebrtc::RTCPeerConnectionFactory> pc_factory = 
        static_cast<libwebrtc::RTCPeerConnectionFactory*>(factory);
    libwebrtc::scoped_refptr<libwebrtc::RTCRtpReceiver> rtp_receiver = 
        static_cast<libwebrtc::RTCRtpReceiver*>(receiver);
    libwebrtc::scoped_refptr<libwebrtc::KeyProvider> provider = 
        static_cast<libwebrtc::KeyProvider*>(key_provider);
    
    libwebrtc::scoped_refptr<libwebrtc::RTCFrameCryptor> cryptor = 
        libwebrtc::FrameCryptorFactory::frameCryptorFromRtpReceiver(
            pc_factory,
            participant_id,
            rtp_receiver,
            ConvertAlgorithm(algorithm),
            provider);
    
    if (!cryptor) {
        return rtcResultU4::kUnknownError;
    }
    
    *pOutRetVal = static_cast<rtcFrameCryptorHandle>(cryptor.release());
    return rtcResultU4::kSuccess;
}

LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
RTCFrameCryptor_SetEnabled(rtcFrameCryptorHandle handle,
                           rtcBool32 enabled) noexcept {
    CHECK_POINTER_EX(handle, rtcBool32::kFalse);
    
    libwebrtc::scoped_refptr<libwebrtc::RTCFrameCryptor> cryptor = 
        static_cast<libwebrtc::RTCFrameCryptor*>(handle);
    
    bool result = cryptor->SetEnabled(enabled == rtcBool32::kTrue);
    return result ? rtcBool32::kTrue : rtcBool32::kFalse;
}

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCFrameCryptor_GetEnabled(rtcFrameCryptorHandle handle,
                           rtcBool32* pOutRetVal) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER(pOutRetVal);
    
    libwebrtc::scoped_refptr<libwebrtc::RTCFrameCryptor> cryptor = 
        static_cast<libwebrtc::RTCFrameCryptor*>(handle);
    
    bool enabled = cryptor->enabled();
    *pOutRetVal = enabled ? rtcBool32::kTrue : rtcBool32::kFalse;
    return rtcResultU4::kSuccess;
}

LIB_WEBRTC_API rtcBool32 LIB_WEBRTC_CALL
RTCFrameCryptor_SetKeyIndex(rtcFrameCryptorHandle handle,
                            int index) noexcept {
    CHECK_POINTER_EX(handle, rtcBool32::kFalse);
    
    libwebrtc::scoped_refptr<libwebrtc::RTCFrameCryptor> cryptor = 
        static_cast<libwebrtc::RTCFrameCryptor*>(handle);
    
    bool result = cryptor->SetKeyIndex(index);
    return result ? rtcBool32::kTrue : rtcBool32::kFalse;
}

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCFrameCryptor_GetKeyIndex(rtcFrameCryptorHandle handle,
                            int* pOutRetVal) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER(pOutRetVal);
    
    libwebrtc::scoped_refptr<libwebrtc::RTCFrameCryptor> cryptor = 
        static_cast<libwebrtc::RTCFrameCryptor*>(handle);
    
    *pOutRetVal = cryptor->key_index();
    return rtcResultU4::kSuccess;
}

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCFrameCryptor_GetParticipantId(rtcFrameCryptorHandle handle,
                                 char* pOutRetVal,
                                 int sz_value) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    CHECK_AND_ZERO_MEMORY(pOutRetVal, sz_value);
    
    libwebrtc::scoped_refptr<libwebrtc::RTCFrameCryptor> cryptor = 
        static_cast<libwebrtc::RTCFrameCryptor*>(handle);
    
    libwebrtc::string participant_id = cryptor->participant_id();
    size_t len = participant_id.copy_to(pOutRetVal, 
                                       static_cast<size_t>(sz_value));
    return participant_id.size() > len ? rtcResultU4::kBufferTooSmall 
                                      : rtcResultU4::kSuccess;
}

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCFrameCryptor_RegisterObserver(rtcFrameCryptorHandle handle,
                                 const rtcFrameCryptorObserverCallbacks* callbacks) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER_EX(callbacks, rtcResultU4::kInvalidParameter);
    
    libwebrtc::scoped_refptr<libwebrtc::RTCFrameCryptor> cryptor = 
        static_cast<libwebrtc::RTCFrameCryptor*>(handle);
    
    auto observer = new libwebrtc::RefCountedObject<FrameCryptorObserverAdapter>(callbacks);
    cryptor->RegisterRTCFrameCryptorObserver(observer);
    return rtcResultU4::kSuccess;
}

LIB_WEBRTC_API rtcResultU4 LIB_WEBRTC_CALL
RTCFrameCryptor_UnregisterObserver(rtcFrameCryptorHandle handle) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    
    libwebrtc::scoped_refptr<libwebrtc::RTCFrameCryptor> cryptor = 
        static_cast<libwebrtc::RTCFrameCryptor*>(handle);
    
    cryptor->DeRegisterRTCFrameCryptorObserver();
    return rtcResultU4::kSuccess;
}