//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// IEnvironmentObject is the interface to get/set the environment objects
/// By now there are only function to call intern static methods.
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		//************************************
		/// Get the Environment object list from a path and specified filter.
		/// \param rObjectNameList [out] The returned object name list.
		/// \param rPath [in] Path of the environment which are wanted. Default = "", this means all objects.
		/// \param AttributesAllowedFilter [in] Filter of the environment which are wanted. Default = 0, this means all objects.
		//************************************
		void getEnvironmentObjectNameList(SVStringArray& rObjectNameList, const SVString& rPath = _T(""), UINT AttributesAllowedFilter = 0);
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;