New-NetSwitchTeam -Name "Team Camera 1" -TeamMembers "GigE_Port_001","GigE_Port_002"

nvspbind /d "Team Camera 1" ms_tcpip6 
nvspbind /d "Team Camera 1" ms_server
nvspbind /d "Team Camera 1" ms_pacer  
nvspbind /d "Team Camera 1" ms_msclient 
nvspbind /d "Team Camera 1" ms_rmcast 
nvspbind /d "Team Camera 1" ms_lltdio 
nvspbind /d "Team Camera 1" ms_rspndr 

netsh interface ipv4 set address name="Team Camera 1" static 192.168.101.1 255.255.255.0

# Pipe something into GigEBinds to make the pause go away at the end
'' | C:\SVObserver\bin\GigEBinds.bat
