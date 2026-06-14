@echo off
setlocal

REM ================================
REM Fast vcpkg dev settings
REM ================================

REM Enable vcpkg binary caching system
set VCPKG_FEATURE_FLAGS=binarycaching

REM Shared binary cache (major speedup for rebuilds)
set VCPKG_BINARY_SOURCES=clear;files,%~dp0..\vcpkg-cache,readwrite

REM Keep builds consistent (prevents unnecessary rebuilds)
set VCPKG_DEFAULT_TRIPLET=x64-windows

REM Faster parallel compilation
set VCPKG_MAX_CONCURRENCY=8

REM ================================
REM Install git submodules
REM ================================
pushd %~dp0..
echo Installing git submodules...
git submodule update --init --recursive

REM ================================
REM Install dependencies (vcpkg)
REM ================================
call vcpkg\bootstrap-vcpkg.bat

REM Manifest mode install (no explicit packages needed)
.\vcpkg\vcpkg install --host-triplet=x64-windows --x-install-root=vcpkg\installed

REM ================================
REM Generate project files
REM ================================
call setup\generate_vs_solution.bat

REM ================================
REM Cleanup
REM ================================
popd
exit /b 0