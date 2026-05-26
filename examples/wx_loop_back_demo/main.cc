// Simple wxWidgets demo that exercises libwebrtc's loopback path through the
// C++ API: two in-process RTCPeerConnections negotiate over a fake signaling
// channel and stream a single audio track from one to the other.

#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/wx.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <thread>
#include <vector>

#include "libwebrtc.h"
#include "rtc_audio_source.h"
#include "rtc_audio_track.h"
#include "rtc_data_channel.h"
#include "rtc_ice_candidate.h"
#include "rtc_media_stream.h"
#include "rtc_media_track.h"
#include "rtc_mediaconstraints.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_rtp_receiver.h"
#include "rtc_rtp_sender.h"
#include "rtc_rtp_transceiver.h"
#include "rtc_video_device.h"
#include "rtc_video_frame.h"
#include "rtc_video_renderer.h"
#include "rtc_video_source.h"
#include "rtc_video_track.h"

using libwebrtc::scoped_refptr;
using libwebrtc::string;

namespace {

// Marshals log messages to the wxTextCtrl on the UI thread regardless of
// which libwebrtc thread emits them.
class UiLogger {
 public:
  explicit UiLogger(wxTextCtrl* sink) : sink_(sink) {}

  void Log(const wxString& message) {
    wxTextCtrl* sink = sink_;
    if (wxIsMainThread()) {
      sink->AppendText(message + "\n");
    } else {
      wxString copy = message;
      sink->GetEventHandler()->CallAfter(
          [sink, copy] { sink->AppendText(copy + "\n"); });
    }
  }

 private:
  wxTextCtrl* sink_;
};

UiLogger* g_logger = nullptr;

void Log(const char* fmt, ...) {
  if (!g_logger) return;
  char buf[1024];
  va_list ap;
  va_start(ap, fmt);
  std::vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  g_logger->Log(wxString::FromUTF8(buf));
}

// Enum -> human-readable name. Keeps log lines greppable.
const char* ToString(libwebrtc::RTCSignalingState s) {
  switch (s) {
    case libwebrtc::RTCSignalingStateStable:
      return "Stable";
    case libwebrtc::RTCSignalingStateHaveLocalOffer:
      return "HaveLocalOffer";
    case libwebrtc::RTCSignalingStateHaveRemoteOffer:
      return "HaveRemoteOffer";
    case libwebrtc::RTCSignalingStateHaveLocalPrAnswer:
      return "HaveLocalPrAnswer";
    case libwebrtc::RTCSignalingStateHaveRemotePrAnswer:
      return "HaveRemotePrAnswer";
    case libwebrtc::RTCSignalingStateClosed:
      return "Closed";
  }
  return "?";
}

const char* ToString(libwebrtc::RTCPeerConnectionState s) {
  switch (s) {
    case libwebrtc::RTCPeerConnectionStateNew:
      return "New";
    case libwebrtc::RTCPeerConnectionStateConnecting:
      return "Connecting";
    case libwebrtc::RTCPeerConnectionStateConnected:
      return "Connected";
    case libwebrtc::RTCPeerConnectionStateDisconnected:
      return "Disconnected";
    case libwebrtc::RTCPeerConnectionStateFailed:
      return "Failed";
    case libwebrtc::RTCPeerConnectionStateClosed:
      return "Closed";
  }
  return "?";
}

const char* ToString(libwebrtc::RTCIceGatheringState s) {
  switch (s) {
    case libwebrtc::RTCIceGatheringStateNew:
      return "New";
    case libwebrtc::RTCIceGatheringStateGathering:
      return "Gathering";
    case libwebrtc::RTCIceGatheringStateComplete:
      return "Complete";
  }
  return "?";
}

const char* ToString(libwebrtc::RTCIceConnectionState s) {
  switch (s) {
    case libwebrtc::RTCIceConnectionStateNew:
      return "New";
    case libwebrtc::RTCIceConnectionStateChecking:
      return "Checking";
    case libwebrtc::RTCIceConnectionStateCompleted:
      return "Completed";
    case libwebrtc::RTCIceConnectionStateConnected:
      return "Connected";
    case libwebrtc::RTCIceConnectionStateFailed:
      return "Failed";
    case libwebrtc::RTCIceConnectionStateDisconnected:
      return "Disconnected";
    case libwebrtc::RTCIceConnectionStateClosed:
      return "Closed";
    case libwebrtc::RTCIceConnectionStateMax:
      return "Max";
  }
  return "?";
}

const char* ToString(libwebrtc::RTCMediaType t) {
  switch (t) {
    case libwebrtc::RTCMediaType::AUDIO:
      return "AUDIO";
    case libwebrtc::RTCMediaType::VIDEO:
      return "VIDEO";
    case libwebrtc::RTCMediaType::DATA:
      return "DATA";
    case libwebrtc::RTCMediaType::UNSUPPORTED:
      return "UNSUPPORTED";
  }
  return "?";
}

class PeerSlot;

// Custom panel that displays the latest decoded video frame, scaled to fit
// the panel client area. The image is updated from the libwebrtc decoder
// thread via UpdateFrame; the actual rasterisation happens in OnPaint on the
// UI thread.
class VideoRenderPanel : public wxPanel {
 public:
  VideoRenderPanel(wxWindow* parent)
      : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(320, 240)) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(*wxBLACK);
    Bind(wxEVT_PAINT, &VideoRenderPanel::OnPaint, this);
  }

  // Thread-safe: takes the wxImage by value, marshals to UI thread.
  void UpdateFrame(wxImage image) {
    if (wxIsMainThread()) {
      latest_image_ = std::move(image);
      Refresh(false);
      return;
    }
    wxImage captured = std::move(image);
    GetEventHandler()->CallAfter(
        [this, captured = std::move(captured)]() mutable {
          latest_image_ = std::move(captured);
          Refresh(false);
        });
  }

 private:
  void OnPaint(wxPaintEvent&) {
    wxPaintDC dc(this);
    wxSize sz = GetClientSize();
    if (sz.x <= 0 || sz.y <= 0) return;
    if (!latest_image_.IsOk()) {
      dc.SetBackground(*wxBLACK_BRUSH);
      dc.Clear();
      return;
    }
    wxImage scaled =
        (latest_image_.GetWidth() != sz.x || latest_image_.GetHeight() != sz.y)
            ? latest_image_.Scale(sz.x, sz.y, wxIMAGE_QUALITY_NEAREST)
            : latest_image_;
    dc.DrawBitmap(wxBitmap(scaled), 0, 0);
  }

  wxImage latest_image_;  // touched only on UI thread
};

// RTCVideoRenderer adapter that converts each decoded I420 frame into a
// wxImage and hands it to a VideoRenderPanel for display. Also keeps a
// running count for diagnostic logging.
class VideoSinkRenderer : public libwebrtc::RTCVideoRenderer<
                              scoped_refptr<libwebrtc::RTCVideoFrame>> {
 public:
  explicit VideoSinkRenderer(VideoRenderPanel* panel) : panel_(panel) {}

  void OnFrame(scoped_refptr<libwebrtc::RTCVideoFrame> frame) override {
    if (!frame || !panel_) return;
    const int w = frame->width();
    const int h = frame->height();
    if (w <= 0 || h <= 0) return;

    // libyuv::I420ToABGR places bytes in memory as R, G, B, A — exactly
    // the order wxImage's RGB buffer wants for the first three.
    std::vector<uint8_t> abgr(static_cast<size_t>(w) * h * 4);
    frame->ConvertToARGB(libwebrtc::RTCVideoFrame::Type::kABGR, abgr.data(),
                         w * 4, w, h);

    // wxImage requires its data pointer to be malloc()'d so it can free()
    // on destruction. Strip the alpha channel as we copy.
    unsigned char* rgb = static_cast<unsigned char*>(
        std::malloc(static_cast<size_t>(w) * h * 3));
    if (!rgb) return;
    for (int i = 0; i < w * h; ++i) {
      rgb[i * 3 + 0] = abgr[i * 4 + 0];
      rgb[i * 3 + 1] = abgr[i * 4 + 1];
      rgb[i * 3 + 2] = abgr[i * 4 + 2];
    }
    wxImage image(w, h, rgb);  // takes ownership

    size_t n = ++count_;
    if (n == 1 || n % 30 == 0) {
      Log("[remote video] frame #%zu: %dx%d", n, w, h);
    }
    panel_->UpdateFrame(std::move(image));
  }

 private:
  VideoRenderPanel* panel_;
  std::atomic<size_t> count_{0};
};

class LoopbackObserver;  // defined below; PeerSlot only needs the type
                         // for unique_ptr storage.

// Holds the state of a single peer in the loopback pair.
class PeerSlot {
 public:
  PeerSlot(const char* tag) : tag_(tag) {}
  // Defined out-of-line below — needed because `observer` is a
  // unique_ptr<LoopbackObserver> and LoopbackObserver is incomplete here.
  ~PeerSlot() = default;

  const char* tag() const { return tag_; }

  scoped_refptr<libwebrtc::RTCPeerConnection> pc;
  PeerSlot* remote = nullptr;
  std::atomic<bool> closed{false};
  std::unique_ptr<LoopbackObserver> observer;

  // Filled in when the remote side receives a video track. The track ref
  // keeps the underlying object alive for as long as we have a renderer
  // attached to it.
  scoped_refptr<libwebrtc::RTCVideoTrack> remote_video_track;
  std::unique_ptr<VideoSinkRenderer> remote_video_renderer;

 private:
  const char* tag_;
};

// Observer attached to one peer; forwards ICE candidates to the loopback peer
// and reports state changes to the UI log. Methods are defined inline because
// PeerSlot is now a complete type at this point.
class LoopbackObserver : public libwebrtc::RTCPeerConnectionObserver {
 public:
  LoopbackObserver(PeerSlot* slot, VideoRenderPanel* render_panel = nullptr)
      : slot_(slot), render_panel_(render_panel) {}

  void OnSignalingState(libwebrtc::RTCSignalingState state) override {
    Log("[%s] signaling state -> %s", slot_->tag(), ToString(state));
  }

  void OnPeerConnectionState(libwebrtc::RTCPeerConnectionState state) override {
    Log("[%s] pc state -> %s", slot_->tag(), ToString(state));
  }

  void OnIceGatheringState(libwebrtc::RTCIceGatheringState state) override {
    Log("[%s] ice gathering -> %s", slot_->tag(), ToString(state));
  }

  void OnIceConnectionState(libwebrtc::RTCIceConnectionState state) override {
    Log("[%s] ice connection -> %s", slot_->tag(), ToString(state));
  }

  void OnIceCandidate(
      scoped_refptr<libwebrtc::RTCIceCandidate> candidate) override {
    if (!candidate) return;
    PeerSlot* remote = slot_->remote;
    if (!remote || remote->closed.load() || !remote->pc) return;

    Log("[%s] -> [%s] candidate (mid=%s, mline=%d)", slot_->tag(),
        remote->tag(), candidate->sdp_mid().c_string(),
        candidate->sdp_mline_index());

    remote->pc->AddCandidate(candidate->sdp_mid(), candidate->sdp_mline_index(),
                             candidate->candidate());
  }

  void OnAddStream(
      scoped_refptr<libwebrtc::RTCMediaStream> /*stream*/) override {}
  void OnRemoveStream(
      scoped_refptr<libwebrtc::RTCMediaStream> /*stream*/) override {}
  void OnDataChannel(scoped_refptr<libwebrtc::RTCDataChannel> /*dc*/) override {
  }
  void OnRenegotiationNeeded() override {}
  void OnTrack(scoped_refptr<libwebrtc::RTCRtpTransceiver> /*t*/) override {}

  void OnAddTrack(
      libwebrtc::vector<scoped_refptr<libwebrtc::RTCMediaStream>> /*streams*/,
      scoped_refptr<libwebrtc::RTCRtpReceiver> receiver) override {
    if (!receiver) return;
    Log("[%s] OnAddTrack (kind=%s)", slot_->tag(),
        ToString(receiver->media_type()));

    if (receiver->media_type() != libwebrtc::RTCMediaType::VIDEO) return;

    scoped_refptr<libwebrtc::RTCMediaTrack> media = receiver->track();
    if (!media) return;

    // Down-cast: when media_type is VIDEO the underlying track is a
    // RTCVideoTrack, which is the only subclass of RTCMediaTrack that
    // exposes AddRenderer/RemoveRenderer.
    auto* video_ptr = static_cast<libwebrtc::RTCVideoTrack*>(media.get());
    slot_->remote_video_track =
        scoped_refptr<libwebrtc::RTCVideoTrack>(video_ptr);
    slot_->remote_video_renderer =
        std::make_unique<VideoSinkRenderer>(render_panel_);
    video_ptr->AddRenderer(slot_->remote_video_renderer.get());
    Log("[%s] attached video sink renderer", slot_->tag());
  }

  void OnRemoveTrack(
      scoped_refptr<libwebrtc::RTCRtpReceiver> /*receiver*/) override {}

 private:
  PeerSlot* slot_;
  VideoRenderPanel* render_panel_;
};

}  // namespace

class MainFrame : public wxFrame {
 public:
  MainFrame()
      : wxFrame(nullptr, wxID_ANY, "libwebrtc loopback demo (C++ API)",
                wxDefaultPosition, wxSize(960, 600)),
        offerer_("offerer"),
        answerer_("answerer") {
    // wxImage codec table is empty by default — register at least JPEG/PNG
    // handlers so wxBitmap conversions used by VideoRenderPanel work cleanly.
    wxInitAllImageHandlers();

    auto* panel = new wxPanel(this);
    auto* root = new wxBoxSizer(wxVERTICAL);

    auto* buttons = new wxBoxSizer(wxHORIZONTAL);
    start_btn_ = new wxButton(panel, wxID_ANY, "Start loopback");
    stop_btn_ = new wxButton(panel, wxID_ANY, "Stop");
    stop_btn_->Disable();
    buttons->Add(start_btn_, 0, wxALL, 5);
    buttons->Add(stop_btn_, 0, wxALL, 5);

    status_ = new wxStaticText(panel, wxID_ANY, "Idle.");

    auto* split = new wxBoxSizer(wxHORIZONTAL);
    video_panel_ = new VideoRenderPanel(panel);
    log_ = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition,
                          wxDefaultSize,
                          wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP);
    split->Add(video_panel_, 1, wxEXPAND | wxALL, 5);
    split->Add(log_, 1, wxEXPAND | wxALL, 5);

    root->Add(buttons, 0, wxEXPAND);
    root->Add(status_, 0, wxALL, 5);
    root->Add(split, 1, wxEXPAND);
    panel->SetSizerAndFit(root);

    start_btn_->Bind(wxEVT_BUTTON, &MainFrame::OnStart, this);
    stop_btn_->Bind(wxEVT_BUTTON, &MainFrame::OnStop, this);
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);

    g_logger = new UiLogger(log_);
  }

  ~MainFrame() override {
    TeardownLoopback();
    delete g_logger;
    g_logger = nullptr;
  }

 private:
  void OnStart(wxCommandEvent&) {
    if (running_) return;
    status_->SetLabel("Starting...");
    Log("LibWebRTC::Initialize");
    if (!libwebrtc::LibWebRTC::Initialize()) {
      status_->SetLabel("LibWebRTC::Initialize failed");
      return;
    }

    factory_ = libwebrtc::LibWebRTC::CreateRTCPeerConnectionFactory();
    if (!factory_ || !factory_->Initialize()) {
      status_->SetLabel("Factory init failed");
      TeardownLoopback();
      return;
    }

    offerer_.remote = &answerer_;
    answerer_.remote = &offerer_;
    offerer_.closed.store(false);
    answerer_.closed.store(false);

    libwebrtc::RTCConfiguration config{};
    scoped_refptr<libwebrtc::RTCMediaConstraints> pc_constraints =
        libwebrtc::RTCMediaConstraints::Create();

    offerer_.pc = factory_->Create(config, pc_constraints);
    answerer_.pc = factory_->Create(config, pc_constraints);
    if (!offerer_.pc || !answerer_.pc) {
      status_->SetLabel("Create RTCPeerConnection failed");
      TeardownLoopback();
      return;
    }

    offerer_.observer = std::make_unique<LoopbackObserver>(&offerer_);
    // The answerer is the one receiving the video — give it the panel so
    // its OnAddTrack can hook the renderer up for display.
    answerer_.observer =
        std::make_unique<LoopbackObserver>(&answerer_, video_panel_);
    offerer_.pc->RegisterRTCPeerConnectionObserver(offerer_.observer.get());
    answerer_.pc->RegisterRTCPeerConnectionObserver(answerer_.observer.get());

    scoped_refptr<libwebrtc::RTCAudioSource> audio_source =
        factory_->CreateAudioSource("loopback-audio");
    if (!audio_source) {
      status_->SetLabel("CreateAudioSource failed");
      TeardownLoopback();
      return;
    }
    scoped_refptr<libwebrtc::RTCAudioTrack> audio_track =
        factory_->CreateAudioTrack(audio_source, "loopback-track-0");
    if (!audio_track) {
      status_->SetLabel("CreateAudioTrack failed");
      TeardownLoopback();
      return;
    }

    std::vector<string> stream_ids;
    stream_ids.push_back(string("loopback-stream"));
    offerer_.pc->AddTrack(audio_track, stream_ids);

    // Synthetic video track: a custom (no-device) capturer is created and
    // the source is fed by a producer thread that emits a 640x480 frame
    // cycling through red / green / blue / white / black, one full-screen
    // colour per second.

    scoped_refptr<libwebrtc::RTCMediaConstraints> video_constraints =
        libwebrtc::RTCMediaConstraints::Create();
    video_source_ =
        factory_->CreateCustomVideoSource("synthetic-video", video_constraints);
    if (video_source_) {
      local_video_track_ =
          factory_->CreateVideoTrack(video_source_, "synthetic-video-0");
    }
    if (local_video_track_) {
      offerer_.pc->AddTrack(local_video_track_, stream_ids);
      Log("Pushing 640x480 synthetic frames at 30 fps");
      pusher_running_.store(true);
      scoped_refptr<libwebrtc::RTCVideoSource> src = video_source_;
      pusher_thread_ = std::thread([this, src] { PushSyntheticFrames(src); });
    } else {
      Log("CreateVideoTrack failed; continuing audio-only");
    }

    scoped_refptr<libwebrtc::RTCMediaConstraints> sdp_constraints =
        libwebrtc::RTCMediaConstraints::Create();

    // Kick off the negotiation: offerer creates the offer, the success
    // callback feeds it into both sides and asks the answerer for an
    // answer, which in turn is fed back into both sides.
    offerer_.pc->CreateOffer(
        [this, sdp_constraints](const string sdp, const string type) {
          Log("[offerer] CreateOffer ok, len=%zu", std::strlen(sdp.c_string()));
          offerer_.pc->SetLocalDescription(
              sdp, type, []() {}, OnSetSdpFailureForTag("offerer"));
          answerer_.pc->SetRemoteDescription(
              sdp, type, []() {}, OnSetSdpFailureForTag("answerer"));
          answerer_.pc->CreateAnswer(
              [this](const string sdp, const string type) {
                Log("[answerer] CreateAnswer ok, len=%zu",
                    std::strlen(sdp.c_string()));
                answerer_.pc->SetLocalDescription(
                    sdp, type, []() {}, OnSetSdpFailureForTag("answerer"));
                offerer_.pc->SetRemoteDescription(
                    sdp, type, []() {}, OnSetSdpFailureForTag("offerer"));
              },
              OnSdpCreateFailureForTag("answerer"), sdp_constraints);
        },
        OnSdpCreateFailureForTag("offerer"), sdp_constraints);

    running_ = true;
    start_btn_->Disable();
    stop_btn_->Enable();
    status_->SetLabel("Loopback running. See log below.");
  }

  void OnStop(wxCommandEvent&) {
    TeardownLoopback();
    status_->SetLabel("Stopped.");
    start_btn_->Enable();
    stop_btn_->Disable();
  }

  void OnClose(wxCloseEvent& event) {
    TeardownLoopback();
    event.Skip();
  }

  void TeardownLoopback() {
    offerer_.closed.store(true);
    answerer_.closed.store(true);

    // Stop the synthetic frame producer before anything else so a frame
    // that's mid-OnCapturedFrame finishes against still-live state.
    if (pusher_running_.exchange(false)) {
      if (pusher_thread_.joinable()) pusher_thread_.join();
    }

    // Detach renderers so we drop the dangling pointer before the unique_ptr
    // resets.
    for (PeerSlot* slot : {&offerer_, &answerer_}) {
      if (slot->remote_video_track && slot->remote_video_renderer) {
        slot->remote_video_track->RemoveRenderer(
            slot->remote_video_renderer.get());
      }
      slot->remote_video_track = nullptr;
      slot->remote_video_renderer.reset();
    }

    local_video_track_ = nullptr;
    video_source_ = nullptr;

    if (offerer_.pc) {
      offerer_.pc->Close();
      if (factory_) factory_->Delete(offerer_.pc);
      offerer_.pc = nullptr;
    }
    if (answerer_.pc) {
      answerer_.pc->Close();
      if (factory_) factory_->Delete(answerer_.pc);
      answerer_.pc = nullptr;
    }
    offerer_.observer.reset();
    answerer_.observer.reset();

    if (factory_) {
      factory_->Terminate();
      factory_ = nullptr;
      libwebrtc::LibWebRTC::Terminate();
    }
    running_ = false;
  }

  static libwebrtc::OnSdpCreateFailure OnSdpCreateFailureForTag(
      const char* tag) {
    return [tag](const char* error) {
      Log("[%s] SDP create failure: %s", tag, error ? error : "(null)");
    };
  }

  static libwebrtc::OnSetSdpFailure OnSetSdpFailureForTag(const char* tag) {
    return [tag](const char* error) {
      Log("[%s] SetDescription failure: %s", tag, error ? error : "(null)");
    };
  }

  wxButton* start_btn_;
  wxButton* stop_btn_;
  wxStaticText* status_;
  wxTextCtrl* log_;
  VideoRenderPanel* video_panel_ = nullptr;

  // Generates a 640x480 I420 frame containing 8 vertical SMPTE-style colour
  // bars that scroll horizontally a few pixels per frame, and pushes it
  // through RTCVideoSource::OnCapturedFrame at ~30 fps. Holds a strong ref
  // to the source so the source can't be destroyed mid-call.
  void PushSyntheticFrames(scoped_refptr<libwebrtc::RTCVideoSource> source) {
    constexpr int kWidth = 640;
    constexpr int kHeight = 480;
    constexpr int kYSize = kWidth * kHeight;
    constexpr int kUVStride = kWidth / 2;
    constexpr int kUVHeight = kHeight / 2;
    constexpr int kUVSize = kUVStride * kUVHeight;
    constexpr int kFrameBytes = kYSize + 2 * kUVSize;  // I420 packed
    constexpr int kFps = 30;
    constexpr auto kFrameInterval = std::chrono::microseconds(1'000'000 / kFps);

    constexpr int kBars = 8;
    constexpr int kBarWidth = kWidth / kBars;  // 80 px
    constexpr int kScrollPxPerFrame = 4;       // ~ 120 px/sec

    // BT.601 limited-range YUV for the eight reference colour bars,
    // ordered the way SMPTE 75 % bars are typically drawn left to right.
    struct YUV {
      uint8_t y, u, v;
    };
    constexpr YUV palette[kBars] = {
        {235, 128, 128},  // white
        {210, 16, 146},   // yellow
        {170, 166, 16},   // cyan
        {145, 54, 34},    // green
        {106, 202, 222},  // magenta
        {81, 90, 240},    // red
        {41, 240, 110},   // blue
        {16, 128, 128},   // black
    };

    std::vector<uint8_t> buffer(kFrameBytes);
    uint8_t* y_plane = buffer.data();
    uint8_t* u_plane = y_plane + kYSize;
    uint8_t* v_plane = u_plane + kUVSize;

    // The bar pattern is constant per row, so we build one row of Y, U,
    // V samples and memcpy it to every scanline. Far cheaper than a 2D
    // nested loop, and keeps the cost trivial at 640x480 / 30fps.
    std::vector<uint8_t> y_row(kWidth);
    std::vector<uint8_t> u_row(kUVStride);
    std::vector<uint8_t> v_row(kUVStride);

    int frame_idx = 0;
    auto next_tick = std::chrono::steady_clock::now();

    while (pusher_running_.load()) {
      int scroll = (frame_idx * kScrollPxPerFrame) % kWidth;

      // Build one Y row: each column picks its bar from the scrolled
      // pattern.
      for (int x = 0; x < kWidth; ++x) {
        int bar = ((x + scroll) % kWidth) / kBarWidth;
        y_row[x] = palette[bar].y;
      }
      // Build one U/V row at half horizontal resolution. Sample at
      // x*2 to match the I420 chroma siting.
      for (int x = 0; x < kUVStride; ++x) {
        int bar = ((x * 2 + scroll) % kWidth) / kBarWidth;
        u_row[x] = palette[bar].u;
        v_row[x] = palette[bar].v;
      }
      // Replicate rows down each plane.
      for (int y = 0; y < kHeight; ++y) {
        std::memcpy(y_plane + y * kWidth, y_row.data(), kWidth);
      }
      for (int y = 0; y < kUVHeight; ++y) {
        std::memcpy(u_plane + y * kUVStride, u_row.data(), kUVStride);
        std::memcpy(v_plane + y * kUVStride, v_row.data(), kUVStride);
      }

      scoped_refptr<libwebrtc::RTCVideoFrame> frame =
          libwebrtc::RTCVideoFrame::Create(kWidth, kHeight, buffer.data(),
                                           kFrameBytes);
      if (frame) source->OnCapturedFrame(frame);

      if (frame_idx % (kFps * 2) == 0) {
        Log("[pusher] color bars frame #%d (scroll=%d)", frame_idx, scroll);
      }
      ++frame_idx;
      next_tick += kFrameInterval;
      std::this_thread::sleep_until(next_tick);
    }
  }

  scoped_refptr<libwebrtc::RTCPeerConnectionFactory> factory_;
  PeerSlot offerer_;
  PeerSlot answerer_;

  scoped_refptr<libwebrtc::RTCVideoSource> video_source_;
  scoped_refptr<libwebrtc::RTCVideoTrack> local_video_track_;

  std::thread pusher_thread_;
  std::atomic<bool> pusher_running_{false};

  bool running_ = false;
};

class LoopbackApp : public wxApp {
 public:
  bool OnInit() override {
    auto* frame = new MainFrame();
    frame->Show();
    return true;
  }
};

wxIMPLEMENT_APP(LoopbackApp);
