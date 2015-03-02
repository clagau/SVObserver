@echo off
call vcvars32.bat
echo Deleting %1 file
del %1
echo Generating %1 file
echo // Help File Resources for SVObserver Application >>%1
echo. >>%1
echo // Identifiers (ID_* and IDM_*) >>%1
makehm /h ID_,HID_,0x10000 IDM_,HIDM_,0x10000 ..\SVOResource\resource.h >>%1
echo. >>%1
echo // Error Resource (IDP_*) >>%1
makehm /h IDP_,HIDP_,0x30000 ..\SVOResource\resource.h >>%1
echo. >>%1
echo // Menus and Toolbars (IDR_*) >>%1
makehm /h IDR_,HIDR_,0x20000 ..\SVOResource\resource.h >>%1
echo. >>%1
echo // Dialogs (IDD_*) >>%1
makehm /h IDD_,HIDD_,0x20000 ..\SVOResource\resource.h >>%1
echo. >>%1
echo // SVSecurity Dialogs (IDD_*) >>%1
makehm /h IDD_,HIDD_,0x60000 ..\SVSecurity\resource.h >>%1
echo. >>%1
echo // SVSecurity Controls (IDC_*) >>%1
makehm /h IDC_,HIDC_,0x70000 ..\SVSecurity\resource.h >>%1
echo. >>%1
echo // System Control ID's >>%1
echo #define HID_FILE_NEW                            0x0001E100 >>%1
echo #define HID_FILE_PRINT_SETUP                    0x0001E106 >>%1
echo #define HID_FILE_MRU_FIRST                      0x0001E110 >>%1
echo #define HID_FILE_MRU_FILE1                      0x0001E110 >>%1
echo #define HID_FILE_MRU_FILE2                      0x0001E111 >>%1
echo #define HID_FILE_MRU_FILE3                      0x0001E112 >>%1
echo #define HID_FILE_MRU_FILE4                      0x0001E113 >>%1
echo #define HID_FILE_MRU_FILE5                      0x0001E114 >>%1
echo #define HID_FILE_MRU_FILE6                      0x0001E115 >>%1
echo #define HID_FILE_MRU_FILE7                      0x0001E116 >>%1
echo #define HID_APP_EXIT                            0x0001E141 >>%1
echo #define HID_APP_ABOUT                           0x0001E140 >>%1
echo #define HID_HELP_FINDER                         0x0001E143 >>%1
echo #define HID_WINDOW_ARRANGE               	     0x0001E131 >>%1
echo #define HID_WINDOW_CASCADE              	     0x0001E132 >>%1
echo #define HID_WINDOW_TILE_HORZ	             0x0001E133 >>%1
echo #define HID_WINDOW_TILE_VERT 	             0x0001E134 >>%1
echo. >>%1
echo // Toolset Draw Id's >>%1
echo #define HID_TOOLSET_DRAW_CURRENT		     0x000180E9 >>%1
echo #define HID_TOOLSET_DRAW_WARNED		     0x000180EA >>%1
echo #define HID_TOOLSET_DRAW_FAILED		     0x000180EB >>%1	
echo #define HID_TOOLSET_DRAW_PASSED		     0x000180EC >>%1
echo #define HID_TOOLSET_DRAW_NOTOOLS		     0x000180ED >>%1
echo #define HID_TOOLSET_DRAW_TOOLCONTROL	     0x000180EE >>%1
echo Created %1 file
