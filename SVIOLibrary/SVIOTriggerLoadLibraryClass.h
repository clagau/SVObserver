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

//namespace used only for forward declaration
namespace SvTh
{
	class TriggerDispatcher;
} //namespace SvTh

typedef HRESULT (WINAPI *SVCreatePtr)( void );
typedef HRESULT (WINAPI *SVDestroyPtr)( void );
typedef HRESULT (WINAPI *SVTriggerGetCountPtr)( unsigned long * );
typedef HRESULT (WINAPI *SVTriggerGetHandlePtr)( unsigned long *, unsigned long );
typedef HRESULT (WINAPI *SVTriggerGetNamePtr)( unsigned long, BSTR * );
typedef HRESULT (WINAPI *SVTriggerRegisterPtr)( unsigned long, const SvTh::TriggerDispatcher &rDispatcher );
typedef HRESULT (WINAPI *SVTriggerUnregisterPtr)( unsigned long, const SvTh::TriggerDispatcher &rDispatcher );
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
	HRESULT GetHandle( unsigned long *pTriggerchannel, unsigned long p_ulIndex );
	HRESULT GetName( unsigned long triggerchannel, BSTR *p_pbstrName );
	HRESULT Register( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher );
	HRESULT Unregister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher );
	HRESULT UnregisterAll( unsigned long triggerchannel );
	HRESULT Start( unsigned long triggerchannel );
	HRESULT Stop( unsigned long triggerchannel );
	HRESULT GetParameterCount( unsigned long triggerchannel, unsigned long *p_pulCount );
	HRESULT GetParameterName( unsigned long triggerchannel, unsigned long p_ulIndex, BSTR *p_pbstrName );
	HRESULT GetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue );
	HRESULT SetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue );

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
