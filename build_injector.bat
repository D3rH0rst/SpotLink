@echo off
if not exist build\ (
    mkdir build
)
setlocal enabledelayedexpansion
set "Compiler=gcc"
set "Wflags=-Wall -Wextra"
set "Buildflags="
set "Outputdir=.\build\"
set "Outputfile=SpotLinkInjector"
set "Includepaths=-I./include"
set "Inputfiles=./src/injector.c"
set "Linkerpaths=-L./lib"
set "Linkerflags="

::set "Linktarget='%~dp0\build\%Outputfile%.exe'"
::set "Shortcut='%~dp0\build\%Outputfile%.lnk'"
::set "PWS=powershell.exe -ExecutionPolicy Bypass -NoLogo -NonInteractive -NoProfile"


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
%Compiler% %Wflags% %Buildflags% -o %Outputdir%%Outputfile%.exe %Inputfiles% %Includepaths% %Linkerpaths% %Linkerflags% %Defines%
@echo off
echo.
echo -------------------------------------
if errorlevel 1 (
    echo Build Failed %date% %time:~0,8%
) else (
    echo Build Complete %date% %time:~0,8%
    ::%PWS% -Command "$ws = New-Object -ComObject WScript.Shell; $s = $ws.CreateShortcut(%Shortcut%); $S.TargetPath = %Linktarget%; $S.Save()"
)
echo -------------------------------------
echo.