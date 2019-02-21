//******************************************************************************
/// \copyright (c) 2016 by Seidenader Maschinenbau GmbH
/// \file TableColumnEquation.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for the column formula in table tool.
//******************************************************************************

#include "stdafx.h"
#include "TableColumnEquation.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVObjectLibrary/SVClsids.h"

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( TableColumnEquation, TableColumnEquationGuid );

TableColumnEquation::TableColumnEquation( SVObjectClass* pOwner, int StringResourceID ) 
: SVEquationClass( pOwner, StringResourceID )
{
	init();
}

TableColumnEquation::~TableColumnEquation()
{
}

void TableColumnEquation::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVEquationObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::TableColumnEquationObjectType;

	// Set default inputs and outputs
	addDefaultInputObjects();
}

} //namespace SvOp