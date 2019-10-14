//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// IRootObject is the interface to get/set the Root children objects
/// By now there are only function to call intern static methods.
//******************************************************************************

#pragma once

#pragma region Includes
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

#pragma region Declarations
namespace SvPb
{
class GetObjectSelectorItemsResponse;
}

class SVOutputInfoListClass;
#pragma endregion Declarations

namespace SvOi
{
	//************************************
	/// Get the Root child object list from a path and specified filter.
	/// \param rObjectNameList [out] The returned object name list.
	/// \param Path [in] Path of the root child desired. Default = "", this means all objects.
	/// \param AttributesAllowedFilter [in] Filter of the environment which are wanted. Default = 0, this means all objects.
	//************************************
	void getRootChildNameList(SvDef::StringVector& rObjectNameList, LPCTSTR Path = _T(""), UINT AttributesAllowedFilter = 0);

	//************************************
	/// Get the Root child selector list from a path and specified filter.
	/// \param Path [in] Path of the root child desired. Default = "", this means all objects.
	/// \param AttributesAllowedFilter [in] Filter of the environment which are wanted. Default = 0, this means all objects.
	/// \returns the vector of tree items
	//************************************
	std::vector<SvPb::TreeItem> getRootChildSelectorList(LPCTSTR Path = _T(""), UINT AttributesAllowedFilter = 0);
} //namespace SvOi
