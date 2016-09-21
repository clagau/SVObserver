//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObjectReference
//* .File Name       : $Workfile:   SVValueObjectReference.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 08:24:40  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVObjectLibrary/SVObjectReference.h"
#include "SVValueObject.h"
#pragma endregion Includes

class SVValueObjectReference : public SVCheckedObjectReference<SVValueObjectClass>
{
public:
	typedef SVCheckedObjectReference<SVValueObjectClass> base;
	// forwarding constructors...
	SVValueObjectReference() : base() {}
	SVValueObjectReference( SVObjectClass* pObject, long lArrayIndex, CString strDefaultValue = CString() ) : base(pObject, lArrayIndex, strDefaultValue) {}
	SVValueObjectReference( SVObjectClass* pObject, const SVObjectNameInfo& p_rNameInfo) : base(pObject, p_rNameInfo) {}
	SVValueObjectReference( SVObjectClass* pObject ) : base(pObject) {}
	SVValueObjectReference( const SVObjectReference& rhs ) : base(rhs) {}
	const SVValueObjectReference& operator = ( const SVObjectReference& rhs ) { base::operator = (rhs); return *this; }


	//interface into value object using internal array index
	template <typename T>
	__forceinline HRESULT SetValue(int iBucket, T value)
	{
		return (static_cast<SVValueObjectClass*>(m_pObject))->SetValueAtConvert(iBucket,  GetArrayIndexNotNegative(), value);
	}

	template <typename T>
	__forceinline HRESULT GetValue(T& rValue) const
	{
		return (static_cast<SVValueObjectClass*>(m_pObject))->GetValueAt((static_cast<SVValueObjectClass*>(m_pObject))->m_iLastSetIndex,GetArrayIndexNotNegative(), rValue);
	}

	template <typename T>
	__forceinline HRESULT GetValue(int iBucket, T& rValue) const
	{
		return (static_cast<SVValueObjectClass*>(m_pObject))->GetValueAt(iBucket, GetArrayIndexNotNegative(), rValue);
	}

	__forceinline HRESULT GetValues(std::vector< _variant_t >&  rValue) const
	{
		return (static_cast<SVValueObjectClass*>(m_pObject))->GetValues(rValue);
	}

	/// Return the value array as string semicolon-separated.
	/// \param rValue [out] The return value.
	/// \returns HRESULT
	HRESULT GetValues(CString& rValue) const
	{
		return (static_cast<SVValueObjectClass*>(m_pObject))->GetValues(rValue);
	}

	template<typename T, typename Iter>
	__forceinline HRESULT SetArrayValues(int iBucket, Iter begin, Iter end)
	{
		typedef SVValueObjectClassImpl<T> VOType;
		VOType * obj = dynamic_cast<VOType *>(m_pObject);
		if (obj)
		{
			HRESULT hr = obj->SetArraySize(static_cast<int>(end - begin));
			if (S_OK != hr)
			{
				return hr;
			}
			else
			{
				return obj->SetArrayValues(iBucket, begin, end);
			}
		}
		else
		{
			return TYPE_E_TYPEMISMATCH;
		}
	}

	template<typename Cont>
	__forceinline HRESULT SetArrayValues(int iBucket, const Cont & cont)
	{
		typedef typename Cont::value_type Type;
		typedef typename Cont::const_iterator Iter;
		return SetArrayValues<Type, Iter>(iBucket, cont.begin(), cont.end());
	}
private :
	int 	GetArrayIndexNotNegative() const 
	{ 
		return  ((m_IsArray && (m_ArrayIndex > -1) )?   m_ArrayIndex : 0); 
	}
};

typedef std::vector<SVValueObjectReference> SVValueObjectReferenceVector;
