//*****************************************************************************
/// \copyright (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ModuleTool.h
/// All Rights Reserved 
//*****************************************************************************
/// Implements the ModuleTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "GroupTool.h"
#pragma endregion Includes


namespace SvTo
{

/// Implements the GroupTool
class ModuleTool : public GroupTool
{
	SV_DECLARE_CLASS
public:
	ModuleTool(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_MODULETOOL);
	virtual ~ModuleTool();

	void MoveObjectToThis(GroupTool& rGroupTool);
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;

	virtual std::vector<std::string> getToolAdjustNameList() const override;

private:
	void Initialize();

private:
	SvVol::SVStringValueObjectClass m_moduleComment;
};

} //namespace SvTo
