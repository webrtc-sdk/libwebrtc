#!/bin/sh
if [ ! -n "$1" ]; then
  echo "Usage: $0 'debug' | 'release'"
  exit 0
fi

MODE=$1
OUT_DIR=./out-$MODE
DEBUG="false"
if [ "$MODE" == "debug" ]; then
  DEBUG="true"
fi


if [ ! -e "$(pwd)/depot_tools" ]
then
  git clone --depth 1 https://chromium.googlesource.com/chromium/tools/depot_tools.git
fi

export PATH="$(pwd)/depot_tools:$PATH"

if [ ! -e "$(pwd)/src" ]
then
  gclient sync -D --no-history
fi

echo "xcframework_dynamic_build.sh: MODE=$MODE, DEBUG=$DEBUG"

gn gen $OUT_DIR/tvOS-arm64-device --root="src" --args="    
      target_os = \"ios\"
      ios_enable_code_signing = false
      is_component_build = false
      target_environment = \"appletv\"
      target_cpu = \"arm64\"
      ios_deployment_target = \"17.0\"
      use_goma = false
      rtc_enable_symbol_export = true
      rtc_libvpx_build_vp9 = true
      rtc_include_tests = false
      rtc_build_examples = false
      rtc_use_h264 = false
      rtc_enable_protobuf = false
      enable_libaom = true
      rtc_include_dav1d_in_internal_decoder_factory = true
      use_rtti = true
      is_debug = $DEBUG
      enable_dsyms = $DEBUG
      enable_stripping = true" --ide=xcode

ninja -C $OUT_DIR/tvOS-arm64-device ios_framework_bundle -j 10

gn gen $OUT_DIR/tvOS-arm64-simulator --root="src" --args="    
      target_os = \"ios\"
      ios_enable_code_signing = false
      is_component_build = false
      target_environment = \"appletvsimulator\"
      target_cpu = \"arm64\"
      ios_deployment_target = \"17.0\"
      use_goma = false
      rtc_enable_symbol_export = true
      rtc_libvpx_build_vp9 = true
      rtc_include_tests = false
      rtc_build_examples = false
      rtc_use_h264 = false
      rtc_enable_protobuf = false
      enable_libaom = true
      rtc_include_dav1d_in_internal_decoder_factory = true
      use_rtti = true
      is_debug = $DEBUG
      enable_dsyms = $DEBUG
      enable_stripping = true" --ide=xcode

ninja -C $OUT_DIR/tvOS-arm64-simulator ios_framework_bundle -j 10

gn gen $OUT_DIR/xrOS-arm64-device --root="src" --args="
      treat_warnings_as_errors = false 
      target_os = \"ios\"
      ios_enable_code_signing = false
      is_component_build = false
      target_environment = \"xrdevice\"
      target_cpu = \"arm64\"
      ios_deployment_target = \"1.1.0\"
      use_goma = false
      rtc_enable_symbol_export = true
      rtc_libvpx_build_vp9 = true
      rtc_include_tests = false
      rtc_build_examples = false
      rtc_use_h264 = false
      rtc_enable_protobuf = false
      enable_libaom = true
      rtc_include_dav1d_in_internal_decoder_factory = true
      use_rtti = true
      is_debug = $DEBUG
      enable_dsyms = $DEBUG
      enable_stripping = true" --ide=xcode

ninja -C $OUT_DIR/xrOS-arm64-device ios_framework_bundle -j 10

gn gen $OUT_DIR/xrOS-arm64-simulator --root="src" --args="
      treat_warnings_as_errors = false
      target_os = \"ios\"
      ios_enable_code_signing = false
      is_component_build = false
      target_environment = \"xrsimulator\"
      target_cpu = \"arm64\"
      ios_deployment_target = \"1.1.0\"
      use_goma = false
      rtc_enable_symbol_export = true
      rtc_libvpx_build_vp9 = true
      rtc_include_tests = false
      rtc_build_examples = false
      rtc_use_h264 = false
      rtc_enable_protobuf = false
      enable_libaom = true
      rtc_include_dav1d_in_internal_decoder_factory = true
      use_rtti = true
      is_debug = $DEBUG
      enable_dsyms = $DEBUG
      enable_stripping = true" --ide=xcode

ninja -C $OUT_DIR/xrOS-arm64-simulator ios_framework_bundle -j 10

gn gen $OUT_DIR/catalyst-arm64 --root="src" --args="
      treat_warnings_as_errors = false
      target_os = \"ios\"
      ios_enable_code_signing = false
      is_component_build = false
      target_environment = \"catalyst\"
      target_cpu = \"arm64\"
      ios_deployment_target = \"14.0\"
      use_goma = false
      rtc_enable_symbol_export = true
      rtc_libvpx_build_vp9 = true
      rtc_include_tests = false
      rtc_build_examples = false
      rtc_use_h264 = false
      rtc_enable_protobuf = false
      enable_libaom = true
      rtc_include_dav1d_in_internal_decoder_factory = true
      use_rtti = true
      is_debug = $DEBUG
      enable_dsyms = $DEBUG
      enable_stripping = true" --ide=xcode

ninja -C $OUT_DIR/catalyst-arm64 ios_framework_bundle -j 10

gn gen $OUT_DIR/catalyst-x64 --root="src" --args="
      treat_warnings_as_errors = false
      target_os = \"ios\"
      ios_enable_code_signing = false
      is_component_build = false
      target_environment = \"catalyst\"
      target_cpu = \"x64\"
      ios_deployment_target = \"14.0\"
      use_goma = false
      rtc_enable_symbol_export = true
      rtc_libvpx_build_vp9 = true
      rtc_include_tests = false
      rtc_build_examples = false
      rtc_use_h264 = false
      rtc_enable_protobuf = false
      enable_libaom = true
      rtc_include_dav1d_in_internal_decoder_factory = true
      use_rtti = true
      is_debug = $DEBUG
      enable_dsyms = $DEBUG
      enable_stripping = true" --ide=xcode

ninja -C $OUT_DIR/catalyst-x64 ios_framework_bundle -j 10

gn gen $OUT_DIR/iOS-arm64-device --root="src" --args="
      treat_warnings_as_errors = false
      target_os = \"ios\"
      ios_enable_code_signing = false
      is_component_build = false
      target_environment = \"device\"
      target_cpu = \"arm64\"
      ios_deployment_target = \"13.0\"
      use_goma = false
      rtc_enable_symbol_export = true
      rtc_libvpx_build_vp9 = true
      rtc_include_tests = false
      rtc_build_examples = false
      rtc_use_h264 = false
      rtc_enable_protobuf = false
      enable_libaom = true
      rtc_include_dav1d_in_internal_decoder_factory = true
      use_rtti = true
      is_debug = $DEBUG
      enable_dsyms = $DEBUG
      enable_stripping = true" --ide=xcode

ninja -C $OUT_DIR/iOS-arm64-device ios_framework_bundle -j 10

gn gen $OUT_DIR/iOS-x64-simulator --root="src" --args="
      treat_warnings_as_errors = false
      target_os = \"ios\"
      ios_enable_code_signing = false
      is_component_build = false
      target_environment = \"simulator\"
      target_cpu = \"x64\"
      ios_deployment_target = \"13.0\"
      rtc_libvpx_build_vp9 = true
      use_goma = false
      rtc_enable_symbol_export = true
      rtc_include_tests = false
      rtc_build_examples = false
      rtc_use_h264 = false
      rtc_enable_protobuf = false
      enable_libaom = true
      rtc_include_dav1d_in_internal_decoder_factory = true
      use_rtti = true
      is_debug = $DEBUG
      enable_dsyms = $DEBUG
      enable_stripping = true" --ide=xcode

ninja -C $OUT_DIR/iOS-x64-simulator ios_framework_bundle -j 10

gn gen $OUT_DIR/iOS-arm64-simulator --root="src" --args="
      treat_warnings_as_errors = false
      target_os = \"ios\"
      ios_enable_code_signing = false
      is_component_build = false
      target_environment = \"simulator\"
      target_cpu = \"arm64\"
      ios_deployment_target = \"13.0\"
      use_goma = false
      rtc_enable_symbol_export = true
      rtc_libvpx_build_vp9 = true
      rtc_include_tests = false
      rtc_build_examples = false
      rtc_use_h264 = false
      rtc_enable_protobuf = false
      enable_libaom = true
      rtc_include_dav1d_in_internal_decoder_factory = true
      use_rtti = true
      is_debug = $DEBUG
      enable_dsyms = $DEBUG
      enable_stripping = true" --ide=xcode

ninja -C $OUT_DIR/iOS-arm64-simulator ios_framework_bundle -j 10

gn gen $OUT_DIR/macOS-x64 --root="src" --args="
      treat_warnings_as_errors = false
      target_os=\"mac\"
      target_cpu=\"x64\"
      mac_deployment_target=\"10.14\"
      is_component_build = false
      target_cpu = \"x64\"
      use_goma = false
      rtc_enable_symbol_export = true
      rtc_libvpx_build_vp9 = true
      rtc_include_tests = false
      rtc_build_examples = false
      rtc_use_h264 = false
      rtc_enable_protobuf = false
      enable_libaom = true
      rtc_include_dav1d_in_internal_decoder_factory = true
      use_rtti = true
      is_debug = $DEBUG
      enable_dsyms = $DEBUG" --ide=xcode

ninja -C $OUT_DIR/macOS-x64 mac_framework_bundle -j 10

gn gen $OUT_DIR/macOS-arm64 --root="src" --args="
      treat_warnings_as_errors = false
      target_os=\"mac\"
      target_cpu=\"x64\"
      mac_deployment_target=\"10.14\"
      is_component_build = false
      target_cpu = \"arm64\"
      use_goma = false
      rtc_enable_symbol_export = true
      rtc_libvpx_build_vp9 = true
      rtc_include_tests = false
      rtc_build_examples = false
      rtc_use_h264 = false
      rtc_enable_protobuf = false
      enable_libaom = true
      rtc_include_dav1d_in_internal_decoder_factory = true
      use_rtti = true
      is_debug = $DEBUG
      enable_dsyms = $DEBUG" --ide=xcode

ninja -C $OUT_DIR/macOS-arm64 mac_framework_bundle -j 10

rm -rf $OUT_DIR/*-lib $OUT_DIR/WebRTC.*

mkdir -p $OUT_DIR/macOS-lib
cp -R $OUT_DIR/macOS-x64/WebRTC.framework $OUT_DIR/macOS-lib/WebRTC.framework
lipo -create -output $OUT_DIR/macOS-lib/WebRTC.framework/WebRTC $OUT_DIR/macOS-arm64/WebRTC.framework/WebRTC $OUT_DIR/macOS-x64/WebRTC.framework/WebRTC

mkdir -p $OUT_DIR/catalyst-lib
cp -R $OUT_DIR/catalyst-arm64/WebRTC.framework $OUT_DIR/catalyst-lib/WebRTC.framework
lipo -create -output $OUT_DIR/catalyst-lib/WebRTC.framework/WebRTC $OUT_DIR/catalyst-arm64/WebRTC.framework/WebRTC $OUT_DIR/catalyst-x64/WebRTC.framework/WebRTC

mkdir -p $OUT_DIR/iOS-device-lib
cp -R $OUT_DIR/iOS-arm64-device/WebRTC.framework $OUT_DIR/iOS-device-lib/WebRTC.framework
lipo -create -output $OUT_DIR/iOS-device-lib/WebRTC.framework/WebRTC $OUT_DIR/iOS-arm64-device/WebRTC.framework/WebRTC

mkdir -p $OUT_DIR/iOS-simulator-lib
cp -R $OUT_DIR/iOS-arm64-simulator/WebRTC.framework $OUT_DIR/iOS-simulator-lib/WebRTC.framework
lipo -create -output $OUT_DIR/iOS-simulator-lib/WebRTC.framework/WebRTC $OUT_DIR/iOS-arm64-simulator/WebRTC.framework/WebRTC $OUT_DIR/iOS-x64-simulator/WebRTC.framework/WebRTC

xcodebuild -create-xcframework \
  -framework $OUT_DIR/iOS-device-lib/WebRTC.framework \
  -framework $OUT_DIR/iOS-simulator-lib/WebRTC.framework \
  -framework $OUT_DIR/catalyst-lib/WebRTC.framework \
  -framework $OUT_DIR/macOS-lib/WebRTC.framework \
  -framework $OUT_DIR/xrOS-arm64-device/WebRTC.framework \
  -framework $OUT_DIR/xrOS-arm64-simulator/WebRTC.framework \
  -framework $OUT_DIR/tvOS-arm64-device/WebRTC.framework \
  -framework $OUT_DIR/tvOS-arm64-simulator/WebRTC.framework \
  -output $OUT_DIR/WebRTC.xcframework

cp ./src/LICENSE $OUT_DIR/WebRTC.xcframework/

cd $OUT_DIR/WebRTC.xcframework/macos-arm64_x86_64/WebRTC.framework/
mv WebRTC Versions/A/WebRTC
ln -s Versions/Current/WebRTC WebRTC
cd ../../../../

cd $OUT_DIR/WebRTC.xcframework/ios-arm64_x86_64-maccatalyst/WebRTC.framework/
mv WebRTC Versions/A/WebRTC
ln -s Versions/Current/WebRTC WebRTC
cd ../../../
zip --symlinks -9 -r WebRTC.xcframework.zip WebRTC.xcframework

# hash
shasum -a 256 WebRTC.xcframework.zip > WebRTC.xcframework.zip.shasum
cat WebRTC.xcframework.zip.shasum
