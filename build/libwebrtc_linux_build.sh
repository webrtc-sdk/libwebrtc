#!/bin/bash
# Exit immediately if any command fails. This ensures CI properly reports build
# failures instead of continuing to create empty/broken artifacts.
set -e

# Copyright 2023 LiveKit, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


arch=""
profile="release"

while [ "$#" -gt 0 ]; do
  case "$1" in
    --arch)
      arch="$2"
      if [ "$arch" != "x64" ] && [ "$arch" != "arm64" ]; then
        echo "Error: Invalid value for --arch. Must be 'x64' or 'arm64'."
        exit 1
      fi
      shift 2
      ;;
    --profile)
      profile="$2"
      if [ "$profile" != "debug" ] && [ "$profile" != "release" ]; then
        echo "Error: Invalid value for --profile. Must be 'debug' or 'release'."
        exit 1
      fi
      shift 2
      ;;
    *)
      echo "Error: Unknown argument '$1'"
      exit 1
      ;;
  esac
done

if [ -z "$arch" ]; then
  echo "Error: --arch must be set."
  exit 1
fi

echo "Building libwebrtc.so - Linux"
echo "Arch: $arch"
echo "Profile: $profile"

if [ ! -e "$(pwd)/depot_tools" ]
then
  git clone --depth 1 https://chromium.googlesource.com/chromium/tools/depot_tools.git
fi

export COMMAND_DIR=$(cd $(dirname $0); pwd)
export PATH="$(pwd)/depot_tools:$PATH"
export OUTPUT_DIR="$(pwd)/src/out-$arch-$profile"
export ARTIFACTS_DIR="$(pwd)/linux-$arch-$profile"

if [ ! -e "$(pwd)/src" ]
then
  gclient sync -D --no-history
fi

if [ ! -e "$(pwd)/src/libwebrtc" ]
then
  mkdir -p $(pwd)/src/libwebrtc
  cp -rf $(pwd)/../{include,src,patches,BUILD.gn,LICENSE} $(pwd)/src/libwebrtc
fi

cd src
git apply "$COMMAND_DIR/src/libwebrtc/patches/custom_audio_source_m144.patch" -v --ignore-space-change --ignore-whitespace --whitespace=nowarn
git apply "$COMMAND_DIR/src/libwebrtc/patches/add_libwebrtc_build_target.patch" -v --ignore-space-change --ignore-whitespace --whitespace=nowarn


mkdir -p "$ARTIFACTS_DIR/{include,lib}"

python3 "./src/build/linux/sysroot_scripts/install-sysroot.py" --arch="$arch"

debug="false"
if [ "$profile" = "debug" ]; then
  debug="true"
fi

# Note: use_clang_modules=false is required to avoid C++ module compilation issues.
# Without this flag, the build may fail partway through, resulting in missing
# or incomplete artifacts.
args="is_debug=$debug  \
  target_os=\"linux\" \
  target_cpu=\"$arch\" \
  rtc_enable_protobuf=false \
  treat_warnings_as_errors=false \
  use_llvm_libatomic=false \
  use_custom_libcxx=false \
  use_custom_libcxx_for_host=false \
  use_clang_modules=false \
  rtc_include_tests=false \
  rtc_build_tools=false \
  rtc_build_examples=false \
  rtc_libvpx_build_vp9=true \
  enable_libaom=true \
  is_component_build=false \
  enable_stripping=true \
  ffmpeg_branding=\"Chrome\" \
  rtc_use_h264=true \
  rtc_use_h265=true \
  rtc_use_pipewire=true \
  symbol_level=0 \
  enable_iterator_debugging=false \
  use_rtti=true \
  rtc_use_x11=true"

# generate ninja files
gn gen "$OUTPUT_DIR" --root="src" --args="${args}"

# build static library
ninja -C "$OUTPUT_DIR" :libwebrtc

# License generation is optional - may fail with some Python versions
# Use vpython3 from depot_tools for consistent Python version
vpython3 "./src/tools_webrtc/libs/generate_licenses.py" \
  --target :libwebrtc "$OUTPUT_DIR" "$OUTPUT_DIR" || echo "Warning: License generation failed (non-critical)"

cp "$OUTPUT_DIR/libwebrtc.so" "$ARTIFACTS_DIR/lib"
cp "$OUTPUT_DIR/LICENSE.md" "$ARTIFACTS_DIR"

cd src
cp -rf "libwebrtc/include/*" "$ARTIFACTS_DIR/include"