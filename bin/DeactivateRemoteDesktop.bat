@echo off
echo This scipt will disable remote desktop connections.
echo This is the default setting for the SVIM.
echo SVObserver will start automatically on startup and logoff will restart the SVObserver.
pause
eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "Deactivate remote desktop"
net start mpssvc
reg add "HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\Terminal Server" /v fDenyTSConnections /t REG_DWORD /d 1 /f
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Terminal Server" /v fDenyTSConnection /t REG_DWORD /d 1 /f
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Terminal Server" /v fDenyTSConnections /t REG_DWORD /d 1 /f
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon" /v ForceAutoLogon /t REG_SZ /d 1 /f
sc config TermService start= disabled
net stop Termservice /y
net stop mpssvc