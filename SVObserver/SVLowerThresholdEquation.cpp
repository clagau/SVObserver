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
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVEquationObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType	= SVLowerThresholdEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// Register Embedded Objects
	RegisterEmbeddedObject( &result, SVLowerThresholdEquationResultObjectGuid, IDS_OBJECTNAME_LOWER_THRESHOLD_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	result.SetDefaultValue( 0.0, TRUE );

	// Set default inputs and outputs
	addDefaultInputObjects();
}

BOOL SVLowerThresholdEquationClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	m_isCreated = SVEquationClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flag
	result.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

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
BOOL SVLowerThresholdEquationClass::OnValidate()
{
	BOOL retVal = TRUE;
	
	// validate our outputs
	retVal = SVEquationClass::OnValidate() && retVal;

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
// If Conditional is disabled conditional.Run() returns always TRUE.
// Otherwise the return value depends on the Conditional equation result!
BOOL SVLowerThresholdEquationClass::onRun( SVRunStatusClass& RRunStatus )
{
	double value = 0.0;

	BOOL retVal = SVEquationClass::onRun( RRunStatus );
	
	if( !retVal )
	{
		SetInvalid();
		RRunStatus.SetInvalid();
	}
	else
	{
		if( HasCondition() && IsEnabled() )
		{
			value = yacc.equationResult;
		}
	}
	result.SetValue( RRunStatus.m_lResultDataIndex, value );

	return retVal;
}

