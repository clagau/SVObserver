//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUpperThresholdEquation.cpp
//* .File Name       : $Workfile:   SVUpperThresholdEquation.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:56:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVUpperThresholdEquation.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVUpperThresholdEquationClass, SVUpperThresholdEquationClassGuid );

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVUpperThresholdEquationClass::SVUpperThresholdEquationClass( SVObjectClass* POwner, int StringResourceID )
							  :SVEquationClass( POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVUpperThresholdEquationClass
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void SVUpperThresholdEquationClass::init()
{
	// Identify our output type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVEquationObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType	= SVUpperThresholdEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// Register Embedded Objects
	RegisterEmbeddedObject( &result, SVUpperThresholdEquationResultObjectGuid, IDS_OBJECTNAME_UPPER_THRESHOLD_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	result.SetDefaultValue( 0.0, true );

	// Set default inputs and outputs
	addDefaultInputObjects();
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVUpperThresholdEquationClass::~SVUpperThresholdEquationClass()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
// If Conditional is disabled conditional.Run() returns always TRUE.
// Otherwise the return value depends on the Conditional equation result!
bool SVUpperThresholdEquationClass::onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	double value = 0.0;

	bool retVal = __super::onRun( RRunStatus, pErrorMessages );
	
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


BOOL SVUpperThresholdEquationClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVEquationClass::CreateObject( PCreateStructure );

	// Set/Reset printable Flags
	result.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	return bOk;
}

