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
#include <comutil.h>
#include "SVPoint.h"

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
	SVSAFEARRAY(VARTYPE p_Type, size_type p_Size);
	SVSAFEARRAY(VARTYPE p_Type, const SVBounds& p_rBounds);
	SVSAFEARRAY(const SVSAFEARRAY& p_rObject);
	explicit SVSAFEARRAY(const VARIANT& p_rVariant);
	explicit SVSAFEARRAY(SAFEARRAY* p_pObject);

	virtual ~SVSAFEARRAY();

	bool empty() const;

	void clear();

	operator SAFEARRAY * ();

	operator _variant_t () const;

	const SVSAFEARRAY& operator=(const SVSAFEARRAY& p_rObject);
	const SVSAFEARRAY& operator=(const VARIANT& p_rVariant);
	const SVSAFEARRAY& operator=(SAFEARRAY* p_pObject);

	template< class InputIterator >
	void assign(InputIterator _First, InputIterator _Last);

	HRESULT GetVartype(VARTYPE& p_rType) const;
	HRESULT GetBounds(SVBounds& p_rBounds) const;

	size_type size() const;

	unsigned int GetDim() const;

	HRESULT GetElement(size_type p_Index, _variant_t& p_rData) const;
	HRESULT GetElement(const SVIndex& p_rIndex, _variant_t& p_rData) const;

	HRESULT PutElement(size_type p_Index, const _variant_t& p_rData);
	HRESULT PutElement(const SVIndex& p_rIndex, const _variant_t& p_rData);

	HRESULT Add(const _variant_t& p_rData);

	SAFEARRAY* getSafeArray() { return m_pSafeArray; }

protected:
	HRESULT UpdateVariant(VARTYPE p_Type, const void* p_pData, _variant_t &p_rVariant) const;
	HRESULT GetGeneralVoidAddress(const _variant_t &p_rVariant, void*& p_rpData) const;
	HRESULT GetElementVoidAddress(const _variant_t &p_rVariant, void*& p_rpData) const;
	HRESULT PutElementVoidAddress(const _variant_t &p_rVariant, void*& p_rpData) const;

	SAFEARRAY* m_pSafeArray;

private:
	void LocalDestroy();

};

long getArraySizeFromOneDim(const _variant_t& rValue);

template<class T>
inline std::vector<T> getVectorFromOneDim(const _variant_t& rValue)
{
	std::vector<T> result;
	long arraySize = getArraySizeFromOneDim(rValue);
	if (arraySize > 0)
	{
		result.resize(arraySize);
		//set all value to array
		for (long i = 0; i < arraySize; i++)
		{
			T  value;
			switch (rValue.vt)
			{

				case  (VT_ARRAY | VT_I8):
				{
					long long  temp;
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&temp));
					value = static_cast<T>(temp);
					break;
				}

				case  (VT_ARRAY | VT_R8):
				{
					double dtemp;
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&dtemp));
					value = static_cast<T>(dtemp);
					break;
				}
				case  (VT_ARRAY | VT_R4):
				{
					float temp;
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&temp));
					value = static_cast<T>(temp);
					break;
				}

				case VT_ARRAY | VT_I4:
				{
					long dlong;
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&dlong));
					value = static_cast<T>(dlong);
					break;
				}
				case VT_ARRAY | VT_UI4:
				{
					unsigned long  ulong;
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&ulong));
					value = static_cast<T>(ulong);
					break;
				}

				case VT_ARRAY | VT_UI2:
				{
					unsigned short  ushort;
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&ushort));
					value = static_cast<T>(ushort);
					break;
				}
				case VT_ARRAY | VT_I2:
				{
					short  shortv;
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&shortv));
					value = static_cast<T>(shortv);
					break;
				}
				default:
				{
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&value));
				}
			}
			result[i] = value;
		}
	}
	return result;

}


//Template specialization to avoid compile error

template<>
inline std::vector<SVPoint<double>> getVectorFromOneDim<SVPoint<double>>(const _variant_t& )
{
	std::vector<SVPoint<double>> result;
	assert(false);
	return result;
}

template<>
inline std::vector<SVPoint<long>> getVectorFromOneDim<SVPoint<long>>(const _variant_t& )
{
	std::vector<SVPoint<long>> result;
	assert(false);
	return result;
}

template<>
inline std::vector<std::string> getVectorFromOneDim<std::string>(const _variant_t& )
{
	std::vector<std::string> result;
	assert(false);
	return result;
}

template<>
inline std::vector<_variant_t> getVectorFromOneDim<_variant_t>(const _variant_t& rValue)
{
	std::vector<_variant_t> result;
	long arraySize = getArraySizeFromOneDim(rValue);
	if (arraySize > 0)
	{
		result.resize(arraySize);
		//set all value to array
		for (long i = 0; i < arraySize; i++)
		{
			_variant_t value;
			switch (rValue.vt)
			{

				case  (VT_ARRAY | VT_I8):
				{
					long long  temp;
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&temp));
					value = temp;
					break;
				}

				case  (VT_ARRAY | VT_R8):
				{
					double dtemp;
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&dtemp));
					value = dtemp;
					break;
				}
				case  (VT_ARRAY | VT_R4):
				{
					float temp;
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&temp));
					value = temp;
					break;
				}

				case VT_ARRAY | VT_I4:
				{
					long dlong;
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&dlong));
					value = dlong;
					break;
				}
				case VT_ARRAY | VT_UI4:
				{
					unsigned long  ulong;
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&ulong));
					value = ulong;
					break;
				}

				case VT_ARRAY | VT_UI2:
				{
					unsigned short  ushort;
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&ushort));
					value = ushort;
					break;
				}
				case VT_ARRAY | VT_I2:
				{
					short  shortv;
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&shortv));
					value = shortv;
					break;
				}
				default:
				{
					::SafeArrayGetElement(rValue.parray, &i, static_cast<void*> (&value));
				}
			}
			result[i] = value;
		}
	}
	return result;
}


template<class T>
inline bool getSingleVariantFromArrayOneDim(const _variant_t& rValue, long pos, T& value)
{
	
	long arraySize = getArraySizeFromOneDim(rValue);
	if (arraySize > pos)
	{
		HRESULT hr {S_OK};
		switch (rValue.vt)
		{

			case  (VT_ARRAY | VT_I8):
			{
				long long  temp;
				hr = ::SafeArrayGetElement(rValue.parray, &pos, static_cast<void*> (&temp));
				value = static_cast<T>(temp);
				break;
			}

			case  (VT_ARRAY | VT_R8):
			{
				double dtemp;
				hr = ::SafeArrayGetElement(rValue.parray, &pos, static_cast<void*> (&dtemp));
				value = static_cast<T>(dtemp);
				break;
			}

			case  (VT_ARRAY | VT_R4):
			{
				float temp;
				hr = ::SafeArrayGetElement(rValue.parray, &pos, static_cast<void*> (&temp));
				value = static_cast<T>(temp);
				break;
			}
			case VT_ARRAY | VT_I4:
			{
				long dlong;
				hr = ::SafeArrayGetElement(rValue.parray, &pos, static_cast<void*> (&dlong));
				value = static_cast<T>(dlong);
				break;
			}
			case VT_ARRAY | VT_UI4:
			{
				unsigned long  ulong;
				hr = ::SafeArrayGetElement(rValue.parray, &pos, static_cast<void*> (&ulong));
				value = static_cast<T>(ulong);
				break;
			}

			case VT_ARRAY | VT_UI2:
			{
				unsigned short  ushort;
				hr = ::SafeArrayGetElement(rValue.parray, &pos, static_cast<void*> (&ushort));
				value = static_cast<T>(ushort);
				break;
			}
			case VT_ARRAY | VT_I2:
			{
				short  shortv;
				hr = ::SafeArrayGetElement(rValue.parray, &pos, static_cast<void*> (&shortv));
				value = static_cast<T>(shortv);
				break;
			}
			default:
			{
				hr = ::SafeArrayGetElement(rValue.parray, &pos, &value);
			}
		}
		return (S_OK == hr);
	}
	return false;
}

template<>
inline bool getSingleVariantFromArrayOneDim<_variant_t>(const _variant_t& rValue, long pos, _variant_t& value)
{
	
	long arraySize = getArraySizeFromOneDim(rValue);
	if (arraySize > pos)
	{
		HRESULT hr {S_OK};
		switch (rValue.vt)
		{

			case  (VT_ARRAY | VT_I8):
			{
				long long  temp;
				hr = ::SafeArrayGetElement(rValue.parray, &pos, static_cast<void*> (&temp));
				value = temp;
				break;
			}

			case  (VT_ARRAY | VT_R8):
			{
				double dtemp;
				hr = ::SafeArrayGetElement(rValue.parray, &pos, static_cast<void*> (&dtemp));
				value = dtemp;
				break;
			}

			case  (VT_ARRAY | VT_R4):
			{
				float temp;
				hr = ::SafeArrayGetElement(rValue.parray, &pos, static_cast<void*> (&temp));
				value = temp;
				break;
			}
			case VT_ARRAY | VT_I4:
			{
				long dlong;
				hr = ::SafeArrayGetElement(rValue.parray, &pos, static_cast<void*> (&dlong));
				value = dlong;
				break;
			}
			case VT_ARRAY | VT_UI4:
			{
				unsigned long  ulong;
				hr = ::SafeArrayGetElement(rValue.parray, &pos, static_cast<void*> (&ulong));
				value = ulong;
				break;
			}

			case VT_ARRAY | VT_UI2:
			{
				unsigned short  ushort;
				hr = ::SafeArrayGetElement(rValue.parray, &pos, static_cast<void*> (&ushort));
				value = ushort;
				break;
			}
			case VT_ARRAY | VT_I2:
			{
				short  shortv;
				hr = ::SafeArrayGetElement(rValue.parray, &pos, static_cast<void*> (&shortv));
				value = shortv;
				break;
			}
			default:
			{
				hr = ::SafeArrayGetElement(rValue.parray, &pos, &value);
			}
		}
		return (S_OK == hr);
	}
	return false;
}

#include "SVSAFEARRAY.inl"
} // namespace SvUl
