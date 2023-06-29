//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************
// This file contains helper functions for values used in the Gui.
//******************************************************************************
#pragma once

#pragma region Includes

#include "SVUtilityLibrary/SVSafeArray.h"

#pragma endregion Includes

namespace SvOgu
{
template <typename T>
std::vector<T> ConvertVariantSafeArrayToVector(const _variant_t& var)
{
	std::vector<T> vec;
	if (var.vt & VT_ARRAY)
	{
		if (var.parray)
		{
			long lBound;
			long uBound;
			SafeArrayGetLBound(var.parray, 1, &lBound);
			SafeArrayGetUBound(var.parray, 1, &uBound);
			long count = uBound - lBound + 1;  // bounds are inclusive

			T* raw;
			SafeArrayAccessData(var.parray, reinterpret_cast<void**>(&raw));

			// assign to an existing vector
			vec.assign(raw, raw + count);

			// When no longer need "raw".
			SafeArrayUnaccessData(var.parray);
		}
	}
	else if (VT_EMPTY != var.vt)
	{
		T value = static_cast<T>(var);
		vec.push_back(value);
	}
	return vec;
}

template <typename V>
_variant_t ConvertVectorToVariantSafeArray(const V& vec)
{
	SvUl::SVSAFEARRAY safeArray;
	_variant_t v;
			
	for (long i = 0; i < vec.size(); ++i)
	{
		v = vec[i];
		safeArray.Add( v );
	}
			
	v = safeArray.operator _variant_t();
	return v;
}
} //namespace SvOgu