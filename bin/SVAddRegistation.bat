eventcreate /ID 13 /L APPLICATION /T INFORMATION  /SO SVObserver.exe /D "Add registration"
regsvr32 c:\svobserver\bin\SVPictureDisplay.ocx
regsvr32 c:\svobserver\bin\SVMaskEditor.ocx
regsvr32 %windir%\system32\atl.dll
regsvr32 c:\svobserver\leadtools\ltocx13n.ocx
c:\svobserver\bin\svobserver.exe -/RegServer
