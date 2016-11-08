@echo off
echo Synchronizing system time, please be patient for 60 seconds!
ping -n 60 127.0.0.1 >nul
net time \\192.168.10.9 /set /yes
exit