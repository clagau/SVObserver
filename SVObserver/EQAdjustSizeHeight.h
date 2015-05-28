//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for Formula for Windows Height for ToolsSize Adjust 
//******************************************************************************
#pragma once
 
#pragma region Includes
#include "SVEquation.h"
#include "EQAdjustSize.h"
#pragma endregion Includes

 
#pragma once
 
//! \brief Equation class for Formula for Windows Height for ToolsSize Adjust 
class EQAdjustSizeHeight : public EQAdjustSize
{

	SV_DECLARE_CLASS( EQAdjustSizeHeight);
public:
	EQAdjustSizeHeight( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_EQ_ADJUSTSIZE_HEIGHT );
};
 