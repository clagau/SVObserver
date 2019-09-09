
for /f "delims=[] tokens=2" %%a in ('ping -4 -n 1 %ComputerName% ^| findstr [') do set NetworkIP=%%a
echo %NetworkIP%>D:\Utilities\BGInfo\ip.txt

D:
cd \Utilities\BGInfo
bginfo64.exe D:\Utilities\BGInfo\bginfo.bgi /Timer:0 /NOLICPROMPT
