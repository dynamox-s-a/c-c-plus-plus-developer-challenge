@echo off
REM =============================================================================
REM Test script for Dynamox Arithmetic Calculator
REM =============================================================================

echo Building and running unit tests...

REM Try to find GCC first
where gcc >nul 2>&1
if %errorlevel%==0 (
    echo Found GCC in PATH
    gcc -Wall -Wextra -Wpedantic -std=c11 -O2 -o test_simple.exe test_simple.c lib/operations/operations.c lib/matrix/matrix.c -lm
    goto :run_test
)

REM Try to find MinGW GCC in common locations
for %%p in ("C:\MinGW\bin\gcc.exe" "C:\TDM-GCC-64\bin\gcc.exe" "C:\TDM-GCC-32\bin\gcc.exe" "C:\msys64\mingw64\bin\gcc.exe" "C:\msys64\mingw32\bin\gcc.exe") do (
    if exist %%p (
        echo Found GCC at %%p
        %%p -Wall -Wextra -Wpedantic -std=c11 -O2 -o test_simple.exe test_simple.c lib/operations/operations.c lib/matrix/matrix.c -lm
        goto :run_test
    )
)

echo ERROR: No C compiler found! Please install GCC or MinGW.
echo See INSTALL.md for instructions.
exit /b 1

:run_test
if %errorlevel%==0 (
    echo.
    echo Running tests...
    echo.
    .\test_simple.exe
) else (
    echo Test compilation failed!
    exit /b %errorlevel%
)
