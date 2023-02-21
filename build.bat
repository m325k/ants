@echo off
mkdir .\build
pushd .\build
cl -Zi /EHsc ..\code\win32_grafika.cpp user32.lib gdi32.lib opengl32.lib winmm.lib
popd