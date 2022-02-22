//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for Formula for Windows Height for ToolsSize Adjust 
//******************************************************************************


#pragma region Includes
#include "stdafx.h"
#include "EQAdjustSizeHeight.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( EQAdjustSizeHeight, SvPb::EQAdjustSizeHeightClassId);

EQAdjustSizeHeight::EQAdjustSizeHeight( SVObjectClass* POwner, int StringResourceID )
: EQAdjustSize(SvPb::EQSizeHeightType, SvPb::EQAdjustSizeHeightResultEId, POwner, StringResourceID )
{
}

EQAdjustSizeHeight::~EQAdjustSizeHeight()
{
}

} //namespace SvOp
