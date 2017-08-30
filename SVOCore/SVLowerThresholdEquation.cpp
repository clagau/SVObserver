//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLowerThresholdEquation.cpp
//* .File Name       : $Workfile:   SVLowerThresholdEquation.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:18:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLowerThresholdEquation.h"
#include "SVObjectLibrary\SVClsIds.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVLowerThresholdEquationClass, SVLowerThresholdEquationClassGuid );

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVLowerThresholdEquationClass::SVLowerThresholdEquationClass( SVObjectClass* POwner, int StringResourceID )
							  :SVEquationClass( POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVLowerThresholdEquationClass
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void SVLowerThresholdEquationClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVEquationObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType	= SVLowerThresholdEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// Register Embedded Objects
	RegisterEmbeddedObject( &result, SVLowerThresholdEquationResultObjectGuid, IDS_OBJECTNAME_LOWER_THRESHOLD_RESULT, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	result.SetDefaultValue( 0.0, TRUE );
	result.setSaveValueFlag(false);

	// Set default inputs and outputs
	addDefaultInputObjects();
}

bool SVLowerThresholdEquationClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	m_isCreated = SVEquationClass::CreateObject( pCreateStructure );

	// Set / Reset Printable Flag
	result.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}
////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVLowerThresholdEquationClass::~SVLowerThresholdEquationClass()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
// If Conditional is disabled conditional.Run() returns always TRUE.
// Otherwise the return value depends on the Conditional equation result!
bool SVLowerThresholdEquationClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	double value = 0.0;

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
			value = getResult();
		}
	}
	result.SetValue(value);

	return retVal;
}

