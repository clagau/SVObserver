//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for Formula for Windows width for ToolsSize Adjust 
//******************************************************************************
#pragma once
 
#pragma region Includes
#include "EQAdjustSize.h"
#pragma endregion Includes
 

namespace SvOp
{
//! \brief Equation class for Formula for Windows Width for ToolsSize Adjust

class EQAdjustSizeWidth : public EQAdjustSize
{
	SV_DECLARE_CLASS
public:
	EQAdjustSizeWidth( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_EQ_ADJUSTSIZE_WIDTH );
	virtual ~EQAdjustSizeWidth();
};

} //namespace SvOp
