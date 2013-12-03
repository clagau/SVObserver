// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxAppManager
// * .File Name       : $Workfile:   SVMatroxAppManager.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:30:40  $
// ******************************************************************************

#ifndef __SVMATROXAPPMANAGER_H__
#define __SVMATROXAPPMANAGER_H__

#include <mil.h>
#include "mildetect.h"
#include <static_check.h>

#include <set>

FILE_SCOPE_STATIC_CHECK( SV_CURRENT_MIL_VERSION >= 0x0750, Need_to_compile_with_MIL_75_or_greater );

class SVMatroxAppManager
{
public:
	typedef std::set< long > SVMatroxLongSet;

	SVMatroxAppManager();
	~SVMatroxAppManager();

	MIL_ID GetApp();

	static long GetStatusCode( TCHAR* p_pszSourceFile, long p_SourceLine );
	static long GetStatusCode( TCHAR* p_pszSourceFile, long p_SourceLine, 
		const SVMatroxLongSet& p_rExcludeStatusCodesFromLogging );

	static CString GetStatusString();

	static long GetError();
	static CString GetErrorString();
	static CString GetErrorStringFull();
	static void SetErrorDisplay( bool bDisplay );
private:
	static bool m_bDisplayErrors;
	static int m_nRefCount;
	static MIL_ID m_MilApp;
	
	static MAPPHOOKFCTPTR m_pfnMilErrorHandler;
	static void*          m_pHandlerUserPtr;

	static long MFTYPE HandleError(long lHookType, MIL_ID EventId, void MPTYPE* pUserData);
};

#endif	//#ifndef __SVMATROXAPPMANAGER_H__

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMatroxAppManager.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:30:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Feb 2007 16:38:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Dec 2004 08:30:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  467
 * SCR Title:  Fix missing bar code problem
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added compile-time check for MIL 7.5 or greater
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Oct 2003 10:53:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetError, GetErrorString, GetErrorStringFull
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 11:21:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added files to support Matrox acquisition devices
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/