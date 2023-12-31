//*****************************************************************************
/// \copyright (c) 2019,2019 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file SafeArrayHelper.h
/// All Rights Reserved 
//*****************************************************************************
//Helper function for SAFEARRAY
//******************************************************************************
#pragma once

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace SvUl
{
enum class VTGroups
{
	Empty,
	Numbers,
	Text,
	Unknown
};

/// Return VTGroup-Type and if it is an array.
/// \param type [in]
/// \returns std::pair<SvUl::VTGroups, bool> 
std::pair<VTGroups, bool> getVTGroup(VARTYPE type);

/// function converts var to string. If var is a 1 dim safearray the string is the comma separated values 
std::string VariantToString(_variant_t var);

//! function returns a 1 dim safe array with one element for an scalar variant
_variant_t VariantToSafeArray(_variant_t var);

///! function convert a safeArray-Variant to another safeArray-Variant with different type. If not possible it return an empty variant
_variant_t convertSafeArrayToOtherSafeArray(_variant_t var, VARTYPE  newType);

//! function returns a 1 dim safe array with 0 element 
_variant_t GetEmptySafeArray(VARTYPE  vart);

bool isSameVar(const _variant_t& rValue1, const _variant_t& rValue2);

/// function converts 1 dim safearray of type T to a string of ';'-separated values 
template<typename T>
std::string SafeArrayToString(SAFEARRAY* pArray)
{
	if (nullptr != pArray)
	{
		std::stringstream sst;
		long lowerBound = pArray->rgsabound[0].lLbound;
		long upperBound = lowerBound + static_cast<long> (pArray->rgsabound[0].cElements);

		for (long i = lowerBound; i < upperBound; ++i)
		{
			T value;
			if (S_OK == ::SafeArrayGetElement(pArray, &i, &value))
			{
				sst << value;
				if (i < upperBound - 1)
				{
					sst << ';';
				}
			}
		}
		return  sst.str();
	}
	return {};
}

template<typename T> // T must be a number type
std::vector<T> SafeArrayToVector(SAFEARRAY* pArray) 
{
	if (nullptr != pArray)
	{
		std::vector<T> retVec;
		long lowerBound = pArray->rgsabound[0].lLbound;
		long upperBound = lowerBound + static_cast<long> (pArray->rgsabound[0].cElements);

		for (long i = lowerBound; i < upperBound; ++i)
		{
			T value;
			if (S_OK == ::SafeArrayGetElement(pArray, &i, &value))
			{
				retVec.push_back(value);
			}
		}
		return retVec;
	}
	return {};
}

///function creates a 1 dim safe array of type T from comma separated string. return value is size of array  
///empty string is array of size 0, negative result signals an error.
template<typename T> // T must be a number type
int StringToSafeArray(const std::string& rText, _variant_t& rVariant)
{
	T tempType {};
	_variant_t tempValue {tempType};
	VARTYPE varType {tempValue.vt};
	rVariant.Clear();
	if (rText.empty())
	{
		SAFEARRAYBOUND arrayBound;
		arrayBound.lLbound = 0;
		arrayBound.cElements = 0;
		rVariant.parray = ::SafeArrayCreate(varType, 1, &arrayBound);
		rVariant.vt = varType | VT_ARRAY;
		return 0;
	}

	std::vector<std::string> words;
	boost::algorithm::split(words, rText, boost::is_any_of(" ,;"), boost::token_compress_on);
	if (words.size() == 0)
	{
		return -1;
	}

	SAFEARRAYBOUND arrayBound;
	arrayBound.lLbound = 0;
	arrayBound.cElements = static_cast<unsigned long> (words.size());
	rVariant.parray = ::SafeArrayCreate(varType, 1, &arrayBound);
	rVariant.vt = varType | VT_ARRAY;
	for (long i = 0; i < static_cast<long> (words.size()); i++)
	{
		try
		{
			T value = boost::lexical_cast<T>(words[i]);
			::SafeArrayPutElement(rVariant.parray, &i, &value);
		}
		catch (...)
		{
			return -1;
		}
	}
	return static_cast<int> (arrayBound.cElements);
}

template<typename T>
variant_t vectorToSafeArray(const std::vector<T>& rVec);

_variant_t getVariantFromStringVector(const std::vector<std::string>& strings);
}