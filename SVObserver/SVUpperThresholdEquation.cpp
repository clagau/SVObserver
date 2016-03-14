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

#include "stdafx.h"
#include "SVUpperThresholdEquation.h"


//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************


//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVUpperThresholdEquationClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************
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
// .History
//	 Date		Author		Comment                                       
//  :26.08.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVUpperThresholdEquationClass::init()
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVEquationObjectType;
	outObjectInfo.ObjectTypeInfo.SubType	= SVUpperThresholdEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// Register Embedded Objects
	RegisterEmbeddedObject( &result, SVUpperThresholdEquationResultObjectGuid, IDS_OBJECTNAME_UPPER_THRESHOLD_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	result.SetDefaultValue( 0.0, TRUE );

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
BOOL SVUpperThresholdEquationClass::OnValidate()
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
BOOL SVUpperThresholdEquationClass::onRun( SVRunStatusClass& RRunStatus )
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


BOOL SVUpperThresholdEquationClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVEquationClass::CreateObject( PCreateStructure );

	// Set/Reset printable Flags
	result.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	return bOk;
}

