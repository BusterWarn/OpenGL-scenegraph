 cmake_minimum_required(VERSION 3.0)

project(ZLIB C)

 set(zlib_directory zlib-1.2.3)

if(CMAKEPORTS_ASSEMBLE)
    # This section is used only in CMakePorts SVN
    set(zlib_directory ${zlib_directory} PARENT_SCOPE)
    set(zlib_url http://www.zlib.net/zlib-1.2.3.tar.gz PARENT_SCOPE)
    set(zlib_tarball zlib-1.2.3.tar.gz   PARENT_SCOPE)
    set(zlib_patches zlib-1.2.3_cmp.diff PARENT_SCOPE)
endif()

if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${zlib_directory}")
    add_subdirectory(${zlib_directory})
endif()

# Allow users to rename the library that gets built
if(TARGET zlib AND CMAKEPORTS_NAME_PREFIX)
    set_target_properties(zlib PROPERTIES OUTPUT_NAME "${CMAKEPORTS_NAME_PREFIX}zlib")
endif()

# Compatibility settings for interoperability with FindZLIB.cmake
set(ZLIB_FOUND true                               PARENT_SCOPE)
set(ZLIB_LIBRARY zlib                             PARENT_SCOPE)
set(ZLIB_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/${zlib_directory} PARENT_SCOPE)
set(ZLIB_LIBRARIES zlib                           PARENT_SCOPE)
