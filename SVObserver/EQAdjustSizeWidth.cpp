//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for Formula for Windows Width for ToolsSize Adjust 
//******************************************************************************


#pragma region Includes
#include "stdafx.h"
#include "EQAdjustSizeWidth.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( EQAdjustSizeWidth, EQAdjustSizeWidthGuid );

EQAdjustSizeWidth::EQAdjustSizeWidth( SVObjectClass* POwner, int StringResourceID )
: EQAdjustSize( EQSizeWidthType, EQAdjustSizeWidthResultGuid, POwner, StringResourceID )
{
}

EQAdjustSizeWidth::~EQAdjustSizeWidth()
{
}
