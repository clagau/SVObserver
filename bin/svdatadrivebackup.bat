eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "SVDataDriveBackup started"
Robocopy d:\ f:\SVIMDBUp\ /dcopy:T /mir /r:3 /w:20 /xa:H
eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "SVDataDriveBackup completed"