# libwebrtc
WebRTC dynamic library wrapper for flutter desktop plugin.

## Usage
```
git clone https://github.com/cloudwebrtc/libwebrtc.git

cd libwebrtc/

cmake . -Bbuild \
-DLIBWEBRTC_INCLUDE_PATH:PATH=${PATH_TO_LIBWEBRTC_SOURCES} \
-DLIBWEBRTC_OUT_PATH:PATH=${PATH_TO_LIBWEBRTC_OUTPUT_PATH}

# Compile.
make -C build/

```
