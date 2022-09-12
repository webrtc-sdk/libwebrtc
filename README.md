# libwebrtc.{dll,so}

WebRTC C++ wrapper
A C++ binary wrapper for webrtc, mainly used for flutter-webrtc desktop (windows, linux, embedded) version release.

## Note

Use the https://github.com/webrtc-sdk/webrtc.git branch m104_release to compile.

## Usage

### Create checkout dir

```bash
mkdir webrtc_src_m104
cd webrtc_src_m104
```

### Create .gclient

```bash
solutions = [
  {
    "name"        : 'src',
    "url"         : 'https://github.com/webrtc-sdk/webrtc.git@m104_release',
    "deps_file"   : 'DEPS',
    "managed"     : False,
    "custom_deps" : {
    },
    "custom_vars": {},
  },
]
target_os  = ['win']
```

### Synchronize source code

```bash
gclient sync
```

### Compile steps for libwebrtc

Confirm that it is currently in the `src`.

```bash
git clone https://github.com/webrtc-sdk/libwebrtc
```

### Modify webrtc's src/BUILD.gn file and add libwebrtc to group("default").

```patch
diff --git a/BUILD.gn b/BUILD.gn
index e60d7dd0bd..b9b6acab8b 100644
--- a/BUILD.gn
+++ b/BUILD.gn
@@ -29,7 +29,7 @@ if (!build_with_chromium) {
   # 'ninja default' and then 'ninja all', the second build should do no work.
   group("default") {
     testonly = true
-    deps = [ ":webrtc" ]
+    deps = [ ":webrtc","//libwebrtc:libwebrtc", ]
     if (rtc_build_examples) {
       deps += [ "examples" ]
     }
```

### GN

```bash
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
set GYP_MSVS_VERSION=2019
set GYP_GENERATORS=ninja,msvs-ninja
set GYP_MSVS_OVERRIDE_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community
cd src
gn gen out-debug/Windows-x64 --args="target_os=\"win\" target_cpu=\"x64\" is_component_build=false is_clang=true is_debug=true rtc_use_h264=true ffmpeg_branding=\"Chrome\" rtc_include_tests=false rtc_build_examples=false libwebrtc_desktop_capture=true" --ide=vs2019
```

### Compile

```bash
ninja -C out-debug/Windows-x64 libwebrtc
```
