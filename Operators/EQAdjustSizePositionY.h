//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
///Equation class for Formula for Windows y Position  for ToolsSize Adjust
//******************************************************************************
#pragma once
 
#pragma region Includes
#include "EQAdjustSize.h"
#pragma endregion Includes

namespace SvOp
{

//! \brief Equation class for Formula for Windows y Position  for ToolsSize Adjust
class EQAdjustSizePositionY : public EQAdjustSize
{
	SV_DECLARE_CLASS
public:
	EQAdjustSizePositionY( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_EQ_ADJUSTSIZE_POSITION_Y );
	virtual ~EQAdjustSizePositionY();
};

} //namespace SvOp
