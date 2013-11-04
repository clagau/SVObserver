// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIODDELoadLibraryClass
// * .File Name       : $Workfile:   SVIODDELoadLibraryClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:43:50  $
// ******************************************************************************

#ifndef SVIODDELOADLIBRARYCLASS_H
#define SVIODDELOADLIBRARYCLASS_H

#include "SVOMFCLibrary/SVCallbackStruct.h"

struct SVDDEItemStruct;

typedef BOOL (WINAPI *SVDDESetServerPtr)( HWND, BSTR, BSTR );
typedef BOOL (WINAPI *SVDDESendDataPtr)( SVDDEItemStruct * );
typedef BOOL (WINAPI *SVDDEReceiveDataPtr)( SVDDEItemStruct * );

class SVIODDELoadLibraryClass  
{
public:
	SVIODDELoadLibraryClass();
	~SVIODDELoadLibraryClass();

	HRESULT Open( LPCTSTR p_szLibrary );
	HRESULT Close();
	
	HRESULT SetDDEServer( HWND HWnd, BSTR TStrServer, BSTR TStrTopic );
	HRESULT SendDDEData( SVDDEItemStruct *p_pItem );
	HRESULT ReceiveDDEData( SVDDEItemStruct *p_pItem );

private:
	HMODULE m_hmHandle;

	SVDDESetServerPtr m_psvSetServer;
	SVDDESendDataPtr m_psvSendData;
	SVDDEReceiveDataPtr m_psvReceiveData;

};

#include "SVIODDELoadLibraryClass.inl"

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIOLibrary\SVIODDELoadLibraryClass.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:43:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Dec 2009 12:17:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Aug 2003 15:17:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated structure definition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 13:20:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added I/O objects to new I/O configuration interface library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
