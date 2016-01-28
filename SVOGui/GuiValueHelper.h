//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This file contains helper function for the Gui values.
//******************************************************************************
#pragma once

#pragma region Includes
#include <boost/noncopyable.hpp>
#include <boost/any.hpp>
#include <vector>
#include <comdef.h>
#include <guiddef.h>
#include <atlsafe.h>
#include <oleauto.h>
#include "SVUtilityLibrary/SVSafeArray.h"

#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		template <typename T>
		std::vector<T> ConvertVariantSafeArrayToVector(const _variant_t& var)
		{
			std::vector<T> vec;
			if ((var.vt & VT_ARRAY) && var.parray)
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
			return vec;
		}

		template <typename V>
		_variant_t ConvertVectorToVariantSafeArray(const V& vec)
		{
			SVSAFEARRAY safeArray;
			_variant_t v;
			
			for (long i = 0; i < vec.size(); ++i)
			{
				v = vec[i];
				safeArray.Add( v );
			}
			
			v = safeArray;
			return v;
		}

	}
}

namespace SvOg = Seidenader::SVOGui;