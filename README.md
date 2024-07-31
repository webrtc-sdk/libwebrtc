# libwebrtc-{windows,linux}-{amd64,x86,armv7,arm64}.{dll,so}

WebRTC C++ wrapper
A C++ binary wrapper for webrtc, mainly used for flutter-webrtc desktop (windows, linux, embedded) version release.

possible supported platforms
- [x] Windows (x86,x64)
- [x] Linux (x86,x64)
- [x] Embedded Linux (armv7, arm64)

## Note

Use the https://github.com/webrtc-sdk/webrtc.git branch m125_release to compile.

## Usage

### Create checkout dir

```bash
mkdir libwebrtc_build
cd libwebrtc_build
```

### Create .gclient

```bash
solutions = [
  {
    "name"        : 'src',
    "url"         : 'https://github.com/webrtc-sdk/webrtc.git@m124_release',
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
+    deps = [ ":webrtc","//libwebrtc", ]
     if (rtc_build_examples) {
       deps += [ "examples" ]
     }
```

## for Windows

Download and install the Visual Studio Community 2022

### GN

```bash
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
set GYP_MSVS_VERSION=2022
set GYP_GENERATORS=ninja,msvs-ninja
set GYP_MSVS_OVERRIDE_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2022\Community
cd src
gn gen out-debug/Windows-x64 --args="target_os=\"win\" target_cpu=\"x64\" is_component_build=false is_clang=true is_debug=true rtc_use_h264=true ffmpeg_branding=\"Chrome\" rtc_include_tests=false rtc_build_examples=false libwebrtc_desktop_capture=true" --ide=vs2022
```

### Compile for windows

```bash
ninja -C out-debug/Windows-x64 libwebrtc
```

## for Linux
```bash
export ARCH=x64 # x86, x64, arm, arm64
gn gen out/Linux-$ARCH --args="target_os=\"linux\" target_cpu=\"$ARCH\" is_debug=false rtc_include_tests=false rtc_use_h264=true ffmpeg_branding=\"Chrome\" is_component_build=false use_rtti=true use_custom_libcxx=false rtc_enable_protobuf=false"
```

### Compile for Linux

```bash
ninja -C out-debug/Windows-x64 libwebrtc
```


