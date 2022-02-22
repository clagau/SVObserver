//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for Formula for Windows PositionY for ToolsSize Adjust 
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "EQAdjustSizePositionY.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( EQAdjustSizePositionY, SvPb::EQAdjustSizePositionYClassId);

EQAdjustSizePositionY::EQAdjustSizePositionY( SVObjectClass* POwner, int StringResourceID )
: EQAdjustSize(SvPb::EQSizePositionYType, SvPb::EQAdjustSizePositionYResultEId, POwner, StringResourceID )
{
}

EQAdjustSizePositionY::~EQAdjustSizePositionY()
{
}

} //namespace SvOp
