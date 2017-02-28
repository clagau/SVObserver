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

#pragma region Includes
#include "stdafx.h"
#include "SVConditional.h"
#include "SVSVIMStateClass.h"
#pragma endregion Includes

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
void SVConditionalClass::init()
{
	// Identify our output type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVEquationObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVConditionalObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// Register Embedded Objects
	RegisterEmbeddedObject( &result, SVConditionalResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	result.SetDefaultValue( false, true );

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
	m_isCreated = SVEquationClass::CreateObject( PCreateStructure );

	// Set/Reset printable Flags
	result.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	return m_isCreated;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
// If Conditional is disabled conditional.Run() returns always TRUE.
// Otherwise the return value depends on the Conditional equation result!
bool SVConditionalClass::onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	BOOL value = false;

	// Bypass all conditional execution if we are performing an internal run
	// This is used to initially setup certain objects.
	if( SVSVIMStateClass::CheckState( SV_STATE_INTERNAL_RUN ) )
	{
		result.SetValue( RRunStatus.m_lResultDataIndex, 1L );
		return true;
	}// end if

	bool retVal = __super::onRun( RRunStatus, pErrorMessages );
	if( !retVal )
	{
		SetInvalid();
		RRunStatus.SetInvalid();
	}
	else
	{
		value = true;
		if( HasCondition() && IsEnabled() )
		{
			value = yacc.equationResult ? true : false;
		}
	}
	result.SetValue( RRunStatus.m_lResultDataIndex, value );

	return retVal;
}

