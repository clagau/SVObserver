REM *** Turn Teaming on for GigE_Port_001 & GigE_Port_002
c:\"Program Files"\Intel\DMIX\CL\ProSetCl.exe Team_Create 6,2 "Camera Team 1" SLA


REM *** Rename Local Area Connection to Team Camera 1
eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "Add camera team1"
netsh interface set interface name="Local Area Connection" newname="Team Camera 1"

nvspbind /d "Team Camera 1" ms_tcpip6
nvspbind /d "Team Camera 1" ms_server
nvspbind /d "Team Camera 1" ms_pacer
nvspbind /d "Team Camera 1" ms_msclient
nvspbind /d "Team Camera 1" ms_rmcast
nvspbind /d "Team Camera 1" ms_lltdio
nvspbind /d "Team Camera 1" ms_rspndr


REM *** Set Static IP addres
netsh interface ipv4 set address name="Team Camera 1" static 192.168.101.1 255.255.255.0