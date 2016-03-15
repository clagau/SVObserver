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

