// ******************************************************************************
// * COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

using TriggerData = std::array<_variant_t, SvDef::cTriggerDataNumber>;

typedef HRESULT (WINAPI *SVCreatePtr)( void );
typedef HRESULT (WINAPI *SVDestroyPtr)( void );
typedef unsigned long (WINAPI *SVInputGetCountPtr)();
typedef bool (WINAPI *SVInputGetValuePtr)(unsigned long);
typedef unsigned long (WINAPI *SVInputGetValuesPtr)();
typedef unsigned long (WINAPI *SVOutputGetCountPtr)();
typedef HRESULT (WINAPI *SVOutputSetValuePtr)(unsigned long, bool);
typedef unsigned long (WINAPI *SVOutputGetPortCountPtr)();
typedef HRESULT (WINAPI *SVOutputSetPortValuePtr)(unsigned long, unsigned long);
typedef HRESULT (WINAPI *SVOutputSetDataPtr)(unsigned long, const TriggerData&);

typedef unsigned long (WINAPI *SVGetParameterCountPtr)();
typedef _variant_t (WINAPI *SVGetParameterNamePtr)( unsigned long);
typedef _variant_t(WINAPI *SVGetParameterValuePtr)(unsigned long);
typedef HRESULT (WINAPI *SVSetParameterValuePtr)( unsigned long , const _variant_t&);

class SVIODigitalLoadLibraryClass  
{
public:
	SVIODigitalLoadLibraryClass() = default;
	~SVIODigitalLoadLibraryClass();

	HRESULT Open(HMODULE libraryHandle);
	HRESULT Close();
	
	unsigned long GetInputCount() const;
	bool GetInputValue(unsigned long channel) const;
	unsigned long GetInputValues() const;

	unsigned long GetOutputCount() const;
	HRESULT SetOutputValue(unsigned long channel, bool value);

	unsigned long GetOutputPortCount() const;
	HRESULT SetOutputPortValue(unsigned long port, unsigned long value);

	HRESULT SetOutputData(unsigned long channel, const TriggerData& rData);

	// Parameter related functions. Not implemented in all dlls.
	unsigned long GetParameterCount() const;
	_variant_t GetParameterName( unsigned long index) const;
	_variant_t GetParameterValue( unsigned long index) const;
	HRESULT SetParameterValue( unsigned long index, const _variant_t& rValue );
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

