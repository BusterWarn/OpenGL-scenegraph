@echo off
set A_CURR_DIR=%~d0%~p0
if "%CMAKE_GENERATOR_VERSION%"=="" (
  call "%A_CURR_DIR%"\setup_env.bat %1
)
IF not errorlevel 0 goto error

IF NOT EXIST "%A_CURR_DIR%dependencies" (
  echo.
  echo.
  echo *** %A_CURR_DIR%dependencies is missing. Please download and unzip to that directory before running this script again
  goto error
)


set LABEL=%1


mkdir "%A_CURR_DIR%\installed\include\rapidxml\"
copy "%A_CURR_DIR%\dependencies\rapidxml-1.13\*.hpp" "%A_CURR_DIR%\installed\include\rapidxml\"


if NOT "%LABEL%"=="" goto %1

IF "%CMAKE_GENERATOR_VERSION%"=="" (
  echo *** setup_env.bat has not been setup correctly
  goto error
)

:: Build FreeGLUT
:glut
mkdir "%A_CURR_DIR%\build"
mkdir "%A_CURR_DIR%\build\freeglut"
pushd "%A_CURR_DIR%\build\freeglut"
IF not errorlevel 0 goto error
cmake -G "%CMAKE_GENERATOR_VERSION%" "%A_CURR_DIR%\dependencies\freeglut3.0.0" -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed" -DFREEGLUT_BUILD_DEMOS=off -DFREEGLUT_BUILD_STATIC_LIBS=off 
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" freeglut.sln /release /WIN64
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" freeglut.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error

popd
IF NOT "%LABEL%"=="" goto exit

:glm
mkdir "%A_CURR_DIR%\build"
mkdir "%A_CURR_DIR%\build\glm"
pushd "%A_CURR_DIR%\build\glm"
IF not errorlevel 0 goto error
cmake -G "%CMAKE_GENERATOR_VERSION%" "%A_CURR_DIR%\dependencies\glm" -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed" 
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" glm.sln /release /WIN64
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" glm.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error

popd
IF NOT "%LABEL%"=="" goto exit



:glew
mkdir "%A_CURR_DIR%\build"
mkdir "%A_CURR_DIR%\build\glew"
pushd "%A_CURR_DIR%\build\glew"
IF not errorlevel 0 goto error
cmake -G "%CMAKE_GENERATOR_VERSION%" "%A_CURR_DIR%\dependencies\glew-2.0.0" -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed" 
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" glew.sln /release /WIN64
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" glew.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error
popd
IF NOT "%LABEL%"=="" goto exit


:zlib
mkdir "%A_CURR_DIR%\build"
mkdir "%A_CURR_DIR%\build\zlib"
pushd "%A_CURR_DIR%\build\zlib"
IF not errorlevel 0 goto error
cmake -G "%CMAKE_GENERATOR_VERSION%" "%A_CURR_DIR%\dependencies\zlib"  -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed" 
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" zlib.sln /release /WIN64
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" zlib.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error
popd
IF NOT "%LABEL%"=="" goto exit


:assimp
mkdir "%A_CURR_DIR%\build"
mkdir "%A_CURR_DIR%\build\assimp"
pushd "%A_CURR_DIR%\build\assimp"
IF not errorlevel 0 goto error
cmake -G "%CMAKE_GENERATOR_VERSION%" "%A_CURR_DIR%\dependencies\assimp-3.3.1" -DCMAKE_CXX_FLAGS=/DASSIMP_BUILD_NO_XFILE_EXPORTER=1 -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_ASSIMP_TOOLS=off -DASSIMP_BUILD_X_IMPORTER=off -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed" -DZLIB_INCLUDE_DIR="%A_CURR_DIR%\installed\include"
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" assimp.sln /release /WIN64
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" assimp.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error
popd
IF NOT "%LABEL%"=="" goto exit

:freetype
mkdir "%A_CURR_DIR%\build"
mkdir "%A_CURR_DIR%\build\freetype-2.3.11"
pushd "%A_CURR_DIR%\build\freetype-2.3.11"
IF not errorlevel 0 goto error
cmake -G "%CMAKE_GENERATOR_VERSION%" "%A_CURR_DIR%\dependencies\freetype-2.3.11"   -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed"
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" freetype.sln /release /WIN64
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" freetype.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error
popd
IF NOT "%LABEL%"=="" goto exit

:jpeg
mkdir "%A_CURR_DIR%\build"
mkdir "%A_CURR_DIR%\build\jpeg"
pushd "%A_CURR_DIR%\build\jpeg"
IF not errorlevel 0 goto error
cmake -G "%CMAKE_GENERATOR_VERSION%" "%A_CURR_DIR%\dependencies\jpeg"   -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed"
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" jpeg.sln /release /WIN64
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" jpeg.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error
popd
IF NOT "%LABEL%"=="" goto exit

:png
mkdir "%A_CURR_DIR%\build"
mkdir "%A_CURR_DIR%\build\png"
pushd "%A_CURR_DIR%\build\png"
IF not errorlevel 0 goto error
cmake -G "%CMAKE_GENERATOR_VERSION%" "%A_CURR_DIR%\dependencies\png"   -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed"
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" png.sln /release /WIN64
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" png.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error
popd
IF NOT "%LABEL%"=="" goto exit

:soil
mkdir "%A_CURR_DIR%\build"
mkdir "%A_CURR_DIR%\build\soil"
pushd "%A_CURR_DIR%\build\soil"
IF not errorlevel 0 goto error
cmake -G "%CMAKE_GENERATOR_VERSION%" "%A_CURR_DIR%\dependencies\SOIL"   -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed"
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" soil.sln /release /WIN64
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" soil.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error
popd
IF NOT "%LABEL%"=="" goto exit

set


:vrlib
set FREETYPE_DIR="%A_CURR_DIR%"\installed\



cmake -G "%CMAKE_GENERATOR_VERSION%" "%A_CURR_DIR%\"  -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed" -DZLIB_INCLUDE_DIR="%A_CURR_DIR%\installed\include"
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" vrlib.sln /release /WIN64
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" vrlib.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error


IF NOT "%LABEL%"=="" goto exit


:error
echo **** An error occurred 
exit /b 1

:exit
exit /b 0
