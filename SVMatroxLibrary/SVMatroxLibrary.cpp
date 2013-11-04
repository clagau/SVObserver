// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxLibrary
// * .File Name       : $Workfile:   SVMatroxLibrary.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:21:40  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxLibrary.h"

long SVMatroxCheckAndLog( SVMatroxApplicationInterface::SVStatusCode p_svCode, TCHAR* p_szFile, long p_lLine )
{
	if( p_svCode != SVMEE_STATUS_OK )
	{
		SVMatroxStatusInformation l_svInfo;
		SVMatroxApplicationInterface::GetLastStatus(l_svInfo);

		SVMatroxString l_strMessage( l_svInfo.m_StatusString.c_str() );

//		SVException e;
//		e.SetException (SVMSG_SVO_36_MIL_ERROR, _T(__DATE__), _T(__TIME__), l_strMessage, p_szFile, p_lLine, _T(__TIMESTAMP__) );
//		e.LogException( l_strMessage );
	}

	return p_svCode;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxLibrary.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:21:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Sep 2011 14:29:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Martox Library to include a new resource management system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/