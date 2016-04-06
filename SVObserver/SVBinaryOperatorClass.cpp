//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVBinaryOperatorClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 11:56:24  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVBinaryOperatorClass.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVBinaryOperatorClass, SVBinaryOperatorClassGuid )

SVBinaryOperatorClass::SVBinaryOperatorClass( SVObjectClass* POwner, int StringResourceID )
					  :SVOperatorClass( POwner, StringResourceID )
{
	init();
}

SVBinaryOperatorClass::~SVBinaryOperatorClass()
{
}

void SVBinaryOperatorClass::init()
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVBinaryOperatorObjectType;

	// Register Embedded Object(s)

	// Set Embedded defaults

	// Set default inputs and outputs
}

