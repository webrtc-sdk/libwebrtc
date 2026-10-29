#include <memory>
#include <string>
#include <vector>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// Negative-path tests that don't require a live RTP parameters object.

TEST(RTCRtpParametersNegative, GetCodecsRejectsNullHandle) {
    rtcRtpCodecParametersListHandle list = nullptr;
    EXPECT_EQ(RTCRtpParameters_GetCodecs(nullptr, &list),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(list, nullptr);
}

TEST(RTCRtpParametersNegative, GetCodecsRejectsNullOutPointer) {
    // CHECK_OUT_POINTER runs before the handle check.
    EXPECT_EQ(RTCRtpParameters_GetCodecs(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCRtpParametersNegative, GetEncodingsRejectsNullHandle) {
    rtcRtpEncodingParametersListHandle list = nullptr;
    EXPECT_EQ(RTCRtpParameters_GetEncodings(nullptr, &list),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(list, nullptr);
}

TEST(RTCRtpParametersNegative, GetEncodingsRejectsNullOutPointer) {
    EXPECT_EQ(RTCRtpParameters_GetEncodings(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCRtpParametersNegative, GetHeaderExtensionsRejectsNullHandle) {
    rtcRtpExtensionListHandle list = nullptr;
    EXPECT_EQ(RTCRtpParameters_GetHeaderExtensions(nullptr, &list),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(list, nullptr);
}

TEST(RTCRtpParametersNegative, GetTransactionIdRejectsNullHandle) {
    char buf[64] = {0};
    EXPECT_EQ(RTCRtpParameters_GetTransactionId(nullptr, buf, sizeof(buf)),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpParametersNegative, GetRtcpParametersRejectsNullOutPointer) {
    // DECLARE_GET_VALUE checks the out value pointer first.
    EXPECT_EQ(RTCRtpParameters_GetRtcpParameters(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCRtpParametersNegative, GetRtcpParametersRejectsNullHandle) {
    rtcRtcpParametersHandle rtcp = nullptr;
    EXPECT_EQ(RTCRtpParameters_GetRtcpParameters(nullptr, &rtcp),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(rtcp, nullptr);
}

TEST(RTCRtpParametersNegative, GetDegradationPreferenceRejectsNullHandle) {
    rtcDegradationPreference pref{};
    EXPECT_EQ(RTCRtpParameters_GetDegradationPreference(nullptr, &pref),
              rtcResultU4::kInvalidNativeHandle);
}

// --- List accessors (null-handle / null-out / index) ---

TEST(RTCRtpParametersNegative, ListGetCountWithNullHandleReturnsZero) {
    EXPECT_EQ(RTCRtpCodecParametersList_GetCount(nullptr), 0);
    EXPECT_EQ(RTCRtpEncodingParametersList_GetCount(nullptr), 0);
    EXPECT_EQ(RTCRtpExtensionList_GetCount(nullptr), 0);
    EXPECT_EQ(RTCRtcpParametersList_GetCount(nullptr), 0);
    EXPECT_EQ(RTCRtpCodecParametersMap_GetCount(nullptr), 0);
}

TEST(RTCRtpParametersNegative, ListGetItemRejectsNullOutPointer) {
    EXPECT_EQ(RTCRtpCodecParametersList_GetItem(nullptr, 0, nullptr),
              rtcResultU4::kInvalidPointer);
    EXPECT_EQ(RTCRtpEncodingParametersList_GetItem(nullptr, 0, nullptr),
              rtcResultU4::kInvalidPointer);
    EXPECT_EQ(RTCRtpExtensionList_GetItem(nullptr, 0, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCRtpParametersNegative, ListGetItemRejectsNullHandle) {
    rtcRtpCodecParametersHandle codec = nullptr;
    EXPECT_EQ(RTCRtpCodecParametersList_GetItem(nullptr, 0, &codec),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(codec, nullptr);

    rtcRtpEncodingParametersHandle enc = nullptr;
    EXPECT_EQ(RTCRtpEncodingParametersList_GetItem(nullptr, 0, &enc),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(enc, nullptr);

    rtcRtpExtensionHandle ext = nullptr;
    EXPECT_EQ(RTCRtpExtensionList_GetItem(nullptr, 0, &ext),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(ext, nullptr);
}

// --- Element-level getter null-handle checks ---

TEST(RTCRtpParametersNegative, ExtensionGettersRejectNullHandle) {
    char buf[64] = {0};
    EXPECT_EQ(RTCRtpExtension_GetUri(nullptr, buf, sizeof(buf)),
              rtcResultU4::kInvalidNativeHandle);
    int id = -1;
    EXPECT_EQ(RTCRtpExtension_GetId(nullptr, &id),
              rtcResultU4::kInvalidNativeHandle);
    rtcBool32 enc = rtcBool32::kTrue;
    EXPECT_EQ(RTCRtpExtension_GetEncrypt(nullptr, &enc),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpParametersNegative, CodecParametersGettersRejectNullHandle) {
    char buf[64] = {0};
    EXPECT_EQ(RTCRtpCodecParameters_GetMimeType(nullptr, buf, sizeof(buf)),
              rtcResultU4::kInvalidNativeHandle);
    int pt = -1;
    EXPECT_EQ(RTCRtpCodecParameters_GetPayloadType(nullptr, &pt),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpParametersNegative, RtcpParametersGettersRejectNullHandle) {
    unsigned int ssrc = 0;
    EXPECT_EQ(RTCRtcpParameters_GetSSRC(nullptr, &ssrc),
              rtcResultU4::kInvalidNativeHandle);
    rtcBool32 mux = rtcBool32::kTrue;
    EXPECT_EQ(RTCRtcpParameters_GetMux(nullptr, &mux),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpParametersNegative, EncodingParametersGettersRejectNullHandle) {
    unsigned int ssrc = 0;
    EXPECT_EQ(RTCRtpEncodingParameters_GetSSRC(nullptr, &ssrc),
              rtcResultU4::kInvalidNativeHandle);
    rtcBool32 active = rtcBool32::kFalse;
    EXPECT_EQ(RTCRtpEncodingParameters_GetActive(nullptr, &active),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpParametersNegative, CodecParametersPairCreateRejectsNullOut) {
    EXPECT_EQ(RTCRtpCodecParametersPair_Create(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

// Fixture that owns a peer connection and produces a live RTP parameters
// object via transceiver -> sender -> parameters.
class RTCRtpParametersTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_EQ(LibWebRTC_Initialize(), rtcBool32::kTrue);
        factory_ = LibWebRTC_CreateRTCPeerConnectionFactory();
        ASSERT_NE(factory_, nullptr);
        ASSERT_EQ(RTCPeerConnectionFactory_Initialize(factory_),
                  rtcBool32::kTrue);

        rtcPeerConnectionConfiguration config{};
        ASSERT_EQ(RTCPeerConnectionFactory_CreatePeerConnection(
                      factory_, &config, nullptr, &pc_),
                  rtcResultU4::kSuccess);
        ASSERT_NE(pc_, nullptr);
    }

    void TearDown() override {
        if (pc_) {
            RTCPeerConnection_Close(pc_);
            RTCPeerConnectionFactory_DeletePeerConnection(factory_, pc_);
            RefCountedObject_Release(pc_);
            pc_ = nullptr;
        }
        if (factory_) {
            EXPECT_EQ(RTCPeerConnectionFactory_Terminate(factory_),
                      rtcBool32::kTrue);
            RefCountedObject_Release(factory_);
            factory_ = nullptr;
        }
        LibWebRTC_Terminate();
    }

    // Builds a live RTP parameters handle for an audio sender. Caller releases.
    rtcRtpParametersHandle GetAudioSenderParameters() {
        rtcRtpTransceiverHandle transceiver = nullptr;
        EXPECT_EQ(RTCPeerConnection_AddTransceiver1(pc_, RTCMediaType::AUDIO,
                                                    &transceiver),
                  rtcResultU4::kSuccess);
        if (!transceiver) return nullptr;

        rtcRtpSenderHandle sender = nullptr;
        EXPECT_EQ(RTCRtpTransceiver_GetSender(transceiver, &sender),
                  rtcResultU4::kSuccess);
        RefCountedObject_Release(transceiver);
        if (!sender) return nullptr;

        rtcRtpParametersHandle params = nullptr;
        EXPECT_EQ(RTCRtpSender_GetParameters(sender, &params),
                  rtcResultU4::kSuccess);
        RefCountedObject_Release(sender);
        return params;
    }

    rtcPeerConnectionFactoryHandle factory_ = nullptr;
    rtcPeerConnectionHandle pc_ = nullptr;
};

TEST_F(RTCRtpParametersTest, ParametersExposeTransactionId) {
    rtcRtpParametersHandle params = GetAudioSenderParameters();
    ASSERT_NE(params, nullptr);

    char txn[128] = {0};
    EXPECT_EQ(RTCRtpParameters_GetTransactionId(params, txn, sizeof(txn)),
              rtcResultU4::kSuccess);

    if (params) RefCountedObject_Release(params);
}

TEST_F(RTCRtpParametersTest, ParametersExposeCodecList) {
    rtcRtpParametersHandle params = GetAudioSenderParameters();
    ASSERT_NE(params, nullptr);

    rtcRtpCodecParametersListHandle codecs = nullptr;
    EXPECT_EQ(RTCRtpParameters_GetCodecs(params, &codecs),
              rtcResultU4::kSuccess);
    ASSERT_NE(codecs, nullptr);

    int count = RTCRtpCodecParametersList_GetCount(codecs);
    EXPECT_GE(count, 0);

    if (count > 0) {
        rtcRtpCodecParametersHandle codec = nullptr;
        EXPECT_EQ(RTCRtpCodecParametersList_GetItem(codecs, 0, &codec),
                  rtcResultU4::kSuccess);
        ASSERT_NE(codec, nullptr);

        char mime[128] = {0};
        EXPECT_EQ(RTCRtpCodecParameters_GetMimeType(codec, mime, sizeof(mime)),
                  rtcResultU4::kSuccess);

        int payload_type = -1;
        EXPECT_EQ(RTCRtpCodecParameters_GetPayloadType(codec, &payload_type),
                  rtcResultU4::kSuccess);

        int clock_rate = -1;
        EXPECT_EQ(RTCRtpCodecParameters_GetClockRate(codec, &clock_rate),
                  rtcResultU4::kSuccess);

        // Sub-object: the codec's RTCP feedback list.
        rtcRtcpFeedbackListHandle fb = nullptr;
        EXPECT_EQ(RTCRtpCodecParameters_GetRtcpFeedBacks(codec, &fb),
                  rtcResultU4::kSuccess);
        if (fb) {
            int fb_count = RTCRtcpFeedbackList_GetCount(fb);
            EXPECT_GE(fb_count, 0);
            if (fb_count > 0) {
                rtcRtcpFeedbackHandle one = nullptr;
                EXPECT_EQ(RTCRtcpFeedbackList_GetItem(fb, 0, &one),
                          rtcResultU4::kSuccess);
                if (one) {
                    rtcRtcpFeedbackType type{};
                    EXPECT_EQ(RTCRtcpFeedback_GetType(one, &type),
                              rtcResultU4::kSuccess);
                    RefCountedObject_Release(one);
                }
            }
            RefCountedObject_Release(fb);
        }

        // Sub-object: the codec's parameters map.
        rtcRtpCodecParametersMapHandle map = nullptr;
        EXPECT_EQ(RTCRtpCodecParameters_GetParameters(codec, &map),
                  rtcResultU4::kSuccess);
        if (map) {
            EXPECT_GE(RTCRtpCodecParametersMap_GetCount(map), 0);
            RefCountedObject_Release(map);
        }

        if (codec) RefCountedObject_Release(codec);
    }

    // Out-of-range index.
    rtcRtpCodecParametersHandle oob = nullptr;
    EXPECT_EQ(RTCRtpCodecParametersList_GetItem(codecs, count, &oob),
              rtcResultU4::kOutOfRange);
    EXPECT_EQ(oob, nullptr);

    if (codecs) RefCountedObject_Release(codecs);
    if (params) RefCountedObject_Release(params);
}

TEST_F(RTCRtpParametersTest, ParametersExposeEncodingList) {
    rtcRtpParametersHandle params = GetAudioSenderParameters();
    ASSERT_NE(params, nullptr);

    rtcRtpEncodingParametersListHandle encodings = nullptr;
    EXPECT_EQ(RTCRtpParameters_GetEncodings(params, &encodings),
              rtcResultU4::kSuccess);
    ASSERT_NE(encodings, nullptr);

    int count = RTCRtpEncodingParametersList_GetCount(encodings);
    EXPECT_GE(count, 0);

    if (count > 0) {
        rtcRtpEncodingParametersHandle enc = nullptr;
        EXPECT_EQ(RTCRtpEncodingParametersList_GetItem(encodings, 0, &enc),
                  rtcResultU4::kSuccess);
        ASSERT_NE(enc, nullptr);

        rtcBool32 active = rtcBool32::kFalse;
        EXPECT_EQ(RTCRtpEncodingParameters_GetActive(enc, &active),
                  rtcResultU4::kSuccess);

        double bitrate_priority = 0.0;
        EXPECT_EQ(RTCRtpEncodingParameters_GetBitratePriority(
                      enc, &bitrate_priority),
                  rtcResultU4::kSuccess);

        if (enc) RefCountedObject_Release(enc);
    }

    // Out-of-range index.
    rtcRtpEncodingParametersHandle oob = nullptr;
    EXPECT_EQ(RTCRtpEncodingParametersList_GetItem(encodings, count, &oob),
              rtcResultU4::kOutOfRange);
    EXPECT_EQ(oob, nullptr);

    if (encodings) RefCountedObject_Release(encodings);
    if (params) RefCountedObject_Release(params);
}

TEST_F(RTCRtpParametersTest, ParametersExposeHeaderExtensionList) {
    rtcRtpParametersHandle params = GetAudioSenderParameters();
    ASSERT_NE(params, nullptr);

    rtcRtpExtensionListHandle exts = nullptr;
    EXPECT_EQ(RTCRtpParameters_GetHeaderExtensions(params, &exts),
              rtcResultU4::kSuccess);
    ASSERT_NE(exts, nullptr);

    int count = RTCRtpExtensionList_GetCount(exts);
    EXPECT_GE(count, 0);

    if (count > 0) {
        rtcRtpExtensionHandle ext = nullptr;
        EXPECT_EQ(RTCRtpExtensionList_GetItem(exts, 0, &ext),
                  rtcResultU4::kSuccess);
        ASSERT_NE(ext, nullptr);

        char uri[256] = {0};
        EXPECT_EQ(RTCRtpExtension_GetUri(ext, uri, sizeof(uri)),
                  rtcResultU4::kSuccess);

        int id = -1;
        EXPECT_EQ(RTCRtpExtension_GetId(ext, &id), rtcResultU4::kSuccess);

        if (ext) RefCountedObject_Release(ext);
    }

    if (exts) RefCountedObject_Release(exts);
    if (params) RefCountedObject_Release(params);
}

TEST_F(RTCRtpParametersTest, ParametersExposeRtcpParameters) {
    rtcRtpParametersHandle params = GetAudioSenderParameters();
    ASSERT_NE(params, nullptr);

    rtcRtcpParametersHandle rtcp = nullptr;
    EXPECT_EQ(RTCRtpParameters_GetRtcpParameters(params, &rtcp),
              rtcResultU4::kSuccess);

    if (rtcp) {
        unsigned int ssrc = 0;
        EXPECT_EQ(RTCRtcpParameters_GetSSRC(rtcp, &ssrc),
                  rtcResultU4::kSuccess);

        rtcBool32 reduced_size = rtcBool32::kFalse;
        EXPECT_EQ(RTCRtcpParameters_GetReducedSize(rtcp, &reduced_size),
                  rtcResultU4::kSuccess);

        rtcBool32 mux = rtcBool32::kFalse;
        EXPECT_EQ(RTCRtcpParameters_GetMux(rtcp, &mux), rtcResultU4::kSuccess);

        RefCountedObject_Release(rtcp);
    }

    if (params) RefCountedObject_Release(params);
}

TEST_F(RTCRtpParametersTest, ParametersExposeDegradationPreference) {
    rtcRtpParametersHandle params = GetAudioSenderParameters();
    ASSERT_NE(params, nullptr);

    rtcDegradationPreference pref{};
    EXPECT_EQ(RTCRtpParameters_GetDegradationPreference(params, &pref),
              rtcResultU4::kSuccess);

    if (params) RefCountedObject_Release(params);
}

}  // namespace libwebrtc
