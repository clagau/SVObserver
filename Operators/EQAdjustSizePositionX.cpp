//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for Formula for Windows PositionX for ToolsSize Adjust 
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "EQAdjustSizePositionX.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( EQAdjustSizePositionX, EQAdjustSizePositionXGuid );

EQAdjustSizePositionX::EQAdjustSizePositionX( SVObjectClass* POwner, int StringResourceID )
: EQAdjustSize(SvPb::EQSizePositionXType,EQAdjustSizePositionXResultGuid , POwner, StringResourceID )
{
}

EQAdjustSizePositionX::~EQAdjustSizePositionX()
{
}

} //namespace SvOp