export ARCH=x64
gn gen out/Linux-$ARCH --args="target_os=\"linux\" target_cpu=\"$ARCH\" is_debug=false rtc_include_tests=false rtc_use_h264=true ffmpeg_branding=\"Chrome\" is_component_build=false use_rtti=true use_custom_libcxx=false rtc_enable_protobuf=false"
ninja -C out/Linux-x64 libwebrtc
