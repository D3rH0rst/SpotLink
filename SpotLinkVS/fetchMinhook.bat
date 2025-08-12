@echo off
setlocal

REM Args from Visual Studio
set "SOL_DIR=%~1"
set "CONFIG=%~2"

set "MINHOOK_DIR=%SOL_DIR%external\minhook"
set "MINHOOK_LIB_DIR=%MINHOOK_DIR%\build\VC17\lib\%CONFIG%"

REM Step 1: Clone if missing
if not exist "%MINHOOK_DIR%\.git" (
    echo Cloning MinHook from GitHub...
    git clone https://github.com/TsudaKageyu/minhook.git "%MINHOOK_DIR%" --depth=1
)

REM Step 2: Build MinHook
if not exist "%MINHOOK_LIB_DIR%" (
    echo Building MinHook for %CONFIG%...
    msbuild "%MINHOOK_DIR%\build\VC17\MinHookVC17.sln" /p:Configuration=%CONFIG% /p:Platform=x64
)

endlocal
