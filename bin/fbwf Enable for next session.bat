@echo off
bcdedit.exe /set {bootloadersettings} BOOTSTATUSPOLICY IgnoreAllFailures
echo.
fbwfmgr /enable
echo.
eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "FBWF enabled next Session"

fbwfmgr /setthreshold  64
fbwfmgr /setcompression  0
fbwfmgr /setpreallocation 1
echo.

fbwfmgr/addvolume C:
fbwfmgr/addexclusion C: "\RUN\"
fbwfmgr/addexclusion C: "\SVObserver\"
echo.


echo.