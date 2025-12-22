@echo off

pushd %~dp0..
.\tools\premake5 --config=Release ecc
popd
exit /b 0
