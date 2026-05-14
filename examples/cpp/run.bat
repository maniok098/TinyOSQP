@echo off
REM Clean build directory
if exist build (
    rmdir /s /q build
)

REM Configure with CMake
cmake -B build || exit /b 1

REM Build (Release by default)
cmake --build build --config Release || exit /b 1

REM Run executable
build\Release\OSQP_Example.exe
