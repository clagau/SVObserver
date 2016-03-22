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

#pragma once

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

