//******************************************************************************
/// \copyright (c) 2016 by Seidenader Maschinenbau GmbH
/// \file TableColumnEquation.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for the column formula in table tool.
//******************************************************************************

#include "stdafx.h"
#include "TableColumnEquation.h"
#include "SVSVIMStateClass.h"


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
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVEquationObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = TableColumnEquationObjectType;

	// Set default inputs and outputs
	addDefaultInputObjects();
}


