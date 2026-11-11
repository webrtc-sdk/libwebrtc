#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// RTCFrameCryptor creation requires a key provider plus a live RTP
// sender/receiver, which is heavy to construct in a unit test. These tests
// therefore concentrate on the null-handle/invalid-argument negative paths
// for every exposed function, asserting the exact return codes from
// src/interop/rtc_frame_cryptor_interop.cc.

TEST(RTCFrameCryptorNegative, CreateForSenderWithNullOutPointerFails) {
    EXPECT_EQ(RTCFrameCryptor_CreateForSender(
                  nullptr, "p0", nullptr,
                  rtcFrameCryptionAlgorithm::kAesGcm, nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCFrameCryptorNegative, CreateForSenderWithNullFactoryFails) {
    rtcFrameCryptorHandle cryptor = nullptr;
    EXPECT_EQ(RTCFrameCryptor_CreateForSender(
                  nullptr, "p0", nullptr,
                  rtcFrameCryptionAlgorithm::kAesGcm, nullptr, &cryptor),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(cryptor, nullptr);
}

TEST(RTCFrameCryptorNegative, CreateForReceiverWithNullOutPointerFails) {
    EXPECT_EQ(RTCFrameCryptor_CreateForReceiver(
                  nullptr, "p0", nullptr,
                  rtcFrameCryptionAlgorithm::kAesGcm, nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCFrameCryptorNegative, CreateForReceiverWithNullFactoryFails) {
    rtcFrameCryptorHandle cryptor = nullptr;
    EXPECT_EQ(RTCFrameCryptor_CreateForReceiver(
                  nullptr, "p0", nullptr,
                  rtcFrameCryptionAlgorithm::kAesGcm, nullptr, &cryptor),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(cryptor, nullptr);
}

TEST(RTCFrameCryptorNegative, SetEnabledWithNullHandleReturnsFalse) {
    EXPECT_EQ(RTCFrameCryptor_SetEnabled(nullptr, rtcBool32::kTrue),
              rtcBool32::kFalse);
}

TEST(RTCFrameCryptorNegative, GetEnabledWithNullHandleFails) {
    rtcBool32 enabled = rtcBool32::kTrue;
    EXPECT_EQ(RTCFrameCryptor_GetEnabled(nullptr, &enabled),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCFrameCryptorNegative, GetEnabledWithNullOutPointerFails) {
    // CHECK_NATIVE_HANDLE runs before CHECK_POINTER, so a null handle is
    // reported first regardless of the out-pointer.
    EXPECT_EQ(RTCFrameCryptor_GetEnabled(nullptr, nullptr),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCFrameCryptorNegative, SetKeyIndexWithNullHandleReturnsFalse) {
    EXPECT_EQ(RTCFrameCryptor_SetKeyIndex(nullptr, 0), rtcBool32::kFalse);
}

TEST(RTCFrameCryptorNegative, GetKeyIndexWithNullHandleFails) {
    int index = 0;
    EXPECT_EQ(RTCFrameCryptor_GetKeyIndex(nullptr, &index),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCFrameCryptorNegative, GetParticipantIdWithNullHandleFails) {
    char buffer[64] = {0};
    EXPECT_EQ(RTCFrameCryptor_GetParticipantId(nullptr, buffer, 64),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCFrameCryptorNegative, RegisterObserverWithNullHandleFails) {
    rtcFrameCryptorObserverCallbacks callbacks{};
    EXPECT_EQ(RTCFrameCryptor_RegisterObserver(nullptr, &callbacks),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCFrameCryptorNegative, UnregisterObserverWithNullHandleFails) {
    EXPECT_EQ(RTCFrameCryptor_UnregisterObserver(nullptr),
              rtcResultU4::kInvalidNativeHandle);
}

}  // namespace libwebrtc
