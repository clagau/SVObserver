
rem this will get the 5 sec timeout value back
attrib -h -r -s c:\boot.ini
attrib -h -r -s c:\boot.svr
copy c:\boot.svr c:\boot.ini
attrib +h +r +s c:\boot.ini

regedit.exe /s c:\SVObserver\bin\SVAutoLogonSVIMRun.reg

del "C:\Documents and Settings\Administrator\Start Menu\Programs\Startup\svrunonce.cmd"

rem restart computer
shutdown /L /R /C /Y /T:2