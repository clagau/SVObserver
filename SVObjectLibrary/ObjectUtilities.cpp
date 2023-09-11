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
std::string renameFormulaTextAfterRenameObject(const std::string& rFormula, const SVObjectClass& rRenamedObject, const std::string& rOldName)
{
	SvPb::SVObjectTypeEnum type = rRenamedObject.GetObjectType();
	if (SvPb::SVInspectionObjectType == type)
	{
		//InspectionName is not part of Equation, so nothing to do.
		return rFormula;
	}
	
	std::string formula = rFormula;
	if (SvPb::SVBasicValueObjectType == type)
	{
		std::string dottedNameWithoutObjectname = rRenamedObject.GetObjectNameToObjectType(SvPb::SVRootObjectType, false);
		std::string newPrefix = _T("\"") + dottedNameWithoutObjectname + _T(".") + rRenamedObject.GetName() + _T("\"");
		std::string oldPrefix = _T("\"") + dottedNameWithoutObjectname + _T(".") + rOldName + _T("\"");
		SvUl::searchAndReplace(formula, oldPrefix.c_str(), newPrefix.c_str());
	}
	else
	{
		std::string dottedNameWithoutObjectname = rRenamedObject.GetObjectNameToObjectType(SvPb::SVToolSetObjectType, false);
		std::string newPrefix = _T("\"") + dottedNameWithoutObjectname + _T(".") + rRenamedObject.GetName() + _T(".");
		std::string oldPrefix = _T("\"") + dottedNameWithoutObjectname + _T(".") + rOldName + _T(".");
		SvUl::searchAndReplace(formula, oldPrefix.c_str(), newPrefix.c_str());
		if (SvPb::SVValueObjectType == type)
		{
			newPrefix = _T("\"") + dottedNameWithoutObjectname + _T(".") + rRenamedObject.GetName() + _T("\"");
			oldPrefix = _T("\"") + dottedNameWithoutObjectname + _T(".") + rOldName + _T("\"");
			SvUl::searchAndReplace(formula, oldPrefix.c_str(), newPrefix.c_str());
		}
	}

	return formula;
}
}