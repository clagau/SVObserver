//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSVIMStateClass
//* .File Name       : $Workfile:   SVSVIMStateClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   25 Aug 2014 02:41:14  $
//******************************************************************************

#if defined ( _MSC_VER ) && ( _MSC_VER >= 1000 )
#pragma once
#endif
#ifndef _INC_SVSVIMSTATECLASS_3A93E1A3036B_INCLUDED
#define _INC_SVSVIMSTATECLASS_3A93E1A3036B_INCLUDED

#define SV_STATE_UNKNOWN		0x00000000

#define SV_STATE_AVAILABLE		0x01000000
#define SV_STATE_READY			0x02000000
#define SV_STATE_RUNNING		0x04000000
#define SV_STATE_UNAVAILABLE	0x08000000

#define SV_STATE_CREATING		0x00100000
#define SV_STATE_LOADING		0x00200000
#define SV_STATE_SAVING			0x00400000
#define SV_STATE_CLOSING		0x00800000

#define SV_STATE_EDITING		0x00010000
#define SV_STATE_CANCELING		0x00020000
#define SV_STATE_INTERNAL_RUN	0x00040000

#define SV_STATE_START_PENDING	0x00001000
#define SV_STATE_STARTING		0x00002000
#define SV_STATE_STOP_PENDING	0x00004000
#define SV_STATE_STOPING		0x00008000

#define SV_STATE_TEST			0x00000100
#define SV_STATE_REGRESSION		0x00000200
#define SV_STATE_EDIT			0x00000400
#define SV_STATE_EDIT_MOVE		0x00000800

#define SV_STATE_SECURED		0x00000010
#define SV_STATE_RAID_FAILURE	0x00000020

#define SV_STATE_MODIFIED		0x00000001

//This class manages the state variable and uses a lock to 
//
//This class supports the following states:
//AVAILABLE - SVIM waiting for a configuration.
//READY - SVIM has a valid configuration loaded.
//RUNNING - SVIM on-line (processing inspection documents).
//UNAVAILABLE - SVIM is busy or cannot respond to a command 
//at this time.
class SVSVIMStateClass
{
public:
	//This operation adds a substate to the existing state 
	//value.  The value passed in as a parameter is ORed to 
	//the existing value.
	static bool AddState( DWORD dwState );

	//This operation removes a particular substate from the 
	//existing state value.  This process takes the substate 
	//value and inverts it and ANDs it to the existing state 
	//value.
	static bool RemoveState( DWORD dwState );

	//This operation checks the parameter state value against 
	//the interal value and outputs in the result parameter 
	//whether there is at least one bit (state) matching.
	static bool CheckState( DWORD dwState );

private:
	//************************************
	// Method:    setEnvironmentParameters
	// Description:  Set all Environment.Mode parameter.
	// Returns:   void
	//************************************
	static void setEnvironmentParameters();

	//************************************
	// Method:    setEnvironmentParameter
	// Description:  Set a Environment parameter
	// Parameter: DWORD dwState State to check
	// Parameter: const TCHAR * name Name of the Environment parameter
	// Returns:   void
	//************************************
	static void setEnvironmentParameter( DWORD dwState, const TCHAR * name );


	//This constructor does nothing. 
	SVSVIMStateClass();

	//This destructor does nothing. 
	virtual ~SVSVIMStateClass();

	//This attribute contain the SVIM state value.
	static long m_SVIMState;
};

#endif /* _INC_SVSVIMSTATECLASS_3A93E1A3036B_INCLUDED */

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVSVIMStateClass.h_v  $
 * 
 *    Rev 1.1   25 Aug 2014 02:41:14   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  934
 * SCR Title:  Add Remote Access to Environment.Mode Parameters
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add methods setEnvironmentParameter(s)
 * set environment-mode parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:16:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   09 Jul 2012 14:17:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Jun 2012 17:28:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   16 Oct 2008 15:30:56   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed CheckState to a static function
 * Moved Log to end of file
 * Removed ModelIds
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   05 Jan 2006 09:57:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  530
 * SCR Title:  Allow SVObserver to monitor Intel SATA RAID status
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new define for RAID Failure state.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Jun 2005 13:12:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added new states for Edit and Edit move
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Jul 2003 15:09:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  377
 * SCR Title:  The Color SVIM runs much slower in 4.x versions
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Removed old reference to SVStateClass' files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Jul 2003 12:19:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  377
 * SCR Title:  The Color SVIM runs much slower in 4.x versions
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Consolidated SVStateClass into SVSVIMStateClass and fixed SVSVIMStateClass to no longer use a mutex for locking.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 15:43:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Apr 2003 17:38:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  342
 * SCR Title:  Configuration report printout changes after going online.
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new state SV_STATE_INTERNAL_RUN. This is used to force all objects to run during the loading process. Otherwise some objects may be conditionally not executed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jun 2001 10:06:16   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Removed #define SV_STATE_STOPING and added #define SV_STATE_STOP_PENDING
 * & #define SV_STATE_STOPING.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 May 2001 16:05:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  197
 * SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed SVObserver state code by adding this new class
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
