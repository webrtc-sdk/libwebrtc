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


profile="release"
commit=""

while [ "$#" -gt 0 ]; do
  case "$1" in
    --profile)
      profile="$2"
      if [ "$profile" != "debug" ] && [ "$profile" != "release" ]; then
        echo "Error: Invalid value for --profile. Must be 'debug' or 'release'."
        exit 1
      fi
      shift 2
      ;;
    --commit)
      commit="$2"
      shift 2
      ;;
    *)
      echo "Error: Unknown argument '$1'"
      exit 1
      ;;
  esac
done

echo "Building libwebrtc.aar - Android (multi-arch)"
echo "Profile: $profile"
echo "Commit: $commit"

if [ ! -e "$(pwd)/depot_tools" ]
then
  git clone --depth 1 https://chromium.googlesource.com/chromium/tools/depot_tools.git
fi

export COMMAND_DIR=$(cd $(dirname $0); pwd)
export PATH="$(pwd)/depot_tools:$PATH"
export OUTPUT_DIR="$(pwd)/src/out-android-$profile"
export ARTIFACTS_DIR="$(pwd)/android-$profile"

run_gclient_sync() {
  local default_branch
  default_branch=$(sed -nE "s/.*\"url\":[[:space:]]*'[^@]+@([^']+)'.*/\1/p" "$COMMAND_DIR/.gclient")
  if [ -z "$default_branch" ]; then
    echo "Error: cannot extract default branch from $COMMAND_DIR/.gclient"
    exit 1
  fi
  local checkout_ref="${commit:-$default_branch}"
  echo "Checkout ref: $checkout_ref"

  if [ ! -e "$(pwd)/src" ]; then
    git clone https://github.com/webrtc-sdk/webrtc.git src
  fi

  (
    cd src
    git fetch
    git checkout -f "$checkout_ref"
    git clean -df
  )

  gclient sync --with_branch_heads --jobs=8
}

run_gclient_sync

if [ ! -e "src/libwebrtc" ]
then
  mkdir -p src/libwebrtc
  cp -rf ../{include,src,patches,BUILD.gn,LICENSE} src/libwebrtc
fi

cd src
cp .vpython3 ..
git apply "libwebrtc/patches/custom_audio_source_m144.patch" -v --ignore-space-change --ignore-whitespace --whitespace=nowarn
git apply "libwebrtc/patches/add_libwebrtc_build_target.patch" -v --ignore-space-change --ignore-whitespace --whitespace=nowarn
cd ..

mkdir -p "$ARTIFACTS_DIR"

debug="false"
if [ "$profile" = "debug" ]; then
  debug="true"
fi

extra_gn_args="rtc_libvpx_build_vp9=true \
  enable_libaom=true \
  rtc_include_dav1d_in_internal_decoder_factory=true \
  is_java_debug=$debug \
  rtc_include_tests=false \
  enable_stripping=true \
  rtc_use_h264=false \
  rtc_use_h265=true \
  is_component_build=false \
  use_rtti=true \
  rtc_build_examples=false \
  treat_warnings_as_errors=false \
  rtc_enable_protobuf=false \
  is_debug=$debug"

# build multi-arch AAR via WebRTC's official helper
cd src
python3 ./tools_webrtc/android/build_aar.py \
  --build-dir "$OUTPUT_DIR" \
  --output "$ARTIFACTS_DIR/libwebrtc.aar" \
  --arch armeabi-v7a arm64-v8a x86_64 x86 \
  --extra-gn-args "$extra_gn_args"
cd ..

cp -rf "src/libwebrtc/LICENSE" "$ARTIFACTS_DIR/"
