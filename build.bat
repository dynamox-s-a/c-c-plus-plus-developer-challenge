@echo off
REM =============================================================================
REM Build script for Dynamox Arithmetic Calculator (Windows)
REM =============================================================================
REM
REM This script attempts to find and use available C compilers on Windows:
REM   1. Microsoft Visual C++ (cl.exe)
REM   2. MinGW GCC (gcc.exe)
REM   3. TDM-GCC or other GCC installations
REM
REM Usage: build.bat [clean]
REM =============================================================================

setlocal enabledelayedexpansion

if "%1"=="clean" (
    echo Cleaning build artifacts...
    del /Q /F calculator.exe 2>nul
    del /Q /F *.o 2>nul
    del /Q /F lib\matrix\*.o 2>nul
    del /Q /F lib\operations\*.o 2>nul
    del /Q /F lib\registry\*.o 2>nul
    del /Q /F lib\logger\*.o 2>nul
    del /Q /F lib\hmi\*.o 2>nul
    echo Clean complete.
    exit /b 0
)

echo Building Dynamox Arithmetic Calculator...

REM Try to find GCC first
where gcc >nul 2>&1
if %errorlevel%==0 (
    echo Found GCC in PATH
    set COMPILER=gcc
    set COMPILE_CMD=gcc -Wall -Wextra -Wpedantic -std=c11 -O2 -o calculator.exe main.c lib/matrix/matrix.c lib/operations/operations.c lib/registry/registry.c lib/logger/logger.c lib/hmi/hmi.c -lm
    goto :compile
)

REM Try to find MinGW GCC in common locations
for %%p in ("C:\MinGW\bin\gcc.exe" "C:\TDM-GCC-64\bin\gcc.exe" "C:\TDM-GCC-32\bin\gcc.exe" "C:\msys64\mingw64\bin\gcc.exe" "C:\msys64\mingw32\bin\gcc.exe") do (
    if exist %%p (
        echo Found GCC at %%p
        set COMPILER=%%p
        set COMPILE_CMD=%%p -Wall -Wextra -Wpedantic -std=c11 -O2 -o calculator.exe main.c lib/matrix/matrix.c lib/operations/operations.c lib/registry/registry.c lib/logger/logger.c lib/hmi/hmi.c -lm
        goto :compile
    )
)

REM Try Microsoft Visual C++
where cl >nul 2>&1
if %errorlevel%==0 (
    echo Found Microsoft Visual C++ compiler
    set COMPILER=cl
    set COMPILE_CMD=cl /W4 /O2 /Fe:calculator.exe main.c lib/matrix/matrix.c lib/operations/operations.c lib/registry/registry.c lib/logger/logger.c lib/hmi/hmi.c
    goto :compile
)

echo ERROR: No C compiler found!
echo.
echo Please install one of the following:
echo   1. MinGW-w64: https://www.mingw-w64.org/downloads/
echo   2. TDM-GCC: https://jmeubank.github.io/tdm-gcc/
echo   3. Microsoft Visual Studio Build Tools
echo   4. MSYS2: https://www.msys2.org/
echo.
echo Or manually compile with:
echo   gcc -Wall -Wextra -Wpedantic -std=c11 -O2 -o calculator.exe main.c lib/matrix/matrix.c lib/operations/operations.c lib/registry/registry.c lib/logger/logger.c lib/hmi/hmi.c -lm
exit /b 1

:compile
echo Compiling with: %COMPILER%
echo Command: %COMPILE_CMD%
echo.

%COMPILE_CMD%

if %errorlevel%==0 (
    echo.
    echo ✓ Build successful! Run with: calculator.exe
    echo.
) else (
    echo.
    echo ✗ Build failed with error code %errorlevel%
    exit /b %errorlevel%
)

endlocal
