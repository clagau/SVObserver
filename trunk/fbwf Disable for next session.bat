@echo off
eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "FBWF disabled next Session"
fbwfmgr /disable
echo.