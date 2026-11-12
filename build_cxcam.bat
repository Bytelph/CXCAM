@echo off
call "D:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d C:\Users\BYTELPH\Desktop\CXCAM
"D:\Qt\Tools\CMake_64\bin\cmake.exe" --build build
