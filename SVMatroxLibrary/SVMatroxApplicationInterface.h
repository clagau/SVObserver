// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxApplicationInterface
// * .File Name       : $Workfile:   SVMatroxApplicationInterface.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 11:08:08  $
// ******************************************************************************

#ifndef SV_MATROX_APPLICATION_INTERFACE_H
#define SV_MATROX_APPLICATION_INTERFACE_H

#include "SVMatroxStatusInformation.h"
#include "SVMatroxTypedefs.h"

/**
@SVObjectName Matrox Application Interface

@SVObjectOverview This class provides an interface for the Matrox Application operations.

@SVObjectOperations The GetLastStatus function returns a SVMatroxStatusInformation with the most recent status.
The GetFirstError returns a SVMatroxStatusInformation which is filled with information of the oldest error information since this function was last read.

*/
class SVMatroxApplicationInterface
{
public:
	typedef SVMatroxStatusInformation::SVStatusCode SVStatusCode;

	static void Startup();
	static void Shutdown();

	static SVStatusCode GetLastStatus();
	static SVStatusCode GetLastStatus( SVMatroxStatusInformation& p_rStatus );

	static SVStatusCode GetFirstError();
	static SVStatusCode GetFirstError( SVMatroxStatusInformation& p_rStatus );

	static void Log( SVMatroxStatusInformation& p_rStatusInfo );
	static void LogMatroxException();
	static SVStatusCode GetSystemCount( long& p_lCount );
	static SVStatusCode GetSystemName( SVMatroxInt p_lSystemNumber, SVMatroxString& p_rSystemName );
	static HRESULT SVMatroxIntToHRESULT( SVMatroxIdentifier p_Int );

protected:
#ifdef _WIN64
   static SVMatroxInt _stdcall SVMatroxHookHandler( SVMatroxInt HookType, SVMatroxIdentifier EventId, void* UserDataPtr );
#else
	static long _stdcall SVMatroxHookHandler( long HookType, SVMatroxIdentifier EventId, void* UserDataPtr );
#endif

private:
	static void LocalInitialize();
	static void LocalClear();

	SVMatroxApplicationInterface();
	SVMatroxApplicationInterface( const SVMatroxApplicationInterface& p_rObject );
	const SVMatroxApplicationInterface& operator=( const SVMatroxApplicationInterface& p_rObject );
};

#endif // SV_MATROX_APPLICATION_INTERFACE_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxApplicationInterface.h_v  $
 * 
 *    Rev 1.1   01 Oct 2013 11:08:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 14:56:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Sep 2011 14:23:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Martox Library to include a new resource management system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jun 2009 15:48:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetSytemCount method
 * Added GetSystemName method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Nov 2008 09:58:48   jbrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  641
 * SCR Title:  BoundsChecker results
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVMatroxApplicationInterface::Log () prototype was changed  to not do a copy on the incoming error structure.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 15:01:28   jbrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  641
 * SCR Title:  BoundsChecker results
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Changed Log prototype to more closely match SVResearch standards within SVMatroxApplicationInterface.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:17:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
