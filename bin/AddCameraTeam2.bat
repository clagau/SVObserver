REM *** Turn Teaming on for GigE_Port_003 (4) & GigE_Port_004 (3)
c:\"Program Files"\Intel\DMIX\CL\ProSetCl.exe Team_Create 4,3 "Camera Team 2" SLA


REM *** Rename Local Area Connection to Team Camera 2
eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "Add camera team2"
netsh interface set interface name="Local Area Connection" newname="Team Camera 2"

nvspbind /d "Team Camera 2" ms_tcpip6
nvspbind /d "Team Camera 2" ms_server
nvspbind /d "Team Camera 2" ms_pacer
nvspbind /d "Team Camera 2" ms_msclient
nvspbind /d "Team Camera 2" ms_rmcast
nvspbind /d "Team Camera 2" ms_lltdio
nvspbind /d "Team Camera 2" ms_rspndr


REM *** Set Static IP addres
netsh interface ipv4 set address name="Team Camera 2" static 192.168.103.1 255.255.255.0