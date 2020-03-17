@echo off

::
:: Setting up Build and Runtime environment for AgX
::

set A_CURR_DIR=%~d0%~p0

set args=%*


:: Setup visual studio path

REM Default is Visual Studio 2017 Win64
if "%args%"=="" (
    set args="/VS2017 /WIN64"

)

:: Is /WIN64 specified?
set vs_build_type=x86
echo %args% | find /i "/WIN64" > nul
if not errorlevel 1 (
  set vs_build_type=Win64
)

:: Is /x64 specified?
echo %args% | find /i "/x64" > nul
if not errorlevel 1 (
  set vs_build_type=Win64
)

echo %args% | find /i "/VS2013" >NUL
if not errorlevel 1 (
  set CMAKE_GENERATOR_VERSION=Visual Studio 12 2013 %vs_build_type%
)

echo %args% | find /i "/VS2015" >NUL
if not errorlevel 1 (
  set CMAKE_GENERATOR_VERSION=Visual Studio 14 2015 %vs_build_type%
)

echo %args% | find /i "/VS2017" >NUL
if not errorlevel 1 (
  set CMAKE_GENERATOR_VERSION=Visual Studio 15 2017 %vs_build_type%
)


call %A_CURR_DIR%\setup_vs.bat %args%

set STATUS=%ERRORLEVEL%

pushd %A_CURR_DIR%
IF NOT "%STATUS%"=="0" (
  echo.
  echo *** No visual studio found ***
  goto error
)

set VR_PATH=%A_CURR_DIR%
set DEPENDENCY_PATH=%A_CURR_DIR%\installed


REM Add to path so .dll files can be found
set PATH=%PATH%;%DEPENDENCY_PATH%\bin;%VR_PATH%\bin
set LIB=%LIB%;%DEPENDENCY_PATH%\lib
set INCLUDE=%INCLUDE%;%DEPENDENCY_PATH%\include
goto exit

:error
exit /B 1

:exit
exit /B 0

