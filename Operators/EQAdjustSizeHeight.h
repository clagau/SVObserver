//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for Formula for Windows Height for ToolsSize Adjust 
//******************************************************************************
#pragma once
 
#pragma region Includes
#include "EQAdjustSize.h"
#pragma endregion Includes

namespace SvOp
{

//! \brief Equation class for Formula for Windows Height for ToolsSize Adjust 
class EQAdjustSizeHeight : public EQAdjustSize
{

	SV_DECLARE_CLASS
public:
	EQAdjustSizeHeight( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_EQ_ADJUSTSIZE_HEIGHT );
	virtual ~EQAdjustSizeHeight();
};

} //namespace SvOp
