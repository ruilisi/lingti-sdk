@echo off
setlocal enabledelayedexpansion
REM Build script for SDK example (Windows)

echo ========================================
echo Building Lingti SDK Example
echo ========================================
echo.

REM Check for MinGW GCC
where gcc >nul 2>&1
if !ERRORLEVEL! EQU 0 (
    echo Using MinGW GCC compiler...
    gcc sdk_example.c -L.. -llingti_sdk -o sdk_example.exe
    if !ERRORLEVEL! EQU 0 (
        echo.
        echo Build successful! Created: sdk_example.exe
        echo.
        echo To run: sdk_example.exe
        goto :end
    ) else (
        echo Build failed!
        goto :error
    )
)

REM Check for MSVC
where cl >nul 2>&1
if !ERRORLEVEL! EQU 0 (
    echo Using MSVC compiler...
    cl /nologo sdk_example.c ..\lingti_sdk.lib
    if !ERRORLEVEL! EQU 0 (
        echo.
        echo Build successful! Created: sdk_example.exe
        echo.
        echo To run: sdk_example.exe
        goto :end
    ) else (
        echo Build failed!
        goto :error
    )
)

echo ERROR: No suitable C compiler found!
echo Please install MinGW GCC or Visual Studio.
goto :error

:error
endlocal
echo.
pause
exit /b 1

:end
endlocal
echo.
pause
