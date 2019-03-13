@echo off
c:\"Program Files"\Intel\DMIX\CL\ProSetCl.exe Team_Enumerate

set /p TeamIndex=What Team would you like to remove?

IF "%TeamIndex%" == "1" goto :cmd
IF "%TeamIndex%" == "2" goto :cmd
goto :error

:cmd
eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "Remove camera team %TeamIndex%"
c:\"Program Files"\Intel\DMIX\CL\ProSetCl.exe Team_Remove %TeamIndex%

call GigEBinds.bat
goto :end

:error
echo Invalid index


:end
pause
