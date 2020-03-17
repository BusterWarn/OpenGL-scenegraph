set(jpeg_directory jpeg-7)
project(jpeg)
cmake_minimum_required(VERSION 3.0)

if(CMAKEPORTS_ASSEMBLE)
    # This section is used only in CMakePorts SVN
    set(jpeg_directory ${jpeg_directory} PARENT_SCOPE)
    set(jpeg_tarball   jpegsrc.v7.tar.gz PARENT_SCOPE)
    set(jpeg_url http://www.ijg.org/files/jpegsrc.v7.tar.gz PARENT_SCOPE)
    set(jpeg_patches
        jerror_missing_stdlib_h.diff
        jpeg-7_cmp.diff
        win32_dll_support.diff
        PARENT_SCOPE)
endif()

if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${jpeg_directory}")
    add_subdirectory(${jpeg_directory})
endif()

# Allow users to rename the library that gets built
if(TARGET jpeg AND CMAKEPORTS_NAME_PREFIX)
    set_target_properties(jpeg PROPERTIES OUTPUT_NAME "${CMAKEPORTS_NAME_PREFIX}jpeg")
endif()

# Compatibility settings for interoperability with FindJPEG.cmake
set(JPEG_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/${jpeg_directory}
    ${CMAKE_CURRENT_BINARY_DIR}/${jpeg_directory} PARENT_SCOPE)
set(JPEG_LIBRARY   jpeg                           PARENT_SCOPE)
set(JPEG_FOUND true                               PARENT_SCOPE)
set(JPEG_LIBRARIES jpeg                           PARENT_SCOPE)
set(NATIVE_JPEG_INCLUDE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/${jpeg_directory}
    ${CMAKE_CURRENT_BINARY_DIR}/${jpeg_directory} PARENT_SCOPE)
