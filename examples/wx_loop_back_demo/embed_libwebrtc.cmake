# Embed libwebrtc.dylib into a macOS .app bundle.
#
# Required arguments (passed via -D on the cmake -P invocation):
#   LIBWEBRTC_LIBRARY - source dylib to copy in
#   BUNDLE_DIR        - <something>.app
#   EXE_PATH          - main executable inside the bundle

if(NOT LIBWEBRTC_LIBRARY OR NOT BUNDLE_DIR OR NOT EXE_PATH)
    message(FATAL_ERROR "embed_libwebrtc.cmake: missing required arguments")
endif()

set(FRAMEWORKS_DIR "${BUNDLE_DIR}/Contents/Frameworks")
set(EMBEDDED       "${FRAMEWORKS_DIR}/libwebrtc.dylib")

file(MAKE_DIRECTORY "${FRAMEWORKS_DIR}")
configure_file("${LIBWEBRTC_LIBRARY}" "${EMBEDDED}" COPYONLY)

# Make the embedded copy resolve via @rpath.
execute_process(
    COMMAND install_name_tool -id "@rpath/libwebrtc.dylib" "${EMBEDDED}"
    RESULT_VARIABLE rc
)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "install_name_tool -id failed (${rc})")
endif()

# Discover whatever path the executable currently records for libwebrtc and
# rewrite it to @rpath/libwebrtc.dylib. The source dylib may have any
# install_name (./libwebrtc.dylib, an absolute path, etc.), so we can't
# hard-code the -change source.
execute_process(
    COMMAND otool -L "${EXE_PATH}"
    OUTPUT_VARIABLE OTOOL_OUT
    RESULT_VARIABLE rc
)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "otool -L ${EXE_PATH} failed (${rc})")
endif()

string(REPLACE "\n" ";" OTOOL_LINES "${OTOOL_OUT}")
foreach(line IN LISTS OTOOL_LINES)
    if(line MATCHES "libwebrtc[^/ ]*\\.dylib")
        string(STRIP "${line}" stripped)
        # Each dependency line is "<path> (compatibility version ...)" — take
        # the path up to the first space.
        string(REGEX REPLACE " \\(compatibility.*$" "" dep "${stripped}")
        if(NOT dep STREQUAL "@rpath/libwebrtc.dylib")
            message(STATUS "Rewriting executable dependency: ${dep} -> @rpath/libwebrtc.dylib")
            execute_process(
                COMMAND install_name_tool -change "${dep}"
                        "@rpath/libwebrtc.dylib" "${EXE_PATH}"
                RESULT_VARIABLE rc
            )
            if(NOT rc EQUAL 0)
                message(FATAL_ERROR "install_name_tool -change failed (${rc})")
            endif()
        endif()
    endif()
endforeach()

# Re-sign — install_name_tool invalidates the existing signature.
execute_process(COMMAND codesign --force --sign - "${EMBEDDED}")
execute_process(COMMAND codesign --force --sign - "${BUNDLE_DIR}")
