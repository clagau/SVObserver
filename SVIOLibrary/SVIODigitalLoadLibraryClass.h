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

#pragma region Includes
//Moved to precompiled header: #include <map>
#pragma endregion Includes

typedef  std::map<int, _variant_t> IntVariantMap;

typedef HRESULT (WINAPI *SVCreatePtr)( void );
typedef HRESULT (WINAPI *SVDestroyPtr)( void );
typedef HRESULT (WINAPI *SVInputGetCountPtr)( unsigned long* );
typedef HRESULT (WINAPI *SVInputGetValuePtr)( unsigned long, bool* );
typedef HRESULT (WINAPI *SVInputGetValuesPtr)( unsigned long* );
typedef HRESULT (WINAPI *SVOutputGetCountPtr)( unsigned long* );
typedef HRESULT (WINAPI *SVOutputSetValuePtr)( unsigned long, bool );
typedef HRESULT (WINAPI *SVOutputGetPortCountPtr)( unsigned long * );
typedef HRESULT(WINAPI *SVOutputSetPortValuePtr)(unsigned long, unsigned long);
typedef HRESULT(WINAPI *SVOutputSetDataPtr)(unsigned long, const IntVariantMap&);

typedef HRESULT (WINAPI *SVGetParameterCountPtr)( unsigned long* );
typedef HRESULT (WINAPI *SVGetParameterNamePtr)( unsigned long, BSTR* );
typedef HRESULT (WINAPI *SVGetParameterValuePtr)( unsigned long, VARIANT* );
typedef HRESULT (WINAPI *SVSetParameterValuePtr)( unsigned long , VARIANT* );

class SVIODigitalLoadLibraryClass  
{
public:
	SVIODigitalLoadLibraryClass() = default;
	~SVIODigitalLoadLibraryClass();

	HRESULT Open(LPCTSTR library);
	HRESULT Close();
	
	HRESULT GetInputCount( unsigned long* pCount );
	HRESULT GetInputValue( unsigned long channel, bool* pValue );
	HRESULT GetInputValues( unsigned long* pValue );

	HRESULT GetOutputCount( unsigned long* pCount );
	HRESULT SetOutputValue( unsigned long channel, bool value );

	HRESULT GetOutputPortCount( unsigned long* pCount );
	HRESULT SetOutputPortValue( unsigned long port, unsigned long value );

	HRESULT SetOutputData(unsigned long channel, const IntVariantMap& rData);

	// Parameter related functions. Not implemented in all dlls.
	HRESULT GetParameterCount( unsigned long* pCount );
	HRESULT GetParameterName( unsigned long index, BSTR* pName );
	HRESULT GetParameterValue( unsigned long index, VARIANT* pValue );
	HRESULT SetParameterValue( unsigned long index, VARIANT* pValue );
private:
	HMODULE m_Handle {nullptr};

	SVCreatePtr m_pCreate {nullptr};
	SVDestroyPtr m_pDestroy {nullptr};
	SVInputGetCountPtr m_pGetInputCount {nullptr};
	SVInputGetValuePtr m_pGetInputValue {nullptr};
	SVInputGetValuesPtr m_pGetInputValues {nullptr};
	SVOutputGetCountPtr m_pGetOutputCount {nullptr};
	SVOutputSetValuePtr m_pSetOutputValue {nullptr};
	SVOutputGetPortCountPtr m_pGetOutputPortCount {nullptr};
	SVOutputSetPortValuePtr m_pSetOutputPortValue {nullptr};
	SVOutputSetDataPtr m_pSetOutputData {nullptr};

	// Parameter related pointers.
	SVSetParameterValuePtr m_pSetParameterValue {nullptr};
	SVGetParameterValuePtr m_pGetParameterValue {nullptr};
	SVGetParameterNamePtr m_pGetParameterName {nullptr};
	SVGetParameterCountPtr m_pGetParameterCount {nullptr};
};

