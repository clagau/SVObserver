//******************************************************************************
/// \copyright (c) 2016 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file TableColumnEquation.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for the column formula in table tool.
//******************************************************************************

#include "stdafx.h"
#include "TableColumnEquation.h"

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( TableColumnEquation, SvPb::TableColumnEquationId);

TableColumnEquation::TableColumnEquation( SVObjectClass* pOwner, int StringResourceID ) 
: SVEquation( pOwner, StringResourceID )
{
	init();
}

TableColumnEquation::~TableColumnEquation()
{
}

void TableColumnEquation::init()
{
	// Identify our output type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVEquationObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::TableColumnEquationObjectType;
}

} //namespace SvOp
