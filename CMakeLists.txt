cmake_minimum_required(VERSION 3.5)

project(libwebrtc LANGUAGES CXX)

# Set version number.
set(libwebrtc_VERSION_MAJOR 0)
set(libwebrtc_VERSION_MINOR 0)
set(libwebrtc_VERSION_PATCH 1)

# Configure a header file to pass the version.
configure_file (
  "${PROJECT_SOURCE_DIR}/version.hpp.in"
  "${PROJECT_SOURCE_DIR}/include/version.hpp"
)

# C++ standard requirements.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project options.
option(LIBWEBRTC_BUILD_TESTS "Build unit tests" ON)

# Project configuration.
set(LIBWEBRTC_INCLUDE_PATH "" CACHE STRING "libwebrtc include path")
set(LIBWEBRTC_OUT_PATH "" CACHE STRING "libwebrtc output path")

if(NOT LIBWEBRTC_INCLUDE_PATH)
	message(FATAL_ERROR "LIBWEBRTC_INCLUDE_PATH not provided")
endif()

if(NOT LIBWEBRTC_OUT_PATH)
	message(FATAL_ERROR "LIBWEBRTC_OUT_PATH not provided")
endif()

message ("\n=========== liblibwebrtc Build Configuration =============\n")
message (STATUS "LIBWEBRTC_BUILD_TESTS  = " ${LIBWEBRTC_BUILD_TESTS})
message (STATUS "LIBWEBRTC_INCLUDE_PATH = " ${LIBWEBRTC_INCLUDE_PATH})
message (STATUS "LIBWEBRTC_OUT_PATH  = " ${LIBWEBRTC_OUT_PATH})
message ("")

if (${BUILD_TESTS})
	add_subdirectory(test)
endif()

set(HEADER_FILES
	include/base/atomicops.h
	include/base/inlined_vector.h
	include/base/refcount.h
	include/base/refcountedobject.h
	include/base/scoped_ref_ptr.h
	include/libwebrtc.h
	include/rtc_audio_device.h
	include/rtc_audio_source.h
	include/rtc_audio_track.h
	include/rtc_data_channel.h
	include/rtc_ice_candidate.h
	include/rtc_media_stream.h
	include/rtc_media_track.h
	include/rtc_mediaconstraints.h
	include/rtc_peerconnection.h
	include/rtc_peerconnection_factory.h
	include/rtc_session_description.h
	include/rtc_types.h
	include/rtc_video_device.h
	include/rtc_video_frame.h
	include/rtc_video_renderer.h
	include/rtc_video_sink.h
	include/rtc_video_source.h
	include/rtc_video_track.h
)

set(
	SOURCE_FILES
	src/libwebrtc.cc
	src/rtc_audio_device_impl.cc
	src/rtc_audio_source_impl.cc
	src/rtc_audio_track_impl.cc
	src/rtc_data_channel_impl.cc
	src/rtc_ice_candidate_impl.cc
	src/rtc_media_stream_impl.cc
	src/rtc_mediaconstraints_impl.cc
	src/rtc_peerconnection_factory_impl.cc
	src/rtc_peerconnection_impl.cc
	src/rtc_session_description_impl.cc
	src/rtc_video_device_impl.cc
	src/rtc_video_frame_impl.cc
	src/rtc_video_sink_adapter.cc
	src/rtc_video_source_impl.cc
	src/rtc_video_track_impl.cc
)

# Create target.
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

# Source Dependencies.
# add_subdirectory(deps/other_deps "${CMAKE_CURRENT_BINARY_DIR}/other_deps")

# Add some compile flags to our source files.
if(MSVC)
	set_source_files_properties(${SOURCE_FILES}
		PROPERTIES COMPILE_FLAGS "/W3 /WX")
else()
	set_source_files_properties(${SOURCE_FILES}
		PROPERTIES COMPILE_FLAGS -Wall -Wextra -Wpedantic)
endif()

# Private (implementation) header files.
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Public (interface) headers from dependencies.
target_include_directories(${PROJECT_NAME} PUBLIC
	"${LIBWEBRTC_INCLUDE_PATH}"
	"${LIBWEBRTC_INCLUDE_PATH}/third_party/abseil-cpp"
	"${LIBWEBRTC_INCLUDE_PATH}/third_party/libyuv/include"
)

# Public (interface) dependencies.
target_link_libraries(${PROJECT_NAME} PUBLIC
	${LIBWEBRTC_OUT_PATH}/libwebrtc${CMAKE_STATIC_LIBRARY_SUFFIX}
)

# Compile definitions for libwebrtc.
target_compile_definitions(${PROJECT_NAME} PUBLIC
	$<$<NOT:$<PLATFORM_ID:Windows>>:WEBRTC_POSIX>
	$<$<PLATFORM_ID:Windows>:WEBRTC_WIN>
	$<$<PLATFORM_ID:Darwin>:WEBRTC_MAC>
)

install(TARGETS libwebrtc DESTINATION lib)
install(FILES ${HEADER_FILES} DESTINATION include/libwebrtc)
