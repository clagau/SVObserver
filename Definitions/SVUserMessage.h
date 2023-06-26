//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUserMessage
//* .File Name       : $Workfile:   SVUserMessage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   12 Dec 2014 13:13:52  $
//******************************************************************************

#pragma once

constexpr UINT WM_REGRESSION_TEST_SET_FILENAMES = WM_APP + 15;
constexpr UINT WM_REGRESSION_TEST_SET_PLAYPAUSE = WM_APP + 16;
constexpr UINT WM_REGRESSION_TEST_SET_PREVIOUS = WM_APP + 17;
constexpr UINT WM_REGRESSION_TEST_CLOSE_REGRESSION = WM_APP + 18;
constexpr UINT WM_REGRESSION_TEST_COMPLETE = WM_APP + 19;

constexpr UINT SV_IPDOC_UPDATE_ALL_DATA = WM_APP + 32;
constexpr UINT SV_IPDOC_UPDATE_NEXT_VIEW = WM_APP + 33;

// Used to start and monitor Object Script parsing
constexpr UINT SV_UPDATE_PROGRESS_TEXT = WM_USER + 6;
constexpr UINT SV_UPDATE_PROGRESS = WM_USER + 8;
constexpr UINT SV_END_PROGRESS_DIALOG = WM_USER + 9;

// Send update to tool set tab view to set list control selection.
constexpr UINT SV_SET_TOOL_SELECTED_IN_TOOL_VIEW = WM_USER + 10;

// Send to a dialog to let call protected refresh() method.
constexpr UINT SV_REFRESH_DIALOG = WM_USER + 11;

// Send update to MainFrame to set enable/disable menuitems and/or hide additional windows
constexpr UINT SV_LOGGED_ON_USER_CHANGED = WM_USER + 12;

// 	Message to call OnRunMostRecentMRU()
constexpr UINT SV_AUTO_RUN_LAST_MRU = WM_USER + 14;

constexpr UINT SV_MDICHILDFRAME_UPDATE_ALL_DATA = WM_USER + 32;
constexpr UINT SV_SET_MODE = WM_USER + 33;
constexpr UINT SV_LOAD_PACKED_CONFIGURATION = WM_USER + 34;
constexpr UINT SV_REFRESH_STATUS_BAR = WM_USER + 35;
constexpr UINT SV_SHUTDOWN = WM_USER + 36;
constexpr UINT SV_REGISTER_MONITOR_LIST = WM_USER + 37;
constexpr UINT SV_ADD_FILE_TO_CONFIG = WM_USER + 38;
constexpr UINT SV_SAVE_CONFIG = WM_USER + 39;
constexpr UINT SV_ADAPT_TO_UNTESTED_DLL = WM_USER + 40;
constexpr UINT SV_ADAPT_TO_TESTED_DLL = WM_USER + 41;
constexpr UINT SV_SELECT_INPUT_VALUE_PAGE = WM_USER + 42;
constexpr UINT SV_UPDATE_IPDOC_VIEWS = WM_USER + 43;
constexpr UINT SV_CLOSE_CONFIG = WM_USER + 44;
constexpr UINT SV_SET_IPDOC_EDIT_TOOLID = WM_USER + 45;


