//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
///Equation class for Formula for Windows X Position  for ToolsSize Adjust 
//******************************************************************************
#pragma once
 
#pragma region Includes
#include "EQAdjustSize.h"
#pragma endregion Includes

//! \brief Equation class for Formula for Windows X Position  for ToolsSize Adjust

class EQAdjustSizePositionX : public EQAdjustSize
{
	SV_DECLARE_CLASS( EQAdjustSizePositionX);
public:
	EQAdjustSizePositionX( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_EQ_ADJUSTSIZE_POSITION_X );
	virtual ~EQAdjustSizePositionX();
};
