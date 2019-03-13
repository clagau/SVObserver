@echo off
MODE 120,60
REM *** Turn Teaming on for GigE_Port_003 & GigE_Port_004
"c:\Program Files\Intel\DMIX\CL\ProSetCl.exe" Adapter_Enumerate

"c:\Program Files\Intel\DMIX\CL\ProSetCl.exe" Team_ValidateAdaptersForTeaming 2,5 

echo.  
echo Trying to create Team from adapter 2 and 5
"c:\Program Files\Intel\DMIX\CL\ProSetCl.exe" Team_Create 2,5 "Camera Team 2" SLA

if errorlevel 1 goto error

REM *** Rename Local Area Connection to Team Camera 1
eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "Add camera Team 2" >NUL
netsh interface set interface name="Local Area Connection" newname="Team Camera 2"

nvspbind /d "Team Camera 2" ms_tcpip6 >NUL
nvspbind /d "Team Camera 2" ms_server >NUL
nvspbind /d "Team Camera 2" ms_pacer  >NUL
nvspbind /d "Team Camera 2" ms_msclient >NUL
nvspbind /d "Team Camera 2" ms_rmcast >NUL
nvspbind /d "Team Camera 2" ms_lltdio >NUL
nvspbind /d "Team Camera 2" ms_rspndr >NUL


REM *** Set Static IP addres
netsh interface ipv4 set address name="Team Camera 2" static 192.168.103.1 255.255.255.0 >NUL

call GigEBinds.bat
pause
exit

:error
echo.
echo.
echo ERROR: Could not create Team.
echo.
pause
exit