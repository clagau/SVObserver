//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "InspectionEngine/RunStatus.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVConditional, SvPb::ConditionalClassId);

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVConditional::SVConditional( SVObjectClass* POwner, int StringResourceID ) 
: SVEquation( POwner, StringResourceID, true )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVConditional
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
void SVConditional::init()
{
	// Identify our output type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVEquationObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVConditionalObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// Register Embedded Objects
	RegisterEmbeddedObject( &result, SvPb::ConditionalResultEId, IDS_OBJECTNAME_RESULT, false, SvOi::SVResetItemNone, false );

	// Set Embedded defaults
	result.SetDefaultValue( BOOL(false), true );
	result.setSaveValueFlag(false);
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVConditional::~SVConditional()
{
}

bool SVConditional::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVEquation::CreateObject(rCreateStructure);

	// Set/Reset printable Flags
	result.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
// If Conditional is disabled conditional.Run() returns always TRUE.
// Otherwise the return value depends on the Conditional equation result!
bool SVConditional::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
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

} //namespace SvOp
