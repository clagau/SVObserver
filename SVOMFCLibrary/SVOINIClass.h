// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOINIClass
// * .File Name       : $Workfile:   SVOINIClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:08:22  $
// ******************************************************************************

#ifndef _SVOINICLASS_H
#define _SVOINICLASS_H

#if _MSC_VER > 1000
#pragma once
#endif

#include <string>

class SVOINIClass  
{
public:
	SVOINIClass();
	~SVOINIClass();

	HRESULT SetFile( LPCTSTR p_szFile );

	std::string GetValueString( LPCTSTR p_szSection,
	                  LPCTSTR p_szKey,
	                  LPCTSTR p_szDefault,
	                  LPCTSTR p_szFileName = NULL );

	HRESULT GetValue( LPCTSTR p_szSection,
	                  LPCTSTR p_szKey,
	                  LPCTSTR p_szDefault,
	                  BSTR*   p_pbstrValue,
	                  LPCTSTR p_szFileName = NULL );

	HRESULT GetValue( LPCTSTR p_szSection,
	                  LPCTSTR p_szKey,
	                  int     p_iDefault,
	                  int*    p_piValue,
	                  LPCTSTR p_szFileName = NULL );

	int GetValueInt( LPCTSTR p_szSection,
	                     LPCTSTR p_szKey,
	                     int     p_iDefault,
	                     LPCTSTR p_szFileName = NULL );

	HRESULT SetValue( LPCTSTR p_szSection,
	                  LPCTSTR p_szKey,
	                  LPCTSTR p_szValue,
	                  LPCTSTR p_szFileName = NULL );

	HRESULT SetValue( LPCTSTR p_szSection,
	                  LPCTSTR p_szKey,
	                  int     p_iValue,
	                  LPCTSTR p_szFileName = NULL );
protected:
	LPTSTR	m_tszFile;
};

#include "SVOINIClass.inl"

#endif // _SVOINICLASS_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVOINIClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:08:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Jul 2005 14:11:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added GetValueString
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Jul 2004 11:27:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetValueInt
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   31 Jul 2003 08:30:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for int type
 * added SetFile
 * made file name optional for gets and sets ( to work with SetFile)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2003 13:13:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322, 320
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   New files needed to handle callback functionality and parameter functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/