@echo off

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

set main_file=raytracer.cpp
set common_compiler_flags=-Od -Zi -EHsc -Wall -wd4820 -wd4640 -wd4350 -wd4710 -wd4514 -wd4625 -wd4571 -wd4265 -wd4626 -wd4100
set include_dir= /I "../src"

cl ..\src\%main_file% %common_compiler_flags% %include_dir%
popd

REM pushd ..\data
REM ..\build\raytracer.exe
REM start test.ppm
REM popd
