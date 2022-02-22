//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
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


	/// Fill the treeItem-vector with objects, which fit the requirements.
	/// \param treeInserter [in,out] inserter
	/// \param Path [in] Start path, "" means all.
	/// \param AttributesAllowedFilter [in] attribute filter
	/// \param type [in] required type of the object.
	void fillRootChildSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, LPCTSTR Path = _T(""), UINT AttributesAllowedFilter = 0, SvPb::ObjectSelectorType type = SvPb::allValueObjects);
} //namespace SvOi
