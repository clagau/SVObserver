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
typedef HRESULT (WINAPI *SVInputSetValuePtr)( unsigned long, bool );
typedef HRESULT (WINAPI *SVInputGetPortCountPtr)( unsigned long* );
typedef HRESULT (WINAPI *SVInputGetPortValuePtr)( unsigned long, unsigned long* );
typedef HRESULT (WINAPI *SVInputSetPortValuePtr)( unsigned long, unsigned long );
typedef HRESULT (WINAPI *SVOutputGetCountPtr)( unsigned long* );
typedef HRESULT (WINAPI *SVOutputGetValuePtr)( unsigned long, bool* );
typedef HRESULT (WINAPI *SVOutputSetValuePtr)( unsigned long, bool );
typedef HRESULT (WINAPI *SVOutputGetPortCountPtr)( unsigned long * );
typedef HRESULT (WINAPI *SVOutputGetPortValuePtr)( unsigned long, unsigned long* );
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
	HRESULT SetInputValue( unsigned long channel, bool value );

	HRESULT GetInputPortCount( unsigned long* pCount );
	HRESULT GetInputPortValue( unsigned long port, unsigned long* pValue );
	HRESULT SetInputPortValue( unsigned long port, unsigned long value );

	HRESULT GetOutputCount( unsigned long* pCount );
	HRESULT GetOutputValue( unsigned long channel, bool* pValue );
	HRESULT SetOutputValue( unsigned long channel, bool value );

	HRESULT GetOutputPortCount( unsigned long* pCount );
	HRESULT GetOutputPortValue( unsigned long port, unsigned long* pValue );
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
	SVInputSetValuePtr m_pSetInputValue {nullptr};
	SVInputGetPortCountPtr m_pGetInputPortCount {nullptr};
	SVInputGetPortValuePtr m_pGetInputPortValue {nullptr};
	SVInputSetPortValuePtr m_pSetInputPortValue {nullptr};
	SVOutputGetCountPtr m_pGetOutputCount {nullptr};
	SVOutputGetValuePtr m_pGetOutputValue {nullptr};
	SVOutputSetValuePtr m_pSetOutputValue {nullptr};
	SVOutputGetPortCountPtr m_pGetOutputPortCount {nullptr};
	SVOutputGetPortValuePtr m_pGetOutputPortValue {nullptr};
	SVOutputSetPortValuePtr m_pSetOutputPortValue {nullptr};
	SVOutputSetDataPtr m_pSetOutputData {nullptr};

	// Parameter related pointers.
	SVSetParameterValuePtr m_pSetParameterValue {nullptr};
	SVGetParameterValuePtr m_pGetParameterValue {nullptr};
	SVGetParameterNamePtr m_pGetParameterName {nullptr};
	SVGetParameterCountPtr m_pGetParameterCount {nullptr};
};

