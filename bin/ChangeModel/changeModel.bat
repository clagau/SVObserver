@echo off
set systemOEMPath="C:\Windows\System32"
set oemInfoPath="C:\SVObserver\bin\ChangeModel"

FC OEMINFO_NEO.ini %systemOEMPath%\OEMINFO.ini
set isNEO=%errorlevel%
FC OEMINFO_X2C.ini %systemOEMPath%\OEMINFO.ini
set isX2C=%errorlevel%

del %systemOEMPath%\OEMINFO.ini

if %isNEO%==0 (
echo Is NEO change to X2C
robocopy %oemInfoPath% %systemOEMPath% OEMINFO_X2C.ini
ren %systemOEMPath%\OEMINFO_X2C.ini OEMINFO.ini
)

if %isX2C%==0 (
echo Is X2C change to NEO
robocopy %oemInfoPath% %systemOEMPath% OEMINFO_NEO.ini
ren %systemOEMPath%\OEMINFO_NEO.ini OEMINFO.ini
)

shutdown -r -t 1