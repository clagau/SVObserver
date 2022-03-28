//*****************************************************************************
/// \copyright (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
/// \file ObjectUtilities.cpp
//*****************************************************************************
/// This file contains free utility functions for object classes.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObjectClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvOl
{
std::pair<std::string, std::string> createPrefixNameForEquation(const SVObjectClass& rRenamedObject, const std::string& rOldName)
{
	std::pair<std::string, std::string> prefixName; //new, old

	SvPb::SVObjectTypeEnum type = rRenamedObject.GetObjectType();
	if (SvPb::SVInspectionObjectType == type)
	{
		//InspectionName is not part of Equation, so nothing to do.
		return {};
	}
	else if (SvPb::SVBasicValueObjectType == type)
	{
		std::string dottedNameWithoutObjectname = rRenamedObject.GetObjectNameToObjectType(SvPb::SVRootObjectType, false);
		prefixName.first = _T("\"") + dottedNameWithoutObjectname + _T(".") + rRenamedObject.GetName() + _T("\"");
		prefixName.second = _T("\"") + dottedNameWithoutObjectname + _T(".") + rOldName + _T("\"");
	}
	else if (SvPb::SVValueObjectType == type)
	{
		std::string dottedNameWithoutObjectname = rRenamedObject.GetObjectNameToObjectType(SvPb::SVToolSetObjectType, false);
		prefixName.first = _T("\"") + dottedNameWithoutObjectname + _T(".") + rRenamedObject.GetName() + _T("\"");
		prefixName.second = _T("\"") + dottedNameWithoutObjectname + _T(".") + rOldName + _T("\"");
	}
	else
	{
		std::string dottedNameWithoutObjectname = rRenamedObject.GetObjectNameToObjectType(SvPb::SVToolSetObjectType, false);
		prefixName.first = _T("\"") + dottedNameWithoutObjectname + _T(".") + rRenamedObject.GetName() + _T(".");
		prefixName.second = _T("\"") + dottedNameWithoutObjectname + _T(".") + rOldName + _T(".");
	}

	return prefixName;
}
}