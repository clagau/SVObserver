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

#pragma once

#include "TriggerInformation/SVTriggerInfoStruct.h"

typedef HRESULT (WINAPI *SVCreatePtr)( void );
typedef HRESULT (WINAPI *SVDestroyPtr)( void );
typedef HRESULT (WINAPI *SVTriggerGetCountPtr)( unsigned long* );
typedef HRESULT (WINAPI *SVTriggerGetHandlePtr)( unsigned long*, unsigned long );
typedef HRESULT (WINAPI *SVTriggerGetNamePtr)( unsigned long, BSTR* );
typedef HRESULT (WINAPI *SVTriggerRegisterPtr)( unsigned long, SvTi::TriggerCallBack pTriggerCallback);
typedef HRESULT (WINAPI *SVTriggerUnregisterPtr)( unsigned long);
typedef HRESULT (WINAPI *SVTriggerStartPtr)( unsigned long );
typedef HRESULT (WINAPI *SVTriggerStopPtr)( unsigned long );
typedef HRESULT (WINAPI *SVTriggerGetParameterCountPtr)( unsigned long, unsigned long* );
typedef HRESULT (WINAPI *SVTriggerGetParameterNamePtr)( unsigned long, unsigned long, BSTR* );
typedef HRESULT (WINAPI *SVTriggerGetParameterValuePtr)( unsigned long, unsigned long, VARIANT* );
typedef HRESULT (WINAPI *SVTriggerSetParameterValuePtr)( unsigned long, unsigned long, VARIANT* );

class SVIOTriggerLoadLibraryClass  
{
public:
	SVIOTriggerLoadLibraryClass() = default;
	~SVIOTriggerLoadLibraryClass();

	HRESULT Open( LPCTSTR libraryPath );
	HRESULT Close();
	
	HRESULT GetCount( unsigned long* pCount );
	HRESULT GetHandle( unsigned long* pTriggerChannel, unsigned long index );
	HRESULT GetName( unsigned long triggerChannel, BSTR* pName );
	HRESULT Register( unsigned long triggerChannel, SvTi::TriggerCallBack pTriggerCallback);
	HRESULT Unregister( unsigned long triggerChannel);
	HRESULT Start( unsigned long triggerChannel );
	HRESULT Stop( unsigned long triggerChannel );
	HRESULT GetParameterCount( unsigned long triggerChannel, unsigned long* pCount );
	HRESULT GetParameterName( unsigned long triggerChannel, unsigned long index, BSTR* pName );
	HRESULT GetParameterValue( unsigned long triggerChannel, unsigned long index, VARIANT* pValue );
	HRESULT SetParameterValue( unsigned long triggerChannel, unsigned long index, VARIANT* pValue );

private:
	HMODULE m_Handle {nullptr};

	SVCreatePtr m_pCreate {nullptr};
	SVDestroyPtr m_pDestroy {nullptr};
	SVTriggerGetCountPtr m_pGetCount {nullptr};
	SVTriggerGetHandlePtr m_pGetHandle {nullptr};
	SVTriggerGetNamePtr m_pGetName {nullptr};
	SVTriggerRegisterPtr m_pRegister {nullptr};
	SVTriggerUnregisterPtr m_pUnregister {nullptr};
	SVTriggerStartPtr m_pStart {nullptr};
	SVTriggerStopPtr m_pStop {nullptr};
	SVTriggerGetParameterCountPtr m_pGetParameterCount {nullptr};
	SVTriggerGetParameterNamePtr m_pGetParameterName {nullptr};
	SVTriggerGetParameterValuePtr m_pGetParameterValue {nullptr};
	SVTriggerSetParameterValuePtr m_pSetParameterValue {nullptr};
};
