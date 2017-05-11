//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for Formula for Windows PositionY for ToolsSize Adjust 
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "EQAdjustSizePositionY.h"
#include "SVObjectLibrary\SVClsids.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( EQAdjustSizePositionY, EQAdjustSizePositionYGuid );

EQAdjustSizePositionY::EQAdjustSizePositionY( SVObjectClass* POwner, int StringResourceID )
: EQAdjustSize(EQSizePositionYType, EQAdjustSizePositionYResultGuid, POwner, StringResourceID )
{
}

EQAdjustSizePositionY::~EQAdjustSizePositionY()
{
}
