//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSVIMStateClass
//* .File Name       : $Workfile:   SVSVIMStateClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   09 Dec 2014 10:12:44  $
//******************************************************************************

#include "stdafx.h"
#include <intrin.h>
#include "SVSVIMStateClass.h"
#include "SVVisionProcessorHelper.h"
#include "EnvironmentObject.h"
#include "AutoSaver.h"

#pragma intrinsic(_InterlockedAnd)
#pragma intrinsic(_InterlockedOr)

long SVSVIMStateClass::m_SVIMState = SV_STATE_AVAILABLE;

SVSVIMStateClass::SVSVIMStateClass()
{
}

SVSVIMStateClass::~SVSVIMStateClass()
{
}

bool SVSVIMStateClass::AddState( DWORD dwState )
{
	//DWORD dwTempState;

	//dwTempState = m_SVIMState | dwState;
	//::InterlockedExchange( &m_SVIMState, dwTempState );

	::_InterlockedOr( &m_SVIMState, dwState );

	if( dwState & SV_STATE_MODIFIED )
	{
		SVVisionProcessorHelper::Instance().SetLastModifiedTime();
		AutoSaver::Instance().SetAutoSaveRequired(true);
	}
	svModeEnum mode = GetMode();
	CheckModeNotify(mode);
	setEnvironmentParameters(mode);
	return true;
}

bool SVSVIMStateClass::RemoveState( DWORD dwState )
{
	//DWORD dwTempState;

	//dwTempState = m_SVIMState & ~dwState;
	//::InterlockedExchange( &m_SVIMState, dwTempState );

	::_InterlockedAnd( &m_SVIMState, ~dwState );
	svModeEnum mode = GetMode();
	CheckModeNotify(mode);
	setEnvironmentParameters(mode);

	return true;
}

bool SVSVIMStateClass::CheckState( DWORD dwState )
{
	bool l_Status = ( dwState & m_SVIMState ) != 0;

	return l_Status;
}

svModeEnum SVSVIMStateClass::GetMode()
{
	svModeEnum retVal = SVIM_MODE_UNKNOWN;

	// Pending conditions...
	if( SVSVIMStateClass::CheckState( SV_STATE_START_PENDING |
		SV_STATE_STARTING |
		SV_STATE_STOP_PENDING |
		SV_STATE_STOPING |
		SV_STATE_CREATING |
		SV_STATE_LOADING |
		SV_STATE_SAVING |
		SV_STATE_CLOSING |
		SV_STATE_EDITING ) )
	{
		retVal = SVIM_MODE_CHANGING;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
	{
		retVal = SVIM_MODE_EDIT;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		retVal = SVIM_MODE_ONLINE;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
	{
		retVal = SVIM_MODE_REGRESSION;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
	{
		retVal = SVIM_MODE_TEST;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_READY ) &&
		!SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
	{
		retVal = SVIM_MODE_OFFLINE;
	}
	else
	{
		retVal = SVIM_MODE_UNKNOWN;
	}

	return retVal;
}

void SVSVIMStateClass::CheckModeNotify(svModeEnum mode)
{
	static svModeEnum currentMode = SVIM_MODE_UNKNOWN;
	if (mode != currentMode)
	{
		currentMode = mode;
		SVVisionProcessorHelper::Instance().FireModeChanged(mode);
	}
}

void SVSVIMStateClass::setEnvironmentParameters(svModeEnum mode)
{
	long modeValue = static_cast<long>(mode);

	EnvironmentObject::setEnvironmentValue( ::EnvironmentModeValue, modeValue );
	EnvironmentObject::setEnvironmentValue( ::EnvironmentModeIsRun, static_cast< BOOL >( SVIM_MODE_ONLINE == mode ) );
	EnvironmentObject::setEnvironmentValue( ::EnvironmentModeIsStop, static_cast< BOOL >( SVIM_MODE_OFFLINE == mode ) );
	EnvironmentObject::setEnvironmentValue( ::EnvironmentModeIsRegressionTest, static_cast< BOOL >( SVIM_MODE_REGRESSION == mode ) );
	EnvironmentObject::setEnvironmentValue( ::EnvironmentModeIsTest, static_cast< BOOL >( SVIM_MODE_TEST == mode ) );
	EnvironmentObject::setEnvironmentValue( ::EnvironmentModeIsEdit, static_cast< BOOL >( SVIM_MODE_EDIT == mode ) );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVSVIMStateClass.cpp_v  $
 * 
 *    Rev 1.5   09 Dec 2014 10:12:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Remove edit move state and supporting functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Sep 2014 10:16:54   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  934
 * SCR Title:  Add Remote Access to Environment.Mode Parameters
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Fixed type for Environment.Mode variables.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Aug 2014 15:44:02   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  934
 * SCR Title:  Add Remote Access to Environment.Mode Parameters
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added static method GetMode.  Removed method setEnvironmentParameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   25 Aug 2014 02:41:14   mziegler
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
 *    Rev 1.1   18 Jun 2013 19:23:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   31 May 2013 08:10:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  839
 * SCR Title:  Add Last Modified Notification to Remote Socket Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated add state method to notify vision processor helper when modified bit is set.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:15:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Jul 2012 14:17:40   jspila
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
 *    Rev 1.3   16 Oct 2008 15:31:28   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Moved Log to end of file
 * Removed ModelIds
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Jul 2003 12:19:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  377
 * SCR Title:  The Color SVIM runs much slower in 4.x versions
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Consolidated SVStateClass into SVSVIMStateClass and fixed SVSVIMStateClass to no longer use a mutex for locking.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 15:43:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 May 2001 16:05:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  197
 * SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed SVObserver state code by adding this new class
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/