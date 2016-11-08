@echo off
eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "FBWF show status"
fbwfmgr
echo.
pause