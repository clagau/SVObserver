eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "Add option color"
Regedit /s SVAddOptionColor.reg
mshta "javascript:var sh=new ActiveXObject('WScript.Shell'); sh.Popup('Re-Start SVObserver to Activate color', 10, 'Action Required', 64);close()"