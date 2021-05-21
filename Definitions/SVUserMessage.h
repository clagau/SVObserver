//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUserMessage
//* .File Name       : $Workfile:   SVUserMessage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   12 Dec 2014 13:13:52  $
//******************************************************************************

#pragma once

#define WM_REGRESSION_TEST_SET_FILENAMES			WM_APP + 15
#define WM_REGRESSION_TEST_SET_PLAYPAUSE			WM_APP + 16
#define WM_REGRESSION_TEST_SET_PREVIOUS				WM_APP + 17
#define WM_REGRESSION_TEST_CLOSE_REGRESSION			WM_APP + 18
#define WM_REGRESSION_TEST_COMPLETE					WM_APP + 19

#define SV_IPDOC_UPDATE_ALL_DATA    WM_APP + 32
#define SV_IPDOC_UPDATE_NEXT_VIEW   WM_APP + 33

// Used to start and monitor Object Script parsing
#define SV_UPDATE_PROGRESS_TEXT						WM_USER + 6
#define SV_UPDATE_PROGRESS							WM_USER + 8
#define SV_END_PROGRESS_DIALOG						WM_USER + 9

// Send update to tool set tab view to set list control selection.
#define SV_SET_TOOL_SELECTED_IN_TOOL_VIEW           WM_USER + 10

// Send to a dialog to let call protected refresh() method.
#define SV_REFRESH_DIALOG							WM_USER + 11

// Send update to MainFrame to set enable/disable menuitems and/or hide additional windows
#define SV_LOGGED_ON_USER_CHANGED				    WM_USER + 12

// 	Message to call OnRunMostRecentMRU()
#define SV_AUTO_RUN_LAST_MRU                        WM_USER + 14

#define SV_MDICHILDFRAME_UPDATE_ALL_DATA    WM_USER + 32
#define SV_SET_MODE WM_USER + 33
#define SV_LOAD_PACKED_CONFIGURATION WM_USER + 34
#define SV_REFRESH_STATUS_BAR WM_USER + 35
#define SV_SHUTDOWN WM_USER + 36
#define SV_REGISTER_MONITOR_LIST WM_USER + 37
#define SV_ADD_FILE_TO_CONFIG WM_USER + 38
#define SV_SAVE_CONFIG WM_USER + 39
#define SV_REMOVE_PAGES_FOR_TESTED_DLL WM_USER + 40
#define SV_ADD_PAGES_FOR_TESTED_DLL WM_USER + 41
#define SV_UPDATE_IPDOC_VIEWS WM_USER + 42


