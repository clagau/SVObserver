//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSAFEARRAY
//* .File Name       : $Workfile:   SVSAFEARRAY.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:24:12  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <vector>

namespace SvUl
{
/**
@SVObjectName Safe Array

@SVObjectOverview

@SVObjectOperations

*/
class SVSAFEARRAY
{
public:
	typedef std::vector< long > SVIndex;
	typedef std::vector< SAFEARRAYBOUND > SVBounds;
	typedef size_t size_type;

	SVSAFEARRAY();
	SVSAFEARRAY( VARTYPE p_Type, size_type p_Size );
	SVSAFEARRAY( VARTYPE p_Type, const SVBounds& p_rBounds );
	SVSAFEARRAY( const SVSAFEARRAY& p_rObject );
	SVSAFEARRAY( const VARIANT& p_rVariant );
	SVSAFEARRAY( SAFEARRAY* p_pObject );

	virtual ~SVSAFEARRAY();

	bool empty() const;

	void clear();

	operator SAFEARRAY * ();

	operator _variant_t () const;
	
	const SVSAFEARRAY& operator=( const SVSAFEARRAY& p_rObject );
	const SVSAFEARRAY& operator=( const VARIANT& p_rVariant );
	const SVSAFEARRAY& operator=( SAFEARRAY* p_pObject );

	template< class InputIterator >
	void assign( InputIterator _First, InputIterator _Last );

	HRESULT GetVartype( VARTYPE& p_rType ) const;
	HRESULT GetBounds( SVBounds& p_rBounds ) const;

	size_type size() const;

	unsigned int GetDim() const;

	HRESULT GetElement( size_type p_Index, _variant_t& p_rData ) const;
	HRESULT GetElement( const SVIndex& p_rIndex, _variant_t& p_rData ) const;

	HRESULT PutElement( size_type p_Index, const _variant_t& p_rData );
	HRESULT PutElement( const SVIndex& p_rIndex, const _variant_t& p_rData );

	HRESULT Add( const _variant_t& p_rData );

	SAFEARRAY* getSafeArray() { return m_pSafeArray; }

protected:
	HRESULT UpdateVariant( VARTYPE p_Type, const void* p_pData, _variant_t &p_rVariant ) const;
	HRESULT GetGeneralVoidAddress( const _variant_t &p_rVariant, void*& p_rpData ) const;
	HRESULT GetElementVoidAddress( const _variant_t &p_rVariant, void*& p_rpData ) const;
	HRESULT PutElementVoidAddress( const _variant_t &p_rVariant, void*& p_rpData ) const;

	SAFEARRAY* m_pSafeArray;

private:
	void LocalDestroy();

};

long getArraySizeFromOneDim(const _variant_t& rValue);

template<class T>
std::vector<T> getVectorFromOneDim(const _variant_t& rValue)
{
	std::vector<T> result;
	long arraySize = getArraySizeFromOneDim(rValue);
	if (arraySize > 0)
	{
		result.resize(arraySize);
		//set all value to array
		for (long i = 0; i < arraySize; i++)
		{
			T value;
			::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&value));
			result[i] = value;
		}
	}
	return result;
}

template<class T>
bool getSingleVariantFromArrayOneDim(const _variant_t& rValue, long pos, T& value)
{
	long arraySize = getArraySizeFromOneDim(rValue);
	if (arraySize > pos)
	{
		HRESULT hr = ::SafeArrayGetElement(rValue.parray, &pos, &value);
		return (S_OK == hr);
	}
	return false;
}
#include "SVSAFEARRAY.inl"
} // namespace SvUl
