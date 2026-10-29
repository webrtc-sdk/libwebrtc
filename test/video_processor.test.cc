#include <cstdint>
#include <cstring>
#include <vector>

#include "test/gtest.h"

#include "libwebrtc.h"
#include "rtc_mediaconstraints.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_video_frame.h"
#include "rtc_video_processor.h"
#include "rtc_video_renderer.h"
#include "rtc_video_source.h"
#include "rtc_video_track.h"

using libwebrtc::LibWebRTC;
using libwebrtc::RTCVideoFrame;
using libwebrtc::scoped_refptr;

namespace {

constexpr int kWidth = 16;
constexpr int kHeight = 16;

// A small I420 frame whose luma plane is filled with `luma`.
scoped_refptr<RTCVideoFrame> MakeFrame(uint8_t luma) {
  std::vector<uint8_t> y(kWidth * kHeight, luma);
  std::vector<uint8_t> uv((kWidth / 2) * (kHeight / 2), 128);
  return RTCVideoFrame::Create(kWidth, kHeight, y.data(), kWidth, uv.data(),
                               kWidth / 2, uv.data(), kWidth / 2);
}

uint8_t Luma(const scoped_refptr<RTCVideoFrame>& frame) {
  return frame->DataY()[0];
}

class LumaRenderer
    : public libwebrtc::RTCVideoRenderer<scoped_refptr<RTCVideoFrame>> {
 public:
  void OnFrame(scoped_refptr<RTCVideoFrame> frame) override {
    lumas.push_back(Luma(frame));
  }

  std::vector<uint8_t> lumas;
};

// Records the luma it sees and replaces the frame with one whose luma is
// `map(input luma)`. With no map it passes the frame through.
class LumaProcessor : public libwebrtc::RTCVideoProcessor {
 public:
  using Map = uint8_t (*)(uint8_t);
  explicit LumaProcessor(Map map = nullptr) : map_(map) {}

  scoped_refptr<RTCVideoFrame> Process(
      scoped_refptr<RTCVideoFrame> frame) override {
    seen.push_back(Luma(frame));
    if (!map_) {
      return frame;
    }
    return MakeFrame(map_(Luma(frame)));
  }

  std::vector<uint8_t> seen;

 private:
  Map map_;
};

class DroppingProcessor : public libwebrtc::RTCVideoProcessor {
 public:
  scoped_refptr<RTCVideoFrame> Process(scoped_refptr<RTCVideoFrame>) override {
    return nullptr;
  }
};

class VideoProcessorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(LibWebRTC::Initialize());
    factory_ = LibWebRTC::CreateRTCPeerConnectionFactory();
    ASSERT_TRUE(factory_->Initialize());
    source_ = factory_->CreateCustomVideoSource(
        "custom", libwebrtc::RTCMediaConstraints::Create());
    ASSERT_TRUE(source_.get() != nullptr);
    track_ = factory_->CreateVideoTrack(source_, "video");
    ASSERT_TRUE(track_.get() != nullptr);
    track_->AddRenderer(&renderer_);
  }

  void TearDown() override {
    track_->RemoveRenderer(&renderer_);
    track_ = nullptr;
    source_ = nullptr;
    factory_->Terminate();
    factory_ = nullptr;
    LibWebRTC::Terminate();
  }

  scoped_refptr<libwebrtc::RTCPeerConnectionFactory> factory_;
  scoped_refptr<libwebrtc::RTCVideoSource> source_;
  scoped_refptr<libwebrtc::RTCVideoTrack> track_;
  LumaRenderer renderer_;
};

TEST_F(VideoProcessorTest, WithoutProcessorsFramesAreUnchanged) {
  source_->OnCapturedFrame(MakeFrame(10));

  EXPECT_EQ(renderer_.lumas, std::vector<uint8_t>({10}));
}

TEST_F(VideoProcessorTest, ReplacedFrameReachesRenderer) {
  LumaProcessor processor([](uint8_t) -> uint8_t { return 200; });
  source_->AddProcessor(&processor);

  source_->OnCapturedFrame(MakeFrame(10));

  EXPECT_EQ(processor.seen, std::vector<uint8_t>({10}));
  EXPECT_EQ(renderer_.lumas, std::vector<uint8_t>({200}));
  source_->RemoveProcessor(&processor);
}

TEST_F(VideoProcessorTest, PassThroughKeepsFrame) {
  LumaProcessor processor;
  source_->AddProcessor(&processor);

  source_->OnCapturedFrame(MakeFrame(10));

  EXPECT_EQ(processor.seen, std::vector<uint8_t>({10}));
  EXPECT_EQ(renderer_.lumas, std::vector<uint8_t>({10}));
  source_->RemoveProcessor(&processor);
}

TEST_F(VideoProcessorTest, ProcessorsRunInOrderAdded) {
  LumaProcessor add_one([](uint8_t luma) -> uint8_t { return luma + 1; });
  LumaProcessor twice([](uint8_t luma) -> uint8_t { return luma * 2; });
  source_->AddProcessor(&add_one);
  source_->AddProcessor(&twice);

  source_->OnCapturedFrame(MakeFrame(10));

  EXPECT_EQ(twice.seen, std::vector<uint8_t>({11}));
  EXPECT_EQ(renderer_.lumas, std::vector<uint8_t>({22}));
  source_->RemoveProcessor(&twice);
  source_->RemoveProcessor(&add_one);
}

TEST_F(VideoProcessorTest, NullResultDropsFrame) {
  DroppingProcessor dropper;
  LumaProcessor after;
  source_->AddProcessor(&dropper);
  source_->AddProcessor(&after);

  source_->OnCapturedFrame(MakeFrame(10));

  EXPECT_TRUE(after.seen.empty());
  EXPECT_TRUE(renderer_.lumas.empty());
  source_->RemoveProcessor(&after);
  source_->RemoveProcessor(&dropper);
}

TEST_F(VideoProcessorTest, RemovedProcessorIsNotCalled) {
  LumaProcessor processor([](uint8_t) -> uint8_t { return 200; });
  source_->AddProcessor(&processor);
  source_->OnCapturedFrame(MakeFrame(10));
  source_->RemoveProcessor(&processor);

  source_->OnCapturedFrame(MakeFrame(20));

  EXPECT_EQ(processor.seen, std::vector<uint8_t>({10}));
  EXPECT_EQ(renderer_.lumas, std::vector<uint8_t>({200, 20}));
}

}  // namespace
