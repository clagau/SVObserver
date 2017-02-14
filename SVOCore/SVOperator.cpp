//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOperator
//* .File Name       : $Workfile:   SVOperator.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   15 May 2014 11:21:48  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOperator.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVOperatorClass, SVOperatorClassGuid )

SVOperatorClass::SVOperatorClass( SVObjectClass* POwner, int StringResourceID )
				:SVTaskObjectClass( POwner, StringResourceID )
{
	init();
}

SVOperatorClass::~SVOperatorClass()
{
}

BOOL SVOperatorClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = false;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) && nullptr != GetTool() )
	{
		bOk = true;
	}

	m_isCreated = bOk;

	return bOk;
}

void SVOperatorClass::init()
{
	m_bUseOverlays = false;	// in general, operators don't have overlays

	// Identify our output type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVOperatorObjectType;

	// Register Embedded Object(s)

	// Set Embedded defaults

	// Set default inputs and outputs
	addDefaultInputObjects();
}

