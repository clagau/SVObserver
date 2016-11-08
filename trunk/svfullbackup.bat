eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "SVFullBackup started"
wbadmin start backup -backupTarget:f: -allCritical -quiet 
eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "SVFullBackup completed"