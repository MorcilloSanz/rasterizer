@echo off
setlocal

REM ============================================================================
REM  compile.bat
REM
REM  Builds and runs the test program (test/main.c) with GCC.
REM
REM  What it does:
REM    1. Creates the "build" directory if it doesn't exist.
REM    2. Copies the test assets (Roboto-Light.ttf and sprite.png) next to
REM       the executable so the program can load them at runtime.
REM    3. Compiles test/main.c as C11 using the project root as the include
REM       path (header-only library), producing build/main.exe.
REM    4. If compilation succeeds, runs main.exe from the build directory;
REM       otherwise prints an error message.
REM
REM  Requirements: GCC (e.g. MinGW-w64) available in PATH.
REM  Usage: run this script from the project root.
REM ============================================================================
REM
REM author: Alberto Morcillo Sanz
REM GitHub: MorcilloSanz

REM Source files
SET SRC="test/main.c"

REM Include directory
SET INCLUDE_DIR="."

REM File name of the executable
SET EXE="main.exe"

REM Create build directory if it doesn't exist
if not exist build mkdir build

REM Copy assets next to the executable
copy /Y "test\Roboto-Light.ttf" build\ >nul
copy /Y "test\sprite.png" build\ >nul

REM Compile the source files into an executable
gcc -std=c11 -I%INCLUDE_DIR% %SRC% -o build/%EXE%

REM Check if the compilation was successful
if %errorlevel% equ 0 (
    echo [32mCompilation successful[0m
    cd build && %EXE%
) else (
    echo [31mCompilation failed[0m
)