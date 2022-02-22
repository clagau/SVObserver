//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for Formula for Windows PositionX for ToolsSize Adjust 
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "EQAdjustSizePositionX.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( EQAdjustSizePositionX, SvPb::EQAdjustSizePositionXClassId);

EQAdjustSizePositionX::EQAdjustSizePositionX( SVObjectClass* POwner, int StringResourceID )
: EQAdjustSize(SvPb::EQSizePositionXType, SvPb::EQAdjustSizePositionXResultEId, POwner, StringResourceID )
{
}

EQAdjustSizePositionX::~EQAdjustSizePositionX()
{
}

} //namespace SvOp
