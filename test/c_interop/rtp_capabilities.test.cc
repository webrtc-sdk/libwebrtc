#include <memory>
#include <string>
#include <vector>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "interop_api.h"

namespace libwebrtc {

// Negative-path tests that don't require an initialized factory.

TEST(RTCRtpCapabilitiesNegative, GetCodecsRejectsNullHandle) {
    rtcRtpCodecCapabilityListHandle list = nullptr;
    EXPECT_EQ(RTCRtpCapabilities_GetCodecs(nullptr, &list),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(list, nullptr);
}

TEST(RTCRtpCapabilitiesNegative, GetCodecsRejectsNullOutPointer) {
    // CHECK_OUT_POINTER runs before the handle check.
    EXPECT_EQ(RTCRtpCapabilities_GetCodecs(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCRtpCapabilitiesNegative, GetHeaderExtensionsRejectsNullHandle) {
    rtcRtpHeaderExtensionCapabilityListHandle list = nullptr;
    EXPECT_EQ(RTCRtpCapabilities_GetHeaderExtensions(nullptr, &list),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(list, nullptr);
}

TEST(RTCRtpCapabilitiesNegative, GetHeaderExtensionsRejectsNullOutPointer) {
    EXPECT_EQ(RTCRtpCapabilities_GetHeaderExtensions(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCRtpCapabilitiesNegative, CodecListGetCountWithNullHandleReturnsZero) {
    // DECLARE_LIST_GET_COUNT returns 0 for a null handle.
    EXPECT_EQ(RTCRtpCodecCapabilityList_GetCount(nullptr), 0);
}

TEST(RTCRtpCapabilitiesNegative,
     HeaderExtensionListGetCountWithNullHandleReturnsZero) {
    EXPECT_EQ(RTCRtpHeaderExtensionCapabilityList_GetCount(nullptr), 0);
}

TEST(RTCRtpCapabilitiesNegative, RtcpFeedbackListGetCountWithNullHandleReturnsZero) {
    EXPECT_EQ(RTCRtcpFeedbackList_GetCount(nullptr), 0);
}

TEST(RTCRtpCapabilitiesNegative, CodecListGetItemRejectsNullHandle) {
    rtcRtpCodecCapabilityHandle item = nullptr;
    EXPECT_EQ(RTCRtpCodecCapabilityList_GetItem(nullptr, 0, &item),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(item, nullptr);
}

TEST(RTCRtpCapabilitiesNegative, CodecListGetItemRejectsNullOutPointer) {
    EXPECT_EQ(RTCRtpCodecCapabilityList_GetItem(nullptr, 0, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCRtpCapabilitiesNegative, HeaderExtensionListGetItemRejectsNullHandle) {
    rtcRtpHeaderExtensionCapabilityHandle item = nullptr;
    EXPECT_EQ(RTCRtpHeaderExtensionCapabilityList_GetItem(nullptr, 0, &item),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(item, nullptr);
}

TEST(RTCRtpCapabilitiesNegative, CodecGetterRejectsNullHandle) {
    char buf[64] = {0};
    EXPECT_EQ(RTCRtpCodecCapability_GetMimeType(nullptr, buf, sizeof(buf)),
              rtcResultU4::kInvalidNativeHandle);

    int value = -1;
    EXPECT_EQ(RTCRtpCodecCapability_GetClockRate(nullptr, &value),
              rtcResultU4::kInvalidNativeHandle);
    EXPECT_EQ(RTCRtpCodecCapability_GetChannels(nullptr, &value),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpCapabilitiesNegative, CodecGetterRejectsNullValuePointer) {
    // DECLARE_GET_VALUE runs CHECK_POINTER on the out value first.
    EXPECT_EQ(RTCRtpCodecCapability_GetClockRate(nullptr, nullptr),
              rtcResultU4::kInvalidPointer);
}

TEST(RTCRtpCapabilitiesNegative, HeaderExtensionGetterRejectsNullHandle) {
    char buf[64] = {0};
    EXPECT_EQ(RTCRtpHeaderExtensionCapability_GetUri(nullptr, buf, sizeof(buf)),
              rtcResultU4::kInvalidNativeHandle);

    int id = -1;
    EXPECT_EQ(RTCRtpHeaderExtensionCapability_GetPreferredId(nullptr, &id),
              rtcResultU4::kInvalidNativeHandle);

    rtcBool32 enc = rtcBool32::kTrue;
    EXPECT_EQ(RTCRtpHeaderExtensionCapability_GetPreferredEncrypt(nullptr, &enc),
              rtcResultU4::kInvalidNativeHandle);
}

TEST(RTCRtpCapabilitiesNegative, RtcpFeedbackGetterRejectsNullHandle) {
    rtcRtcpFeedbackType type{};
    EXPECT_EQ(RTCRtcpFeedback_GetType(nullptr, &type),
              rtcResultU4::kInvalidNativeHandle);

    rtcRtcpFeedbackMessageType msg{};
    EXPECT_EQ(RTCRtcpFeedback_GetMessageType(nullptr, &msg),
              rtcResultU4::kInvalidNativeHandle);
}

// Fixture that owns an initialized factory for the duration of each test.
class RTCRtpCapabilitiesTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASSERT_EQ(LibWebRTC_Initialize(), rtcBool32::kTrue);
        factory_ = LibWebRTC_CreateRTCPeerConnectionFactory();
        ASSERT_NE(factory_, nullptr);
        ASSERT_EQ(RTCPeerConnectionFactory_Initialize(factory_),
                  rtcBool32::kTrue);
    }

    void TearDown() override {
        if (factory_) {
            EXPECT_EQ(RTCPeerConnectionFactory_Terminate(factory_),
                      rtcBool32::kTrue);
            RefCountedObject_Release(factory_);
            factory_ = nullptr;
        }
        LibWebRTC_Terminate();
    }

    // Returns a real RTP sender capabilities handle (caller releases it).
    rtcRtpCapabilitiesHandle GetSenderCapabilities(rtcMediaType media_type) {
        rtcRtpCapabilitiesHandle caps = nullptr;
        EXPECT_EQ(RTCPeerConnectionFactory_GetRtpSenderCapabilities(
                      factory_, media_type, &caps),
                  rtcResultU4::kSuccess);
        return caps;
    }

    rtcPeerConnectionFactoryHandle factory_ = nullptr;
};

TEST_F(RTCRtpCapabilitiesTest, AudioCapabilitiesExposeCodecList) {
    rtcRtpCapabilitiesHandle caps = GetSenderCapabilities(RTCMediaType::AUDIO);
    ASSERT_NE(caps, nullptr);

    rtcRtpCodecCapabilityListHandle codecs = nullptr;
    EXPECT_EQ(RTCRtpCapabilities_GetCodecs(caps, &codecs),
              rtcResultU4::kSuccess);
    ASSERT_NE(codecs, nullptr);

    int count = RTCRtpCodecCapabilityList_GetCount(codecs);
    EXPECT_GT(count, 0);

    // Valid index: first codec.
    rtcRtpCodecCapabilityHandle codec = nullptr;
    EXPECT_EQ(RTCRtpCodecCapabilityList_GetItem(codecs, 0, &codec),
              rtcResultU4::kSuccess);
    ASSERT_NE(codec, nullptr);

    char mime[128] = {0};
    EXPECT_EQ(RTCRtpCodecCapability_GetMimeType(codec, mime, sizeof(mime)),
              rtcResultU4::kSuccess);
    EXPECT_GT(std::string(mime).size(), 0u);

    int clock_rate = -1;
    EXPECT_EQ(RTCRtpCodecCapability_GetClockRate(codec, &clock_rate),
              rtcResultU4::kSuccess);

    int channels = -1;
    EXPECT_EQ(RTCRtpCodecCapability_GetChannels(codec, &channels),
              rtcResultU4::kSuccess);

    char fmtp[256] = {0};
    EXPECT_EQ(RTCRtpCodecCapability_GetSdpFmtpLine(codec, fmtp, sizeof(fmtp)),
              rtcResultU4::kSuccess);

    // Out-of-range and negative indices.
    rtcRtpCodecCapabilityHandle oob = nullptr;
    EXPECT_EQ(RTCRtpCodecCapabilityList_GetItem(codecs, count, &oob),
              rtcResultU4::kOutOfRange);
    EXPECT_EQ(oob, nullptr);
    EXPECT_EQ(RTCRtpCodecCapabilityList_GetItem(codecs, -1, &oob),
              rtcResultU4::kOutOfRange);
    EXPECT_EQ(oob, nullptr);

    if (codec) RefCountedObject_Release(codec);
    if (codecs) RefCountedObject_Release(codecs);
    if (caps) RefCountedObject_Release(caps);
}

TEST_F(RTCRtpCapabilitiesTest, VideoCapabilitiesExposeCodecList) {
    rtcRtpCapabilitiesHandle caps = GetSenderCapabilities(RTCMediaType::VIDEO);
    ASSERT_NE(caps, nullptr);

    rtcRtpCodecCapabilityListHandle codecs = nullptr;
    EXPECT_EQ(RTCRtpCapabilities_GetCodecs(caps, &codecs),
              rtcResultU4::kSuccess);
    ASSERT_NE(codecs, nullptr);

    int count = RTCRtpCodecCapabilityList_GetCount(codecs);
    EXPECT_GT(count, 0);

    rtcRtpCodecCapabilityHandle codec = nullptr;
    EXPECT_EQ(RTCRtpCodecCapabilityList_GetItem(codecs, 0, &codec),
              rtcResultU4::kSuccess);
    ASSERT_NE(codec, nullptr);

    char mime[128] = {0};
    EXPECT_EQ(RTCRtpCodecCapability_GetMimeType(codec, mime, sizeof(mime)),
              rtcResultU4::kSuccess);
    EXPECT_GT(std::string(mime).size(), 0u);

    if (codec) RefCountedObject_Release(codec);
    if (codecs) RefCountedObject_Release(codecs);
    if (caps) RefCountedObject_Release(caps);
}

TEST_F(RTCRtpCapabilitiesTest, CapabilitiesExposeHeaderExtensionList) {
    rtcRtpCapabilitiesHandle caps = GetSenderCapabilities(RTCMediaType::AUDIO);
    ASSERT_NE(caps, nullptr);

    rtcRtpHeaderExtensionCapabilityListHandle exts = nullptr;
    EXPECT_EQ(RTCRtpCapabilities_GetHeaderExtensions(caps, &exts),
              rtcResultU4::kSuccess);
    ASSERT_NE(exts, nullptr);

    int count = RTCRtpHeaderExtensionCapabilityList_GetCount(exts);
    EXPECT_GE(count, 0);

    if (count > 0) {
        rtcRtpHeaderExtensionCapabilityHandle ext = nullptr;
        EXPECT_EQ(RTCRtpHeaderExtensionCapabilityList_GetItem(exts, 0, &ext),
                  rtcResultU4::kSuccess);
        ASSERT_NE(ext, nullptr);

        char uri[256] = {0};
        EXPECT_EQ(RTCRtpHeaderExtensionCapability_GetUri(ext, uri, sizeof(uri)),
                  rtcResultU4::kSuccess);

        int preferred_id = -1;
        EXPECT_EQ(RTCRtpHeaderExtensionCapability_GetPreferredId(ext,
                                                                 &preferred_id),
                  rtcResultU4::kSuccess);

        rtcBool32 encrypt = rtcBool32::kTrue;
        EXPECT_EQ(
            RTCRtpHeaderExtensionCapability_GetPreferredEncrypt(ext, &encrypt),
            rtcResultU4::kSuccess);

        if (ext) RefCountedObject_Release(ext);
    }

    // Out-of-range index.
    rtcRtpHeaderExtensionCapabilityHandle oob = nullptr;
    EXPECT_EQ(RTCRtpHeaderExtensionCapabilityList_GetItem(exts, count, &oob),
              rtcResultU4::kOutOfRange);
    EXPECT_EQ(oob, nullptr);

    if (exts) RefCountedObject_Release(exts);
    if (caps) RefCountedObject_Release(caps);
}

}  // namespace libwebrtc
