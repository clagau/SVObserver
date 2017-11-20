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
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVObjectLibrary/SVClsIds.h"
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVEquationObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVConditionalObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// Register Embedded Objects
	RegisterEmbeddedObject( &result, SVConditionalResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	result.SetDefaultValue( BOOL(false), true );
	result.setSaveValueFlag(false);

	// Set default inputs and outputs
	addDefaultInputObjects();
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVConditionalClass::~SVConditionalClass()
{
}

bool SVConditionalClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
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
bool SVConditionalClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	BOOL Value = false;

	// Bypass all conditional execution if we are performing an internal run
	// This is used to initially setup certain objects.
	if( SVSVIMStateClass::CheckState( SV_STATE_INTERNAL_RUN ) )
	{
		result.SetValue(BOOL(true));
		return true;
	}// end if

	bool retVal = __super::onRun( rRunStatus, pErrorMessages );
	if( !retVal )
	{
		SetInvalid();
		rRunStatus.SetInvalid();
	}
	else
	{
		Value = true;
		if( HasCondition() && IsEnabled() )
		{
            Value = getResult() ? true : false;
		}
	}
	result.SetValue(Value);

	return retVal;
}

