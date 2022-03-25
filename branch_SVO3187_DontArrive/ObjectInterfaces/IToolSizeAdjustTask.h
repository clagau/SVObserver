//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for ToolSizeAdjustTask class
//******************************************************************************

#pragma once
#pragma region Includes
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvOi
{
//this class is a interface. It should only have pure virtual public method and no member variables
class IToolSizeAdjustTask
{
public:
	virtual ~IToolSizeAdjustTask() {}

	virtual bool IsFullSizeAllowed() const = 0;
	virtual bool IsAdjustSizeAllowed() const = 0;
	virtual bool IsAdjustPositionAllowed() const = 0;
	virtual ITool* getTool() const = 0;
};
} //namespace SvOi