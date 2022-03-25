if "%OS%"=="" goto NOTNT
if not "%OS%"=="Windows_NT" goto NOTNT
"%1%" /RegServer
echo regsvr32 exec. time > "%2%\regsvr32.trg"
echo Server registration done!
goto end
:NOTNT
echo Warning : Cannot register Unicode EXE on Windows 95
:end
