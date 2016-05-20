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

#include "TriggerHandling/SVCallbackStruct.h"

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
	HRESULT Register( unsigned long p_ulHandle, SvTh::SVCallbackStruct &p_rCallback );
	HRESULT Unregister( unsigned long p_ulHandle, SvTh::SVCallbackStruct &p_rCallback );
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

