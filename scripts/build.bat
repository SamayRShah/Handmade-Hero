@echo off
rem Create build directory if it doesn't exist
if not exist "..\handmade-hero\build" (
    mkdir "..\handmade-hero\build"
)

rem Change to build directory
pushd "..\handmade-hero\build"

rem Compile program
g++ ..\src\win32_handmade_hero.cpp -o win32_handmade_hero.exe -g -mwindows -luser32 -lgdi32 -Wall

rem Check if compilation succeeded
if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Build failed.
    pause
) else (
    echo.
    echo [SUCCESS] Build succeeded.
)

rem Return to previous directory
popd
