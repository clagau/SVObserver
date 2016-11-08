@echo off
eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "FBWF overlay detail"
fbwfmgr /overlaydetail
echo.
pause