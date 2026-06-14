@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64
cd /d "%~dp0.."
cmake --preset engine
cmake --build build --config Release
