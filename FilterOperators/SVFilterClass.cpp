//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
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

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS( SVFilterClass, SVFilterClassGuid )



SVFilterClass::SVFilterClass( SVObjectClass* POwner, int StringResourceID )
: SVUnaryImageOperatorClass( POwner, StringResourceID )
{
	init();
}

SVFilterClass::~SVFilterClass()
{
}

bool SVFilterClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVUnaryImageOperatorClass::CreateObject(rCreateStructure);

	//hide valid, status and color for the result picker.
	m_statusTag.SetObjectAttributesAllowed(SvDef::selectableAttributes, SvOi::SetAttributeType::RemoveAttribute);
	m_statusColor.SetObjectAttributesAllowed(SvDef::selectableAttributes, SvOi::SetAttributeType::RemoveAttribute);

	return bOk;
}

void SVFilterClass::init()
{
	// Identify our output type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;

	// Register Embedded Object(s)

	// Set Embedded defaults
	
	// Set default inputs and outputs
}

