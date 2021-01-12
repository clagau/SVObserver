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
#include "SVStatusLibrary/RunStatus.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVUpperThresholdEquation, SvPb::UpperThresholdEquationClassId);

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVUpperThresholdEquation::SVUpperThresholdEquation( SVObjectClass* POwner, int StringResourceID )
							  :SVEquation( POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVUpperThresholdEquation
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void SVUpperThresholdEquation::init()
{
	// Identify our output type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVEquationObjectType;
	m_ObjectTypeInfo.m_SubType	= SvPb::SVUpperThresholdEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// Register Embedded Objects
	RegisterEmbeddedObject( &result, SvPb::UpperThresholdEquationResultEId, IDS_OBJECTNAME_UPPER_THRESHOLD_RESULT, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	result.SetDefaultValue( 0.0, true );
	result.setSaveValueFlag(false);
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVUpperThresholdEquation::~SVUpperThresholdEquation()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
// If Conditional is disabled conditional.Run() returns always TRUE.
// Otherwise the return value depends on the Conditional equation result!
bool SVUpperThresholdEquation::onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	double value = 0.0;

	bool retVal = __super::onRun( rRunStatus, pErrorMessages );
	
	if( !retVal )
	{
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


bool SVUpperThresholdEquation::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVEquation::CreateObject(rCreateStructure);

	// Set/Reset printable Flags
	result.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	return bOk;
}

} //namespace SvOp
