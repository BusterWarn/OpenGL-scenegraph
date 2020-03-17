# Based on http://freya3d.org/browser/CMakeFind/FindAssimp.cmake
# Based on http://www.daimi.au.dk/~cgd/code/extensions/Assimp/FindAssimp.cmake
# - Try to find Assimp
# Once done this will define
#
#  ASSIMP_FOUND - system has Assimp
#  ASSIMP_INCLUDE_DIR - the Assimp include directory
#  ASSIMP_LIBRARY - Link these to use Assimp
#  ASSIMP_LIBRARIES

  

find_path (ASSIMP_INCLUDE_DIR NAMES assimp/scene.h
  HINTS
  $ENV{DEPENDENCY_PATH}/include
  PATH_SUFFIXES lib lib64 win32/Dynamic_Release "Win32/${MSVC_YEAR_NAME}/x64/Release" "Win32/${MSVC_YEAR_NAME}/Win32/Release"
  PATHS ${ASSIMP_SEARCH_PATHS}
)
 
find_library (ASSIMP_LIBRARY_DEBUG NAMES assimpd libassimpd libassimp_d PATHS ${ASSIMP_SEARCH_PATHS})
find_library (ASSIMP_LIBRARY_RELEASE NAMES assimp libassimp PATHS ${ASSIMP_SEARCH_PATHS})

if (ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARY_RELEASE)
  set(ASSIMP_FOUND TRUE)
endif()

if (ASSIMP_LIBRARY_RELEASE)
    set (ASSIMP_LIBRARIES ${ASSIMP_LIBRARY_RELEASE})
endif()

if (ASSIMP_LIBRARY_DEBUG AND ASSIMP_LIBRARY_RELEASE)
    set (ASSIMP_LIBRARIES debug ${ASSIMP_LIBRARY_DEBUG} optimized ${ASSIMP_LIBRARY_RELEASE} )
endif()


if (ASSIMP_FOUND)
  MESSAGE("-- Found Assimp ${ASSIMP_LIBRARIES}")
  mark_as_advanced (ASSIMP_INCLUDE_DIR ASSIMP_LIBRARY ASSIMP_LIBRARIES)
endif()
