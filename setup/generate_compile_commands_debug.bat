@echo off

pushd %~dp0..
.\tools\premake5 --config=Debug ecc
popd
exit /b 0
