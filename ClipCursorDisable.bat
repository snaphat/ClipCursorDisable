@echo off
setlocal enabledelayedexpansion

REM Use first argument / dragged-on file as the target executable
if "%~1"=="" (
    echo No executable specified. Exiting...
    exit /b 1
)
set "TARGET_EXE=%~1"

:next

REM Check if the executable is 32-bit
for /f %%G in ('powershell -NoProfile -Command "(get-content '%TARGET_EXE%' -totalcount 50) | select-string -Pattern 'PE..L' -Quiet"') do (
    set "IS32EXE=%%G"
)
REM Check if the executable is 64-bit
for /f %%G in ('powershell -NoProfile -Command "(get-content '%TARGET_EXE%' -totalcount 50) | select-string -Pattern 'PE..d' -Quiet"') do (
    set "IS64EXE=%%G"
)

REM Check for 32-bit target executable
if "!IS32EXE!"=="True" (
    set "DLL_NAME=ClipCursorDisable_x86.dll"
    set "SETDLL=setdll_x86.exe"
    REM Additional actions for 32-bit executable...
)

REM Check for 64-bit target executable
if "!IS64EXE!"=="True" (
    set "DLL_NAME=ClipCursorDisable_x64.dll"
    set "SETDLL=setdll_x64.exe"
    REM Additional actions for 64-bit executable...
)

REM Prompt user for action
echo 1. Add ClipCursorDisable library to "%TARGET_EXE%"
echo 2. Remove ClipCursorDisable library from "%TARGET_EXE%"
set /p "ACTION=Enter action number (1 or 2): "

REM Process user's choice
if "!ACTION!"=="1" (
    REM Inject the ClipCursorDisable library using setdll.exe
    %SETDLL% /d:"%DLL_NAME%" "%TARGET_EXE%"
) else if "!ACTION!"=="2" (
    REM Remove the ClipCursorDisable library using setdll.exe
    %SETDLL% /r "%TARGET_EXE%"
) else (
    echo Invalid action number. Exiting...
)

endlocal
