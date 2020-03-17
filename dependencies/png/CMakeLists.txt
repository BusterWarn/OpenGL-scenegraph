set(png_directory libpng-1.2.39)

project(PNG)
cmake_minimum_required(VERSION 3.0)


IF (APPLE)
  set(ZLIB_ROOT /usr/lib)
ENDIF()

if(CMAKEPORTS_ASSEMBLE)
    # This section is used only in CMakePorts SVN
    set(png_directory ${png_directory} PARENT_SCOPE)
    set(png_url http://prdownloads.sourceforge.net/libpng/libpng-1.2.39-no-config.tar.gz PARENT_SCOPE)
    set(png_tarball libpng-1.2.39-no-config.tar.gz  PARENT_SCOPE)
    set(png_patches libpng-1.2.39_cmp.diff PARENT_SCOPE)
endif()

# We can't build both concurrently because a shared png on MSVC causes link
# errors against a static zlib
if(BUILD_SHARED_LIBS)
    set(PNG_SHARED ON)
    set(PNG_STATIC OFF)
else()
    set(PNG_SHARED OFF)
    set(PNG_STATIC ON)
endif()

if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${png_directory}")
    add_subdirectory(${png_directory})
endif()


IF (PNG_STATIC AND WIN32)
  # Added by Algoryx; add debug information into obj-files to avoid warning LNK4098 when using lib.
  string(REGEX REPLACE "/Z[iI7]" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7")
ENDIF (PNG_STATIC AND WIN32)



# Allow users to rename the library that gets built
if(CMAKEPORTS_NAME_PREFIX AND (TARGET png12 OR TARGET png12_static))
    if(PNG_SHARED)
        set_target_properties(png12 PROPERTIES OUTPUT_NAME "${CMAKEPORTS_NAME_PREFIX}png12")
    endif()
    if(PNG_STATIC)
        set_target_properties(png12_static PROPERTIES OUTPUT_NAME "${CMAKEPORTS_NAME_PREFIX}png12")
    endif()
endif()

# Hide these cache variables so as not to confuse CMakePorts users
if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}
        VERSION_GREATER 2.6.2)
    unset(PNG_STATIC CACHE)
    unset(PNG_SHARED CACHE)
endif()

# Compatibility settings for interoperability with FindPNG.cmake
if(ZLIB_FOUND)
    set(PNG_PNG_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/${png_directory} PARENT_SCOPE)
    set(PNG_LIBRARY   png12                          PARENT_SCOPE)
    set(PNG_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/${png_directory}
        ${ZLIB_INCLUDE_DIR}                          PARENT_SCOPE)
    set(PNG_LIBRARIES png12 ${ZLIB_LIBRARY}          PARENT_SCOPE)
    if(CYGWIN AND NOT BUILD_SHARED_LIBS)
        set(PNG_DEFINITIONS -DPNG_STATIC             PARENT_SCOPE)
    endif()
    set(PNG_FOUND true                               PARENT_SCOPE)
endif()
