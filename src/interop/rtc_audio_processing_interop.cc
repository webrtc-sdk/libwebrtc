#include "rtc_audio_processing.h"
#include "src/rtc_audio_processing_impl.h"
#include "interop_api.h"

using namespace libwebrtc;

// CustomProcessing implementation that bridges C++ and C callbacks
class CustomProcessingBridge : public RTCAudioProcessing::CustomProcessing {
public:
    CustomProcessingBridge(rtcAudioProcessingCustomCallbacks* callbacks)
        : callbacks_(*callbacks) {
    }
    
    ~CustomProcessingBridge() {
        if (callbacks_.Release) {
            callbacks_.Release(callbacks_.UserData);
        }
    }
    
    void Initialize(int sample_rate_hz, int num_channels) override {
        if (callbacks_.Initialize) {
            callbacks_.Initialize(callbacks_.UserData, sample_rate_hz, num_channels);
        }
    }
    
    void Process(int num_bands, int num_frames, int buffer_size, float* buffer) override {
        if (callbacks_.Process) {
            callbacks_.Process(callbacks_.UserData, num_bands, num_frames, buffer_size, buffer);
        }
    }
    
    void Reset(int new_rate) override {
        if (callbacks_.Reset) {
            callbacks_.Reset(callbacks_.UserData, new_rate);
        }
    }
    
    void Release() override {
        // Memory management is handled by the bridge
    }
    
private:
    rtcAudioProcessingCustomCallbacks callbacks_;
};

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioProcessing_Create(
    rtcAudioProcessingHandle* pOutRetVal
) noexcept {
    CHECK_OUT_POINTER(pOutRetVal);
    
    scoped_refptr<RTCAudioProcessing> audioProcessing = 
        new RefCountedObject<RTCAudioProcessingImpl>();
    
    *pOutRetVal = static_cast<rtcAudioProcessingHandle>(audioProcessing.release());
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioProcessing_SetCapturePostProcessing(
    rtcAudioProcessingHandle handle,
    rtcAudioProcessingCustomHandle customHandle
) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER(customHandle);
    
    scoped_refptr<RTCAudioProcessing> audioProcessing = 
        static_cast<RTCAudioProcessing*>(handle);
    
    RTCAudioProcessing::CustomProcessing* customProcessing = 
        static_cast<RTCAudioProcessing::CustomProcessing*>(customHandle);
    
    audioProcessing->SetCapturePostProcessing(customProcessing);
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioProcessing_SetRenderPreProcessing(
    rtcAudioProcessingHandle handle,
    rtcAudioProcessingCustomHandle customHandle
) noexcept {
    CHECK_NATIVE_HANDLE(handle);
    CHECK_POINTER(customHandle);
    
    scoped_refptr<RTCAudioProcessing> audioProcessing = 
        static_cast<RTCAudioProcessing*>(handle);
    
    RTCAudioProcessing::CustomProcessing* customProcessing = 
        static_cast<RTCAudioProcessing::CustomProcessing*>(customHandle);
    
    audioProcessing->SetRenderPreProcessing(customProcessing);
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioProcessing_CreateCustom(
    rtcAudioProcessingCustomCallbacks* callbacks,
    rtcAudioProcessingCustomHandle* pOutRetVal
) noexcept {
    CHECK_OUT_POINTER(pOutRetVal);
    CHECK_POINTER(callbacks);
    
    // Validate required callbacks
    if (!callbacks->Process) {
        return rtcResultU4::kInvalidParameter;
    }
    
    // Create the bridge
    CustomProcessingBridge* bridge = new (std::nothrow) CustomProcessingBridge(callbacks);
    if (!bridge) {
        return rtcResultU4::kUnknownError;
    }
    
    *pOutRetVal = static_cast<rtcAudioProcessingCustomHandle>(bridge);
    return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCAudioProcessing_ReleaseCustom(
    rtcAudioProcessingCustomHandle customHandle
) noexcept {
    CHECK_POINTER(customHandle);
    
    CustomProcessingBridge* bridge = 
        static_cast<CustomProcessingBridge*>(customHandle);
    
    delete bridge;
    return rtcResultU4::kSuccess;
}