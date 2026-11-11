#include <cstdint>
#include <memory>
#include <string>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_audio_processing.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_types.h"
#include "libwebrtc.h"

namespace libwebrtc {
namespace {

// Minimal CustomProcessing implementation that records the calls it receives.
class CountingProcessing : public RTCAudioProcessing::CustomProcessing {
 public:
  void Initialize(int /*sample_rate_hz*/, int /*num_channels*/) override {
    ++initialize_calls;
  }
  void Process(int /*num_bands*/, int /*num_frames*/, int /*buffer_size*/,
               float* /*buffer*/) override {
    ++process_calls;
  }
  void Reset(int /*new_rate*/) override { ++reset_calls; }
  void Release() override { ++release_calls; }

  int initialize_calls = 0;
  int process_calls = 0;
  int reset_calls = 0;
  int release_calls = 0;
};

}  // namespace

// Audio processing is obtained from a live peer connection factory.
class RTCAudioProcessingTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(LibWebRTC::Initialize());
    factory_ = LibWebRTC::CreateRTCPeerConnectionFactory();
    ASSERT_TRUE(factory_.get() != nullptr);
    ASSERT_TRUE(factory_->Initialize());
    processing_ = factory_->GetAudioProcessing();
    ASSERT_TRUE(processing_.get() != nullptr);
  }

  void TearDown() override {
    processing_ = nullptr;
    if (factory_) {
      factory_->Terminate();
      factory_ = nullptr;
    }
    LibWebRTC::Terminate();
  }

  scoped_refptr<RTCPeerConnectionFactory> factory_;
  scoped_refptr<RTCAudioProcessing> processing_;
};

TEST_F(RTCAudioProcessingTest, GetAudioProcessingReturnsSameInstance) {
  scoped_refptr<RTCAudioProcessing> again = factory_->GetAudioProcessing();
  EXPECT_TRUE(again.get() != nullptr);
}

TEST_F(RTCAudioProcessingTest, SetCapturePostProcessingAcceptsCustom) {
  CountingProcessing custom;
  processing_->SetCapturePostProcessing(&custom);
  // Detach before `custom` leaves scope so the adapter holds no dangling ptr.
  processing_->SetCapturePostProcessing(nullptr);
}

TEST_F(RTCAudioProcessingTest, SetRenderPreProcessingAcceptsCustom) {
  CountingProcessing custom;
  processing_->SetRenderPreProcessing(&custom);
  processing_->SetRenderPreProcessing(nullptr);
}

TEST_F(RTCAudioProcessingTest, ClearingProcessorsWithNullIsSafe) {
  // Detaching with nullptr must not dereference the (absent) processor.
  processing_->SetCapturePostProcessing(nullptr);
  processing_->SetRenderPreProcessing(nullptr);
}

}  // namespace libwebrtc
