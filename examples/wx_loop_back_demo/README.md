# wx_loop_back_demo

A wxWidgets desktop demo that exercises the libwebrtc C++ API end-to-end:
two in-process `RTCPeerConnection`s negotiate over a fake signaling channel,
an audio track plus a synthetic 640×480 colour-bars video track loop back,
and the remote video is rendered into the window.

```
+-----------+   offer/answer + ICE candidates    +-----------+
|  offerer  |  <-------------------------------> |  answerer |
+-----------+                                    +-----------+
      |                                                |
      | AddTrack (audio + colour-bars video)           |
      |                                                v
      |                                  RTCVideoTrack -> AddRenderer
      |                                                |
      v                                                v
 OnCapturedFrame()                          VideoSinkRenderer ->
 (synthetic frames)                         wxImage -> wxBitmap -> Panel
```

## Prerequisites

| Component | Notes |
|---|---|
| `libwebrtc.dylib` (macOS) / `.so` (Linux) / `.lib` + `.dll` (Windows) | Built from this repo, placed in `<repo>/src/libwebrtc/libs/`. The demo links the prebuilt artifact rather than the source tree. |
| wxWidgets ≥ 3.2 | `brew install wxwidgets` on macOS, `apt install libwxgtk3.2-dev` on Debian/Ubuntu, [official installer](https://www.wxwidgets.org/downloads/) on Windows. |
| CMake ≥ 3.16 | |
| C++17 toolchain | clang/AppleClang/GCC/MSVC all fine. |

## 1. Build / refresh `libwebrtc.dylib`

This demo depends on three libwebrtc additions:

- `RTCVideoSource::OnCapturedFrame(scoped_refptr<RTCVideoFrame>)` — push user-supplied frames.
- `RTCPeerConnectionFactory::CreateCustomVideoSource(...)` — custom video source not bound to any device.
- `internal::VideoCapturer::OnFrame` made public so the source can forward into the broadcaster.

They ship in the current source tree but are **not** in older prebuilt
binaries. Rebuild the dylib and copy it in before building the demo:

```bash
# from <repo root>
./lk_dynamic_build.sh                               # or whichever GN/Ninja wrapper applies
cp out-debug/macOS-arm64/libwebrtc.dylib src/libwebrtc/libs/
```

> Skipping this step → demo links the old dylib → vtable mismatch → either
> silent no-op video or a runtime crash on `OnCapturedFrame`.

## 2. Build the demo

From this directory:

```bash
cmake -B build
cmake --build build
```

A post-build step ([`embed_libwebrtc.cmake`](embed_libwebrtc.cmake)) copies
`libwebrtc.dylib` into `wx_loop_back_demo.app/Contents/Frameworks/`,
rewrites the executable's recorded dependency to `@rpath/libwebrtc.dylib`,
and ad-hoc re-signs both binaries (required on Apple Silicon).

## 3. Run

```bash
open build/wx_loop_back_demo.app          # macOS
./build/wx_loop_back_demo                 # Linux / Windows
```

## 4. Test plan

After the window opens, click **Start loopback** and verify:

| Check | Expected |
|---|---|
| First-run permission prompt (macOS) | macOS asks for **microphone** access. Grant it. The demo doesn't open a real camera, so no camera prompt should appear. |
| Status label | `Idle.` → `Starting...` → `Loopback running. See log below.` |
| Log — state transitions | `LibWebRTC::Initialize`, then state lines using enum names, e.g.<br>`[offerer] signaling state -> HaveLocalOffer`<br>`[answerer] signaling state -> HaveRemoteOffer`<br>`[answerer] signaling state -> Stable`<br>`[offerer] signaling state -> Stable`<br>`[offerer/answerer] ice gathering -> Gathering -> Complete`<br>`[offerer/answerer] ice connection -> Checking -> Connected -> Completed`<br>`[offerer/answerer] pc state -> Connecting -> Connected`<br>`[answerer] OnAddTrack (kind=AUDIO)`<br>`[answerer] OnAddTrack (kind=VIDEO)`<br>`[answerer] attached video sink renderer` |
| Video panel | 8 SMPTE colour bars (white / yellow / cyan / green / magenta / red / blue / black) **scrolling horizontally** at ~120 px/sec, scaled to fit the left half of the window. |
| Periodic log | `[pusher] color bars frame #N (scroll=…)` every 2 s; `[remote video] frame #N: 640x480` every 30 frames — both ends of the pipeline are flowing. |
| Click **Stop** | Status flips to `Stopped.`, log stops growing, video panel freezes on its last frame, `Start loopback` becomes available again. |
| Close the window | No crash, no zombie threads (`TeardownLoopback` joins the pusher thread before releasing the PCs / factory). |

### CLI sanity checks (macOS)

After step 2, verify the bundle was assembled correctly:

```bash
# embedded dylib is present
ls build/wx_loop_back_demo.app/Contents/Frameworks/libwebrtc.dylib

# executable resolves libwebrtc via @rpath, not the source path
otool -L build/wx_loop_back_demo.app/Contents/MacOS/wx_loop_back_demo \
  | grep webrtc
# → @rpath/libwebrtc.dylib

# rpath points inside the bundle
otool -l build/wx_loop_back_demo.app/Contents/MacOS/wx_loop_back_demo \
  | grep -A2 LC_RPATH
# → @executable_path/../Frameworks
```

### Resetting macOS TCC permissions while iterating

```bash
tccutil reset Microphone io.github.webrtc-sdk.libwebrtc.wx_loop_back_demo
tccutil reset Camera     io.github.webrtc-sdk.libwebrtc.wx_loop_back_demo
```

## Troubleshooting

- **`libwebrtc shared library not found at: …/libs/libwebrtc.dylib`** — step 1 wasn't run, or the artifact is in a different `out-*` directory. Copy it into `<repo>/src/libwebrtc/libs/`.
- **Black video panel, log shows `OnAddTrack (kind=VIDEO)` but no `[remote video] frame #…`** — almost always means the dylib is still the old one without `OnCapturedFrame`. Rebuild the dylib (step 1) and the demo (step 2).
- **`pc state -> Failed`, no audio/video loopback** — on hosts with very strict firewalls add a STUN server to `RTCConfiguration::ice_servers` in `MainFrame::OnStart`.
- **`install_name_tool` errors during the post-build step** — ensure Xcode CLT is installed (`xcode-select --install`).
