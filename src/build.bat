@echo off

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

set main_file=raytracer.cpp
set common_compiler_flags=-Od -Zi -EHsc
set include_dir= /I "../src"

cl ..\src\%main_file% %common_compiler_flags% %include_dir%
popd

REM pushd ..\data
REM ..\build\raytracer.exe
REM start test.ppm
REM popd
