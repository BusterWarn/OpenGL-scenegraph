project(JPEG C)

include(CheckIncludeFile)
CHECK_INCLUDE_FILE(stddef.h HAVE_STDDEF_H)
CHECK_INCLUDE_FILE(stdlib.h HAVE_STDLIB_H)
if(WIN32)
    # This is for jpeg binaries and is #defined in jconfig.h
    set(TWO_FILE_COMMANDLINE true)
endif()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/jconfig.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/jconfig.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "D")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(JPEG_PUBLIC_HDRS
    jerror.h
    jmorecfg.h
    jpeglib.h
    ${CMAKE_CURRENT_BINARY_DIR}/jconfig.h
)
set(JPEG_PRIVATE_HDRS
    cderror.h
    cdjpeg.h
    jdct.h
    jinclude.h
    jmemsys.h
    jpegint.h
    jversion.h
    transupp.h
)

# memmgr back ends: compile only one of these into a working library
# (For now, let's use the mode that requires the image fit into memory.
# This is the recommended mode for Win32 anyway.)
SET(JPEG_systemdependent_SRCS jmemnobs.c)

set(JPEG_SRCS
    jaricom.c jcapimin.c jcapistd.c jcarith.c jccoefct.c jccolor.c
    jcdctmgr.c jchuff.c jcinit.c jcmainct.c jcmarker.c jcmaster.c
    jcomapi.c jcparam.c jcprepct.c jcsample.c jctrans.c jdapimin.c
    jdapistd.c jdarith.c jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c
    jddctmgr.c jdhuff.c jdinput.c jdmainct.c jdmarker.c jdmaster.c
    jdmerge.c jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c
    jfdctfst.c jfdctint.c jidctflt.c jidctfst.c jidctint.c jquant1.c
    jquant2.c jutils.c jmemmgr.c)

add_library(jpeg ${JPEG_systemdependent_SRCS} ${JPEG_SRCS} ${JPEG_PUBLIC_HDRS} ${JPEG_PRIVATE_HDRS})
IF(NOT EXECUTABLE_OUTPUT_PATH)
  SET(EXECUTABLE_OUTPUT_PATH bin)
ENDIF()
set_target_properties(jpeg PROPERTIES VERSION 7.0.0 RUNTIME_OUTPUT_PATH satan)
set_target_properties(jpeg PROPERTIES SOVERSION 7)
if(WIN32 AND BUILD_SHARED_LIBS)
    # We add JPEG_DLL only to building of this target because bad things
    # happen if it's enabled on all binary targets owing to the macros
    # defined in jmorecfg.h
    set_target_properties(jpeg PROPERTIES COMPILE_FLAGS -DJPEG_DLL)
endif()


set(JPEG_MANPAGES cjpeg.1 djpeg.1 jpegtran.1 rdjpgcom.1 wrjpgcom.1)

add_executable(cjpeg cjpeg.c rdppm.c rdgif.c rdtarga.c rdrle.c rdbmp.c rdswitch.c cdjpeg.c)
target_link_libraries(cjpeg jpeg)

add_executable(djpeg djpeg.c wrppm.c wrgif.c wrtarga.c wrrle.c wrbmp.c rdcolmap.c cdjpeg.c)
target_link_libraries(djpeg jpeg)

add_executable(jpegtran jpegtran.c rdswitch.c cdjpeg.c transupp.c)
target_link_libraries(jpegtran jpeg)

add_executable(rdjpgcom rdjpgcom.c)
add_executable(wrjpgcom wrjpgcom.c)

if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL )
    install(TARGETS jpeg
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib )
endif()
if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL )
    install(FILES ${JPEG_PUBLIC_HDRS} DESTINATION include)
endif()
if(NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL)
    install(FILES ${JPEG_MANPAGES} DESTINATION share/man/man1)
endif()
if(NOT SKIP_INSTALL_EXECUTABLES AND NOT SKIP_INSTALL_ALL)
    install(TARGETS cjpeg djpeg jpegtran rdjpgcom wrjpgcom DESTINATION bin)
endif()
