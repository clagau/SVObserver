//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for Formula for Windows width for ToolsSize Adjust 
//******************************************************************************
#pragma once
 
#pragma region Includes
#include "EQAdjustSize.h"
#pragma endregion Includes
 

//! \brief Equation class for Formula for Windows Width for ToolsSize Adjust

class EQAdjustSizeWidth : public EQAdjustSize
{

	SV_DECLARE_CLASS( EQAdjustSizeWidth);
public:
	EQAdjustSizeWidth( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_EQ_ADJUSTSIZE_WIDTH );
	
}	;
