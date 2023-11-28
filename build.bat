@echo off

rmdir /q /s build
mkdir build

cmake -S . -B build -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug

rem build\svm.exe
rem build\app.exe