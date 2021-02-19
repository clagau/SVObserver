New-NetSwitchTeam -Name "Team Camera 2" -TeamMembers "GigE_Port_003","GigE_Port_004"

nvspbind /d "Team Camera 2" ms_tcpip6 
nvspbind /d "Team Camera 2" ms_server
nvspbind /d "Team Camera 2" ms_pacer  
nvspbind /d "Team Camera 2" ms_msclient 
nvspbind /d "Team Camera 2" ms_rmcast 
nvspbind /d "Team Camera 2" ms_lltdio 
nvspbind /d "Team Camera 2" ms_rspndr 

netsh interface ipv4 set address name="Team Camera 2" static 192.168.103.1 255.255.255.0

# Pipe something into GigEBinds to make the pause go away at the end
'' | C:\SVObserver\bin\GigEBinds.bat
