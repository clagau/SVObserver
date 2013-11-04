// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOTriggerLoadLibraryClass
// * .File Name       : $Workfile:   SVIOTriggerLoadLibraryClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:48:18  $
// ******************************************************************************

#ifndef SVIOTRIGGERLOADLIBRARYCLASS_H
#define SVIOTRIGGERLOADLIBRARYCLASS_H

#include "SVOMFCLibrary/SVCallbackStruct.h"

typedef HRESULT (WINAPI *SVCreatePtr)( void );
typedef HRESULT (WINAPI *SVDestroyPtr)( void );
typedef HRESULT (WINAPI *SVTriggerGetCountPtr)( unsigned long * );
typedef HRESULT (WINAPI *SVTriggerGetHandlePtr)( unsigned long *, unsigned long );
typedef HRESULT (WINAPI *SVTriggerGetNamePtr)( unsigned long, BSTR * );
typedef HRESULT (WINAPI *SVTriggerRegisterPtr)( unsigned long, SVCallbackPtr, void *, void * );
typedef HRESULT (WINAPI *SVTriggerUnregisterPtr)( unsigned long, SVCallbackPtr, void *, void * );
typedef HRESULT (WINAPI *SVTriggerUnregisterAllPtr)( unsigned long );
typedef HRESULT (WINAPI *SVTriggerStartPtr)( unsigned long );
typedef HRESULT (WINAPI *SVTriggerStopPtr)( unsigned long );
typedef HRESULT (WINAPI *SVTriggerGetParameterCountPtr)( unsigned long, unsigned long * );
typedef HRESULT (WINAPI *SVTriggerGetParameterNamePtr)( unsigned long, unsigned long, BSTR * );
typedef HRESULT (WINAPI *SVTriggerGetParameterValuePtr)( unsigned long, unsigned long, VARIANT * );
typedef HRESULT (WINAPI *SVTriggerSetParameterValuePtr)( unsigned long, unsigned long, VARIANT * );

class SVIOTriggerLoadLibraryClass  
{
public:
	SVIOTriggerLoadLibraryClass();
	~SVIOTriggerLoadLibraryClass();

	HRESULT Open( LPCTSTR p_szLibrary );
	HRESULT Close();
	
	HRESULT GetCount( unsigned long *p_pulCount );
	HRESULT GetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex );
	HRESULT GetName( unsigned long p_ulHandle, BSTR *p_pbstrName );
	HRESULT Register( unsigned long p_ulHandle, SVCallbackStruct &p_rCallback );
	HRESULT Unregister( unsigned long p_ulHandle, SVCallbackStruct &p_rCallback );
	HRESULT UnregisterAll( unsigned long p_ulHandle );
	HRESULT Start( unsigned long p_ulHandle );
	HRESULT Stop( unsigned long p_ulHandle );
	HRESULT GetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount );
	HRESULT GetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName );
	HRESULT GetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );
	HRESULT SetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );

private:
	HMODULE m_hmHandle;

	SVCreatePtr m_psvCreate;
	SVDestroyPtr m_psvDestroy;
	SVTriggerGetCountPtr m_psvGetCount;
	SVTriggerGetHandlePtr m_psvGetHandle;
	SVTriggerGetNamePtr m_psvGetName;
	SVTriggerRegisterPtr m_psvRegister;
	SVTriggerUnregisterPtr m_psvUnregister;
	SVTriggerUnregisterAllPtr m_psvUnregisterAll;
	SVTriggerStartPtr m_psvStart;
	SVTriggerStopPtr m_psvStop;
	SVTriggerGetParameterCountPtr m_psvGetParameterCount;
	SVTriggerGetParameterNamePtr m_psvGetParameterName;
	SVTriggerGetParameterValuePtr m_psvGetParameterValue;
	SVTriggerSetParameterValuePtr m_psvSetParameterValue;
};

#include "SVIOTriggerLoadLibraryClass.inl"

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIOLibrary\SVIOTriggerLoadLibraryClass.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:48:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Dec 2009 12:17:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Jan 2004 07:19:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added functions to allow configuring IO to work with special SVIM hardware
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Oct 2003 12:57:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated exposed function prototypes to change parameters from a class to individual items.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 13:21:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added I/O objects to new I/O configuration interface library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
