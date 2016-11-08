eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "Remove option color"
Regedit /s SVRemoveOptioncolor.reg
mshta "javascript:var sh=new ActiveXObject('WScript.Shell'); sh.Popup('Re-Start SVObserver to Deactivate color', 10, 'Action Required', 64);close()"