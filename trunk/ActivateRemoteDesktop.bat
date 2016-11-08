@echo off
echo This scipt will enable remote desktop connections.
echo It will allow to connect to this computer via network.
echo SVObserver will still start automatically on startup
echo But a login prompt will be shown when you logoff / switch the user or finish remote desktop session.
pause
eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "Activate remote desktop"
net start mpssvc
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Terminal Server" /v fDenyTSConnections /t REG_DWORD /d 0 /f
reg add "HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\Terminal Server"  /v fDenyTSConnections /t REG_DWORD /d 0 /f
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Terminal Server" /v fDenyTSConnection /t REG_DWORD /d 0 /f
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon" /v ForceAutoLogon /t REG_SZ /d 0 /f
sc config TermService start= auto
net start Termservice
net stop mpssvc