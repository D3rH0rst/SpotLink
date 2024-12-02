@echo off
if not exist build\ (
    mkdir build
)
setlocal enabledelayedexpansion
set "Compiler=gcc"
set "Wflags=-Wall -Wextra"
set "Buildflags="
set "Outputfile=./build/SpotLinkInjector.exe"
set "Includepaths=-I./include"
set "Inputfiles=./src/injector.c"
set "Linkerpaths=-L./lib"
set "Linkerflags="

:: Parse command-line arguments for -D flags
set "Defines="
:parse_args
if "%~1"=="" goto build
set "var=%~1"
if "%var:~0,2%"=="-D" (
    shift
    set "Defines=!Defines! %~1"
) else (
    shift
)
goto parse_args

:build
@echo on
%Compiler% %Wflags% %Buildflags% -o %Outputfile% %Inputfiles% %Includepaths% %Linkerpaths% %Linkerflags% %Defines%
@echo off
echo.
echo -------------------------------------
if errorlevel 1 (
    echo Build Failed %date% %time:~0,8%
) else (
    echo Build Complete %date% %time:~0,8%
)
echo -------------------------------------
echo.