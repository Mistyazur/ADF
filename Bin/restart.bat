@echo off
echo Restarting...
REM Initialize
pushd "%~dp0"
set current_dir=%~dp0
REM DELAY
choice /t 1 /d y /n >nul
REM Run ExCapsLock
cmd /C START %currentdir%ADF.exe -a
popd
exit