//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMathEquation.cpp
//* .File Name       : $Workfile:   SVMathEquation.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:29:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVMathEquation.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVMathEquationClass, SVMathEquationClassGuid );

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVMathEquationClass::SVMathEquationClass( SVObjectClass* POwner, int StringResourceID )
				  :SVEquationClass( POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVMathEquationClass
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void SVMathEquationClass::init()
{
	m_bUseOverlays = false;

	// Identify our output type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVEquationObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVMathEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// SetObjectDepth() already called in SVObjectClass Ctor

	// Register Embedded Objects
	RegisterEmbeddedObject( &result, SVMathEquationResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	result.SetDefaultValue( 0.0, TRUE );

	// Set default inputs and outputs
	addDefaultInputObjects();
}

BOOL SVMathEquationClass::CreateObject(SVObjectLevelCreateStruct *PCreateStruct)
{
	m_isCreated = SVEquationClass::CreateObject(PCreateStruct);

	// Set / Reset Printable Flag
	result.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	return m_isCreated;
}
////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVMathEquationClass::~SVMathEquationClass()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
// If Conditional is disabled conditional.Run() returns always TRUE.
// Otherwise the return value depends on the Conditional equation result!
BOOL SVMathEquationClass::onRun( SVRunStatusClass& RRunStatus )
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
