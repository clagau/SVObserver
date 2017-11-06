//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAutoThresholdEquation.cpp
//* .File Name       : $Workfile:   SVAutoThresholdEquation.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:32:14  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"

#include "SVAutoThresholdEquation.h"
#include "SVObjectLibrary\SVClsIds.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVAutoThresholdEquationClass, SVAutoThresholdEquationClassGuid );

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVAutoThresholdEquationClass::SVAutoThresholdEquationClass( SVObjectClass* POwner, int StringResourceID )
							  :SVEquationClass( POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVAutoThresholdEquationClass
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void SVAutoThresholdEquationClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVEquationObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType	= SVAutoThresholdEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// Register Embedded Objects
	RegisterEmbeddedObject( &result, SVAutoThresholdEquationResultObjectGuid, IDS_OBJECTNAME_AUTO_THRESHOLD_RESULT, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	result.SetDefaultValue( 0.0 );
	result.setSaveValueFlag(false);

	// Set default inputs and outputs
	addDefaultInputObjects();
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVAutoThresholdEquationClass::~SVAutoThresholdEquationClass()
{
}


bool SVAutoThresholdEquationClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVEquationClass::CreateObject(rCreateStructure);

	// Set/Reset printable Flags
	result.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
// If Conditional is disabled conditional.Run() returns always TRUE.
// Otherwise the return value depends on the Conditional equation result!
bool SVAutoThresholdEquationClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	double Value = 0.0;

	bool retVal = __super::onRun( rRunStatus, pErrorMessages );
	
	if( !retVal )
	{
		SetInvalid();
		rRunStatus.SetInvalid();
	}
	else
	{
		if( HasCondition() && IsEnabled() )
		{
			Value = getResult();
		}
	}
	result.SetValue(Value);

	return retVal;
}





