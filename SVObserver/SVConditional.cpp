//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditional.cpp
//* .File Name       : $Workfile:   SVConditional.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:59:26  $
//******************************************************************************

#include "stdafx.h"
#include "SVConditional.h"
#include "SVSVIMStateClass.h"


//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************


//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVConditionalClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************
SV_IMPLEMENT_CLASS( SVConditionalClass, SVConditionalClassGuid );

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVConditionalClass::SVConditionalClass( SVObjectClass* POwner, int StringResourceID ) 
: SVEquationClass( POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVConditionalClass
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :26.08.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVConditionalClass::init()
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVEquationObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVConditionalObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// SetObjectDepth() already called in SVObjectClass Ctor

	// Register Embedded Objects
	RegisterEmbeddedObject( &result, SVConditionalResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	result.SetDefaultValue( FALSE, TRUE );

	// Set default inputs and outputs
	addDefaultInputObjects();
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVConditionalClass::~SVConditionalClass()
{
}



BOOL SVConditionalClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	isCreated = SVEquationClass::CreateObject( PCreateStructure );

	// Set/Reset printable Flags
	result.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	return isCreated;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
BOOL SVConditionalClass::OnValidate()
{
	BOOL retVal = TRUE;
	
	// validate our outputs
	retVal = SVEquationClass::OnValidate() && retVal;

	if( !retVal )
		SetInvalid();

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
// If Conditional is disabled conditional.Run() returns always TRUE.
// Otherwise the return value depends on the Conditional equation result!
BOOL SVConditionalClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL value = FALSE;

	// Bypass all conditional execution if we are performing an internal run
	// This is used to initially setup certain objects.
	if( SVSVIMStateClass::CheckState( SV_STATE_INTERNAL_RUN ) )
	{
		result.SetValue( RRunStatus.m_lResultDataIndex, 1L );
		return TRUE;
	}// end if

	BOOL retVal = SVEquationClass::onRun( RRunStatus );
	if( !retVal )
	{
		SetInvalid();
		RRunStatus.SetInvalid();
	}
	else
	{
		value = TRUE;
		if( HasCondition() && IsEnabled() )
		{
			//value = ( BOOL )yacc.equationResult;  08 Feb 2000 - frb.
            value = yacc.equationResult ? TRUE : FALSE;
		}
	}
	result.SetValue( RRunStatus.m_lResultDataIndex, value );

	return retVal;
}

