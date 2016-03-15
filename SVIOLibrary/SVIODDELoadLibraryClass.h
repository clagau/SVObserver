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

