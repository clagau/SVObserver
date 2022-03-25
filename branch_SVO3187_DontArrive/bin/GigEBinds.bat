REM MODE 120,60
@echo off
setlocal enabledelayedexpansion

REM nvsbind: 
REM /u unbind
REM /d disable binding

set adapter[0]="GigE_Port_001"
set adapter[1]="GigE_Port_002"
set adapter[2]="GigE_Port_003"
set adapter[3]="GigE_Port_004"

for /l %%n in (0,1,3) do (
  nvspbind /d !adapter[%%n]! ms_tcpip6 
  nvspbind /d !adapter[%%n]! ms_netbios
  nvspbind /d !adapter[%%n]! ms_pacer
  nvspbind /d !adapter[%%n]! ms_wfplwf
  nvspbind /d !adapter[%%n]! ms_msclient
  nvspbind /d !adapter[%%n]! ms_rmcast
  nvspbind /d !adapter[%%n]! ms_ndisuio
  nvspbind /d !adapter[%%n]! ms_netbt
  nvspbind /d !adapter[%%n]! ms_smb
  nvspbind /d !adapter[%%n]! ms_pppoe
  nvspbind /d !adapter[%%n]! ms_rspndr
  nvspbind /d !adapter[%%n]! ms_lltdio
  nvspbind /d !adapter[%%n]! ms_server
  nvspbind /d !adapter[%%n]! ms_server

  REM bind teaming protocol
  REM nvspbind /e !adapter[%%n]! iansprotocol
)

echo.
echo #################################################################
echo #################################################################
echo #################################################################
echo.

for /l %%n in (0,1,3) do (
  nvspbind !adapter[%%n]!
)

pause
exit

