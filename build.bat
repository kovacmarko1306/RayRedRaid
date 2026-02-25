@echo off
REM RayRedRaid Build Script
REM Koristenje:
REM   BUILD.bat          - Release build (x64-windows-static) - PREPORUCENO!
REM   BUILD.bat Debug    - Debug build (x64-windows)
REM   BUILD.bat Clean    - Ocisti build dir

setlocal enabledelayedexpansion

REM Dodaj CMake u PATH ako vec nije dostupan
where cmake >nul 2>&1
if errorlevel 1 (
    set "PATH=C:\Program Files\Microsoft Visual Studio\18\Insiders\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;%PATH%"
)

set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Release

if /i "%CONFIG%"=="Clean" (
    echo Brisanje build direktorija...
    rmdir /s /q build 2>nul
    echo Ocisceno.
    exit /b 0
)

set TRIPLET=x64-windows-static
set BUILDDIR=build

if /i "%CONFIG%"=="Debug" (
    set TRIPLET=x64-windows
)

echo.
echo ==================================================
echo   RayRedRaid Build Script
echo ==================================================
echo Konfiguracija: %CONFIG%
echo Triplet:       %TRIPLET%
echo.

if not exist "%BUILDDIR%" mkdir "%BUILDDIR%"

echo [1/2] CMake konfiguracija...
cd "%BUILDDIR%"

cmake .. ^
    -G "Visual Studio 18 2026" ^
    -A x64 ^
    -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake ^
    -DVCPKG_TARGET_TRIPLET=%TRIPLET%

if errorlevel 1 (
    echo GRESKA: CMake konfiguracija neuspjesna!
    exit /b 1
)

echo.
echo [2/2] Build (%CONFIG%)...
cmake --build . --config %CONFIG%

if errorlevel 1 (
    echo GRESKA: Build neuspjesan!
    cd ..
    exit /b 1
)

set EXEPATH=RayRedRaid\%CONFIG%\RayRedRaid.exe
if exist "%EXEPATH%" (
    for /F "usebackq" %%A in ('%EXEPATH%') do set SIZE=%%~zA

    echo.
    echo ==================================================
    echo   BUILD USPJESAN!
    echo ==================================================
    echo Output:   %CD%\%EXEPATH%
    if defined SIZE (
        set /A SIZEMB=SIZE / 1048576
        if !SIZEMB! equ 0 set SIZEMB=1
        echo Velicina: !SIZEMB! MB
    )
    echo ==================================================
    echo.
) else (
    echo GRESKA: EXE nije pronađen: %EXEPATH%
    cd ..
    exit /b 1
)

cd ..
endlocal
