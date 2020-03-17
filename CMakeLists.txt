# Name of whole project (only useful in visualstudio)
SET(TARGET_NAME vrlib)
project(${TARGET_NAME})

#SET(${TARGET_NAME}_MAJOR_VERSION 2)
#SET(${TARGET_NAME}_MINOR_VERSION 0)
#SET(${TARGET_NAME}_PATCH_VERSION 0)
#configure_file(doc/Doxyfile.in doc/Doxyfile @ONLY)
cmake_minimum_required(VERSION 2.8)

IF(WIN32)
ELSE()
  SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
ENDIF(WIN32)

#if (COMMAND cmake_policy)
#  cmake_policy(SET CMP0003 OLD)
#endif (COMMAND cmake_policy)


SET(${TARGET_NAME}_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(${TARGET_NAME}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include )
SET(${TARGET_NAME}_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)


# Path to other .cmake files
SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")


# FInd a few useful packages
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)
FIND_PACKAGE(Freetype)
FIND_PACKAGE(GLM REQUIRED)
FIND_PACKAGE(SOIL REQUIRED)

FIND_PACKAGE(Assimp REQUIRED)


IF(NOT WIN32)
  FIND_PACKAGE(Threads) 
ENDIF()

FIND_PACKAGE(JPEG REQUIRED)

FIND_PACKAGE(PNG)

# Did we find them?
IF(NOT GLUT_FOUND)
  MESSAGE(FATAL_ERROR " Unable to locate GLUT, download and install")
ENDIF()

IF(NOT FREETYPE_FOUND)
  MESSAGE(WARNING " Building without Freetype. Unable to locate FREETYPE, download and install")
  SET(FREETYPE_LIBRARY "")
ELSE()
  ADD_DEFINITIONS("-DHAVE_FREETYPE")
ENDIF()

IF(NOT GLEW_FOUND)
  MESSAGE(FATAL_ERROR " Unable to locate GLEW, download and install")
ENDIF()

# Did we find them?
IF(NOT PNG_FOUND)
  MESSAGE(FATAL_ERROR " Unable to locate PNG, download and install")
ENDIF()

# Did we find them?
IF(NOT JPEG_FOUND)
  MESSAGE(FATAL_ERROR " Unable to locate JPEG, download and install")
ENDIF()

# Did we find them?
IF(NOT ZLIB_FOUND)
  MESSAGE(FATAL_ERROR " Unable to locate ZLIB, download and install")
ENDIF()

# Did we find them?
IF(NOT GLM_FOUND)
  MESSAGE(FATAL_ERROR " Unable to locate GLM, download and install")
ENDIF(NOT GLM_FOUND)

# Did we find them?
IF(NOT SOIL_FOUND)
  MESSAGE(FATAL_ERROR " Unable to locate SOIL, download and install")
ENDIF()

# Did we find them?
IF(NOT ASSIMP_FOUND)
  MESSAGE(FATAL_ERROR " Unable to locate Assimp, download and install")
ENDIF()


# Set the default include directories
INCLUDE_DIRECTORIES(
  ${JPEG_INCLUDE_DIR}
  ${PNG_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
  ${${TARGET_NAME}_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${GLUT_INCLUDE_DIR}
  ${GLM_INCLUDE_DIR}
  ${IL_INCLUDE_DIR}
  ${ASSIMP_INCLUDE_DIR}
  ${SOIL_INCLUDE_DIR}
)

IF (FREETYPE_FOUND)
  INCLUDE_DIRECTORIES(
    ${FREETYPE_INCLUDE_DIR_ft2build}
    ${FREETYPE_INCLUDE_DIR_ft2build}/freetype2
  )
ENDIF()


# Go through subdirectories
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(scenegraph)

