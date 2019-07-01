# libwebrtc

WebRTC dynamic library wrapper for flutter desktop plugin.

## Note

Use the WebRTC branch M74 to compile.

## Usage

- Download webrtc source into webrtc_src_m74, Please refer to https://webrtc.org/native-code/development/.

```
cd webrtc_src_m74/src
git clone https://github.com/cloudwebrtc/libwebrtc.git
```

- Modify webrtc's src/BUILD.gn file and add libwebrtc to group("default").

```patch
diff --git a/BUILD.gn b/BUILD.gn
index bfe6d02ab9..2c0eaaa631 100644
--- a/BUILD.gn
+++ b/BUILD.gn
@@ -30,6 +30,7 @@ if (!build_with_chromium) {
     testonly = true
     deps = [
       ":webrtc",
+      "//libwebrtc:libwebrtc",
     ]
     if (rtc_build_examples) {
       deps += [ "examples" ]
```

- Compile

```
ninja -C out/Default libwebrtc
```
