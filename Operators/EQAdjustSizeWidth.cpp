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

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( EQAdjustSizeWidth, SvPb::EQAdjustSizeWidthClassId);

EQAdjustSizeWidth::EQAdjustSizeWidth( SVObjectClass* POwner, int StringResourceID )
: EQAdjustSize(SvPb::EQSizeWidthType, SvPb::EQAdjustSizeWidthResultEId, POwner, StringResourceID )
{
}

EQAdjustSizeWidth::~EQAdjustSizeWidth()
{
}

} //namespace SvOp
