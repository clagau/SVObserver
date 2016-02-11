//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// IRootObject is the interface to get/set the Root children objects
/// By now there are only function to call intern static methods.
//******************************************************************************

#pragma once

#pragma region Includes
#include "ISelectorItemVector.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		//************************************
		/// Get the Root child object list from a path and specified filter.
		/// \param rObjectNameList [out] The returned object name list.
		/// \param Path [in] Path of the root child desired. Default = "", this means all objects.
		/// \param AttributesAllowedFilter [in] Filter of the environment which are wanted. Default = 0, this means all objects.
		//************************************
		void getRootChildNameList( SVStringArray& rObjectNameList, LPCTSTR Path = _T(""), UINT AttributesAllowedFilter = 0 );

		//************************************
		/// Get the Root child selector list from a path and specified filter.
		/// \param Path [in] Path of the root child desired. Default = "", this means all objects.
		/// \param AttributesAllowedFilter [in] Filter of the environment which are wanted. Default = 0, this means all objects.
		/// \return rObjectNameList [out] The returned object name list.
		//************************************
		ISelectorItemVectorPtr getRootChildSelectorList( LPCTSTR Path = _T(""), UINT AttributesAllowedFilter = 0 );
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;