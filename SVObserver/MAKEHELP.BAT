@echo off
call vcvars32.bat
echo Deleting %1 file
del %1
echo Generating %1 file
echo // Help File Resources for SVObserver Application >>%1
echo. >>%1
echo // Identifiers (ID_* and IDM_*) >>%1
makehm /h ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>%1
echo. >>%1
echo // Error Resource (IDP_*) >>%1
makehm /h IDP_,HIDP_,0x30000 resource.h >>%1
echo. >>%1
echo // Menus and Toolbars (IDR_*) >>%1
makehm /h IDR_,HIDR_,0x20000 resource.h >>%1
echo. >>%1
echo // Dialogs (IDD_*) >>%1
makehm /h IDD_,HIDD_,0x20000 resource.h >>%1
echo. >>%1
echo // SVSecurity Dialogs (IDD_*) >>%1
makehm /h IDD_,HIDD_,0x60000 ..\SVSecurity\resource.h >>%1
echo. >>%1
echo // SVSecurity Controls (IDC_*) >>%1
makehm /h IDC_,HIDC_,0x70000 ..\SVSecurity\resource.h >>%1
echo Created %1 file
