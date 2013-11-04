// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIODigitalLoadLibraryClass
// * .File Name       : $Workfile:   SVIODigitalLoadLibraryClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:46:40  $
// ******************************************************************************

#ifndef SVIODIGITALLOADLIBRARYCLASS_H
#define SVIODIGITALLOADLIBRARYCLASS_H

#include "SVOMFCLibrary/SVCallbackStruct.h"

typedef HRESULT (WINAPI *SVCreatePtr)( void );
typedef HRESULT (WINAPI *SVDestroyPtr)( void );
typedef HRESULT (WINAPI *SVInputGetCountPtr)( unsigned long * );
typedef HRESULT (WINAPI *SVInputGetValuePtr)( unsigned long, bool * );
typedef HRESULT (WINAPI *SVInputSetValuePtr)( unsigned long, bool );
typedef HRESULT (WINAPI *SVInputGetPortCountPtr)( unsigned long * );
typedef HRESULT (WINAPI *SVInputGetPortValuePtr)( unsigned long, unsigned long * );
typedef HRESULT (WINAPI *SVInputSetPortValuePtr)( unsigned long, unsigned long );
typedef HRESULT (WINAPI *SVOutputGetCountPtr)( unsigned long * );
typedef HRESULT (WINAPI *SVOutputGetValuePtr)( unsigned long, bool * );
typedef HRESULT (WINAPI *SVOutputSetValuePtr)( unsigned long, bool );
typedef HRESULT (WINAPI *SVOutputGetPortCountPtr)( unsigned long * );
typedef HRESULT (WINAPI *SVOutputGetPortValuePtr)( unsigned long, unsigned long * );
typedef HRESULT (WINAPI *SVOutputSetPortValuePtr)( unsigned long, unsigned long );

typedef HRESULT (WINAPI *SVGetParameterCountPtr)( unsigned long * );
typedef HRESULT (WINAPI *SVGetParameterNamePtr)( unsigned long, BSTR * );
typedef HRESULT (WINAPI *SVGetParameterValuePtr)( unsigned long, VARIANT * );
typedef HRESULT (WINAPI *SVSetParameterValuePtr)( unsigned long , VARIANT * );

class SVIODigitalLoadLibraryClass  
{
public:
	SVIODigitalLoadLibraryClass();
	~SVIODigitalLoadLibraryClass();

	HRESULT Open( LPCTSTR p_szLibrary );
	HRESULT Close();
	
	HRESULT GetInputCount( unsigned long *p_pulCount );
	HRESULT GetInputValue( unsigned long p_ulChannel, bool *p_pbValue );
	HRESULT SetInputValue( unsigned long p_ulChannel, bool p_bValue );

	HRESULT GetInputPortCount( unsigned long *p_pulCount );
	HRESULT GetInputPortValue( unsigned long p_ulPort, unsigned long *p_pulValue );
	HRESULT SetInputPortValue( unsigned long p_ulPort, unsigned long p_ulValue );

	HRESULT GetOutputCount( unsigned long *p_pulCount );
	HRESULT GetOutputValue( unsigned long p_ulChannel, bool *p_pbValue );
	HRESULT SetOutputValue( unsigned long p_ulChannel, bool p_bValue );

	HRESULT GetOutputPortCount( unsigned long *p_pulCount );
	HRESULT GetOutputPortValue( unsigned long p_ulPort, unsigned long *p_pulValue );
	HRESULT SetOutputPortValue( unsigned long p_ulPort, unsigned long p_ulValue );
	// Parameter related functions. Not implemented in all dlls.
	HRESULT GetParameterCount( unsigned long *p_pulCount );
	HRESULT GetParameterName( unsigned long p_ulIndex, BSTR *p_pbstrName );
	HRESULT GetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue );
	HRESULT SetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue );
private:
	HMODULE m_hmHandle;

	SVCreatePtr m_psvCreate;
	SVDestroyPtr m_psvDestroy;
	SVInputGetCountPtr m_psvGetInputCount;
	SVInputGetValuePtr m_psvGetInputValue;
	SVInputSetValuePtr m_psvSetInputValue;
	SVInputGetPortCountPtr m_psvGetInputPortCount;
	SVInputGetPortValuePtr m_psvGetInputPortValue;
	SVInputSetPortValuePtr m_psvSetInputPortValue;
	SVOutputGetCountPtr m_psvGetOutputCount;
	SVOutputGetValuePtr m_psvGetOutputValue;
	SVOutputSetValuePtr m_psvSetOutputValue;
	SVOutputGetPortCountPtr m_psvGetOutputPortCount;
	SVOutputGetPortValuePtr m_psvGetOutputPortValue;
	SVOutputSetPortValuePtr m_psvSetOutputPortValue;

	// Parameter related pointers.
	SVSetParameterValuePtr m_psvSetParameterValue;
	SVGetParameterValuePtr m_psvGetParameterValue;
	SVGetParameterNamePtr m_psvGetParameterName;
	SVGetParameterCountPtr m_psvGetParameterCount;
};

#include "SVIODigitalLoadLibraryClass.inl"

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIOLibrary\SVIODigitalLoadLibraryClass.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:46:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Dec 2009 12:17:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jun 2006 09:04:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  564
 * SCR Title:  Add configurability to SVTVicLpt DLL to set the system type
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SetParameterValue, GetParameterValue, GetParameterCount, and GetParameterName.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Jul 2003 13:40:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Removed Rename method from header.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 13:21:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added I/O objects to new I/O configuration interface library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
