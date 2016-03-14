//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVUnaryOperatorClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 12:04:40  $
//******************************************************************************

#include "stdafx.h"
#include "SVUnaryOperatorClass.h"

SV_IMPLEMENT_CLASS( SVUnaryOperatorClass, SVUnaryOperatorClassGuid )

SVUnaryOperatorClass::SVUnaryOperatorClass( SVObjectClass* POwner, int StringResourceID )
					 :SVOperatorClass( POwner, StringResourceID )
{
	init();
}

void SVUnaryOperatorClass::init()
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryOperatorObjectType;

	// Register Embedded Object(s)

	// Set Embedded defaults

	// Set default inputs and outputs
	addDefaultInputObjects();
}

