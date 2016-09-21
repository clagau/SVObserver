//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVFilterClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 11:58:30  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVFilterClass.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVFilterClass, SVFilterClassGuid )

SVFilterClass::SVFilterClass( SVObjectClass* POwner, int StringResourceID )
: SVUnaryImageOperatorClass( POwner, StringResourceID )
{
	init();
}

SVFilterClass::~SVFilterClass()
{
}

BOOL SVFilterClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL bOk = SVUnaryImageOperatorClass::CreateObject( pCreateStructure );

	//hide valid, status and color for the result picker.
	m_isObjectValid.ObjectAttributesAllowedRef() |= SV_HIDDEN;
	m_statusTag.ObjectAttributesAllowedRef() |= SV_HIDDEN;
	m_statusColor.ObjectAttributesAllowedRef() |= SV_HIDDEN;

	return bOk;
}

void SVFilterClass::init()
{
	// Identify our output type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;

	// Register Embedded Object(s)

	// Set Embedded defaults
	
	// Set default inputs and outputs
}

