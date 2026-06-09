@echo off
setlocal

set SRC=C:\DX_ExtBuild\Extension.dll
set DST=Z:\DeusEx_RTX\System\Extension.dll
set BAK=Z:\DeusEx_RTX\System\Extension.dll.original

echo Installing Extension.dll to Z:\DeusEx_RTX\System

if not exist "%SRC%" (
    echo Source DLL not found: "%SRC%"
    exit /b 1
)

if exist "%DST%" (
    if not exist "%BAK%" (
        echo Backing up original Extension.dll
        copy /Y "%DST%" "%BAK%" >nul
        if errorlevel 1 exit /b 1
    )
)

copy /Y "%SRC%" "%DST%"
if errorlevel 1 exit /b 1

echo Installed Extension.dll
exit /b 0
