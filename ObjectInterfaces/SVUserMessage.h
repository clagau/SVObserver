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
#define WM_REGRESSION_MODE_CHANGED					WM_APP + 20

#define SV_IPDOC_UPDATE_ALL_DATA    WM_APP + 32
#define SV_IPDOC_UPDATE_NEXT_VIEW   WM_APP + 33

// Used to start and monitor Object Script parsing
#define SV_UPDATE_PROGRESS_TEXT						WM_USER + 6
#define SV_PARSE_OBJECT_SCRIPT_END					WM_USER + 7
#define SV_UPDATE_PROGRESS							WM_USER + 8
#define SV_END_PROGRESS_DIALOG						WM_USER + 9

// Send update to tool set tab view to set list control selection.
#define SV_SET_TOOL_SELECTED_IN_TOOL_VIEW           WM_USER + 10

// Send to a dialog to let call protected refresh() method.
#define SV_REFRESH_DIALOG							WM_USER + 11

// Send update to MainFrame to set enable/disable menuitems and/or hide additional windows
#define SV_LOGGED_ON_USER_CHANGED				    WM_USER + 12

// Send to MainFrame after all objects for all IPD are created
#define SV_PARSE_OBJECT_CREATE_DONE					WM_USER + 13

// 	Message to call OnRunMostRecentMRU()
#define SV_AUTO_RUN_LAST_MRU                        WM_USER + 14

#define SV_IOVIEW_HIDE_TAB   WM_USER + 24

#define SV_MDICHILDFRAME_UPDATE_ALL_DATA    WM_USER + 32
#define SV_SET_MODE WM_USER + 33
#define SV_LOAD_CONFIGURATION WM_USER + 34
#define SV_REFRESH_STATUS_BAR WM_USER + 35
#define SV_SHUTDOWN WM_USER + 36

#define SV_REGISTER_MONITOR_LIST WM_USER + 37

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVUserMessage.h_v  $
 * 
 *    Rev 1.4   12 Dec 2014 13:13:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  918
 * SCR Title:  Implement Method RegisterMonitorList for RemoteControl (SVO-369)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   added new define SV_REGISTER_MONITOR_LIST
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Jun 2014 07:28:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  905
 * SCR Title:  Implement Shutdown Command thru Remote Control
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added constants to support remote shutdown command.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Aug 2013 06:15:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added message to refresh status bar.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:12:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   16 Apr 2013 15:35:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:56:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   11 Feb 2013 14:22:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new framework for the remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   04 Sep 2012 15:50:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new progress dialog functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   02 Jul 2012 17:52:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   07 Oct 2011 14:22:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated MDI Child Window and IP Document to fix threading problems by moving the data through a lockless queue and notifying the IP Document via the MDI Child Window with a windows message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   08 Dec 2010 13:48:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   14 Nov 2008 14:46:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added IO Tab views
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   10 Aug 2005 12:45:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   15 Jul 2005 11:34:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new user messages for the regression test
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   21 Jun 2005 13:14:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added new message for auto run last mru
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   22 Apr 2003 17:18:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   20 Nov 2002 14:54:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   took out SV_UPDATE_IO_VALUE
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   09 Aug 2001 10:35:34   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  210
 * SCR Title:  Fix of shared data, multiple IPD's, same digitizer
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added SV_PARSE_OBJECT_CREATE_DONE message definition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   05 May 2000 13:44:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  134
 * SCR Title:  Restricted Operator Security level
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SV_LOGGED_ON_USER_CHANGED message define
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   23 Mar 2000 16:51:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added new WM_REFRESH_DIALOG (used for LUT Dialog
 * page to be informed about necessary refresh)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   Feb 01 2000 10:40:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  90
 * SCR Title:  Save  Operator Tool Move Priviledge in Configuration
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added new SVObserver message for tool move feature.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Jan 03 2000 14:45:56   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  77
 * SCR Title:  Automatic configuration load and go online fails.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Remove SV_AUTO_RUN_LAST_CONFIGURATION message definition. Not needed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 14:02:46   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Nov 1999 11:30:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Windows user message for Object Script parsing end
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   03 Nov 1999 17:47:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added User Messages for Progress Dialog and Object
 * Script Parsing begin notification
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Aug 23 1999 11:33:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  22
 * SCR Title:  Port Multiple Position Queue code from program version 2.4.2
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Added User Messages for AutRun Last Configuration and Update IO Value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
//** EOF **