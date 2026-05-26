cmake_minimum_required(VERSION 3.16)

project(wx_loop_back_demo CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Locate libwebrtc relative to this example directory:
#   <libwebrtc>/examples/wx_loop_back_demo/  <- CMAKE_CURRENT_SOURCE_DIR
#   <libwebrtc>/include/
#   <libwebrtc>/libs/libwebrtc.so
set(LIBWEBRTC_ROOT     "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(LIBWEBRTC_INCLUDE  "${LIBWEBRTC_ROOT}/include")

if(WIN32)
    set(LIBWEBRTC_LIBRARY "${LIBWEBRTC_ROOT}/libs/libwebrtc.lib")
elseif(APPLE)
    set(LIBWEBRTC_LIBRARY "${LIBWEBRTC_ROOT}/libs/libwebrtc.dylib")
else()
    set(LIBWEBRTC_LIBRARY "${LIBWEBRTC_ROOT}/libs/libwebrtc.so")
endif()

if(NOT EXISTS "${LIBWEBRTC_LIBRARY}")
    message(FATAL_ERROR
        "libwebrtc shared library not found at: ${LIBWEBRTC_LIBRARY}\n"
        "Build libwebrtc first or place the prebuilt binary under "
        "${LIBWEBRTC_ROOT}/libs/.")
endif()

# --- wxWidgets ---------------------------------------------------------------
# Prefer the modern CONFIG package shipped by wxWidgets >= 3.2 (provides
# imported targets wx::core, wx::base). Fall back to the legacy module-mode
# find_package, which sets wxWidgets_INCLUDE_DIRS / wxWidgets_LIBRARIES.

find_package(wxWidgets CONFIG QUIET COMPONENTS core base)

if(NOT wxWidgets_FOUND)
    find_package(wxWidgets REQUIRED COMPONENTS core base)
    include(${wxWidgets_USE_FILE})
endif()

# --- Target ------------------------------------------------------------------
add_executable(wx_loop_back_demo WIN32 MACOSX_BUNDLE main.cc)

target_include_directories(wx_loop_back_demo PRIVATE
    ${LIBWEBRTC_INCLUDE}
)

# Mirror the macros libwebrtc was built with, otherwise headers like
# rtc_peerconnection_factory.h compile away the desktop-capture surface and
# we'd be missing GetDesktopDevice / CreateDesktopSource etc.
target_compile_definitions(wx_loop_back_demo PRIVATE RTC_DESKTOP_DEVICE)

if(TARGET wx::core)
    target_link_libraries(wx_loop_back_demo PRIVATE wx::core wx::base)
else()
    target_include_directories(wx_loop_back_demo PRIVATE ${wxWidgets_INCLUDE_DIRS})
    target_compile_definitions(wx_loop_back_demo PRIVATE ${wxWidgets_DEFINITIONS})
    target_link_libraries(wx_loop_back_demo PRIVATE ${wxWidgets_LIBRARIES})
endif()

target_link_libraries(wx_loop_back_demo PRIVATE ${LIBWEBRTC_LIBRARY})

if(APPLE)
    # On macOS, embed libwebrtc.dylib inside the .app bundle's Frameworks/
    # directory and resolve it at runtime via @rpath, so the bundle is
    # self-contained and can be launched from anywhere.
    set_target_properties(wx_loop_back_demo PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
        MACOSX_BUNDLE_BUNDLE_NAME "wx_loop_back_demo"
        MACOSX_BUNDLE_GUI_IDENTIFIER "io.github.webrtc-sdk.libwebrtc.wx_loop_back_demo"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        BUILD_RPATH   "@executable_path/../Frameworks"
        INSTALL_RPATH "@executable_path/../Frameworks"
    )

    find_library(APPLICATION_SERVICES ApplicationServices)
    find_library(AUDIO_TOOLBOX AudioToolbox)
    find_library(AV_FOUNDATION AVFoundation)
    find_library(CORE_AUDIO CoreAudio)
    find_library(CORE_FOUNDATION Foundation)
    find_library(CORE_MEDIA CoreMedia)
    find_library(CORE_VIDEO CoreVideo)
    find_library(CORE_SERVICES CoreServices)

    target_link_libraries(wx_loop_back_demo PRIVATE
        ${APPLICATION_SERVICES}
        ${AUDIO_TOOLBOX}
        ${AV_FOUNDATION}
        ${CORE_AUDIO}
        ${CORE_FOUNDATION}
        ${CORE_MEDIA}
        ${CORE_VIDEO}
        ${CORE_SERVICES}
    )

    # Post-build: copy the dylib into the bundle, rewrite the executable's
    # recorded dependency to @rpath/libwebrtc.dylib (the source library may
    # be linked under any install_name — `./libwebrtc.dylib`, an absolute
    # path, etc., so the rewrite is computed from `otool -L` at runtime),
    # then ad-hoc re-sign so recent macOS will load the modified binaries.
    add_custom_command(TARGET wx_loop_back_demo POST_BUILD
        COMMAND ${CMAKE_COMMAND}
            -DLIBWEBRTC_LIBRARY=${LIBWEBRTC_LIBRARY}
            -DBUNDLE_DIR=$<TARGET_BUNDLE_DIR:wx_loop_back_demo>
            -DEXE_PATH=$<TARGET_FILE:wx_loop_back_demo>
            -P ${CMAKE_CURRENT_SOURCE_DIR}/embed_libwebrtc.cmake
        COMMENT "Embedding libwebrtc.dylib into wx_loop_back_demo.app"
    )
else()
    # Non-Apple: keep the build tree pointing at libs/ for direct execution.
    set_target_properties(wx_loop_back_demo PROPERTIES
        BUILD_RPATH   "${LIBWEBRTC_ROOT}/libs"
        INSTALL_RPATH "${LIBWEBRTC_ROOT}/libs"
    )
endif()
