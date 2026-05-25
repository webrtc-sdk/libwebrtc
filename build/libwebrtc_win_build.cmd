@echo off

setlocal enabledelayedexpansion

set arch=
set profile=release

:arg_loop
if "%1" == "" goto end_arg_loop
if "%1" == "--arch" (
    set "arch=%2"
    shift & shift & goto arg_loop
)
if "%1" == "--profile" (
    set "profile=%2"
    shift & shift & goto arg_loop
)
echo Error: Unknown argument '%1'
exit /b 1
:end_arg_loop

if not "!arch!" == "x64" if not "!arch!" == "arm64" (
    echo Error: Invalid value for --arch. Must be 'x64' or 'arm64'.
    exit /b 1
)
if not "!profile!" == "debug" if not "!profile!" == "release" (
    echo Error: Invalid value for --profile. Must be 'debug' or 'release'.
    exit /b 1
)

echo "Building  libwebrtc dll - Windows"
echo "Arch: !arch!"
echo "Profile: !profile!"

if not exist depot_tools (
  git clone --depth 1 https://chromium.googlesource.com/chromium/tools/depot_tools.git
)

set COMMAND_DIR=%~dp0
set PATH=%cd%\depot_tools;%PATH%
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
set GYP_GENERATORS=ninja,msvs-ninja
set GYP_MSVS_VERSION=2022
set OUTPUT_DIR=src\out-!arch!-!profile!
set ARTIFACTS_DIR=%cd%\libwebrtc-!arch!-!profile!
set vs2019_install=C:\Program Files\Microsoft Visual Studio\2022\Enterprise

if not exist src (
  call gclient.bat sync -D --with_branch_heads --with_tags
)

if not exist src/libwebrtc (
  md "%COMMAND_DIR%\src\libwebrtc"
  xcopy "%COMMAND_DIR%..\include" "%COMMAND_DIR%\src\libwebrtc\include" /E /I /Y
  xcopy "%COMMAND_DIR%..\src" "%COMMAND_DIR%\src\libwebrtc\src" /E /I /Y
  xcopy "%COMMAND_DIR%..\patches" "%COMMAND_DIR%\src\libwebrtc\patches" /E /I /Y
  copy "%COMMAND_DIR%..\BUILD.gn" "%COMMAND_DIR%\src\libwebrtc\"
  copy "%COMMAND_DIR%..\LICENSE" "%COMMAND_DIR%\src\libwebrtc\"
)

cd src
call git apply "libwebrtc\patches\custom_audio_source_m144.patch" -v --ignore-space-change --ignore-whitespace --whitespace=nowarn
call git apply "libwebrtc\patches\add_libwebrtc_build_target.patch" -v --ignore-space-change --ignore-whitespace --whitespace=nowarn

copy ".vpython3" "..\"

cd ..

mkdir "%ARTIFACTS_DIR%\lib"
mkdir "%ARTIFACTS_DIR%\include"

set "debug=false"
if "!profile!" == "debug" (
  set "debug=true"
)

rem generate ninja for release
call gn.bat gen %OUTPUT_DIR% --root="src" ^
  --args="is_debug=!debug! is_clang=true target_cpu=\"!arch!\" use_custom_libcxx=false rtc_libvpx_build_vp9=true enable_libaom=true rtc_include_tests=false rtc_build_examples=false rtc_build_tools=false is_component_build=false rtc_enable_protobuf=false rtc_use_h264=true ffmpeg_branding=\"Chrome\" symbol_level=0 enable_iterator_debugging=false"

rem build
ninja.exe -C %OUTPUT_DIR% libwebrtc

rem copy static library for release build
copy "%OUTPUT_DIR%\libwebrtc.dll.lib" "%ARTIFACTS_DIR%\lib"
copy "%OUTPUT_DIR%\libwebrtc.dll" "%ARTIFACTS_DIR%\lib"

rem generate license
call python3 "%cd%\src\tools_webrtc\libs\generate_licenses.py" ^
  --target :default %OUTPUT_DIR% %OUTPUT_DIR%

copy "%OUTPUT_DIR%\LICENSE.md" "%ARTIFACTS_DIR%"

rem copy header
xcopy src\libwebrtc\include\*.h "%ARTIFACTS_DIR%\include" /C /S /I /F /H\