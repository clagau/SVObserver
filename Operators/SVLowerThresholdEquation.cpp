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

SV_IMPLEMENT_CLASS( SVLowerThresholdEquation, SvPb::LowerThresholdEquationClassId);

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVLowerThresholdEquation::SVLowerThresholdEquation( SVObjectClass* POwner, int StringResourceID )
							  :SVEquation( POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVLowerThresholdEquation
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void SVLowerThresholdEquation::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVEquationObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType	= SvPb::SVLowerThresholdEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// Register Embedded Objects
	RegisterEmbeddedObject( &result, SvPb::LowerThresholdEquationResultEId, IDS_OBJECTNAME_LOWER_THRESHOLD_RESULT, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	result.SetDefaultValue( 0.0 );
	result.setSaveValueFlag(false);

	// Set default inputs and outputs
	addDefaultInputObjects();
}

bool SVLowerThresholdEquation::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVEquation::CreateObject(rCreateStructure);

	// Set / Reset Printable Flag
	result.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}
////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVLowerThresholdEquation::~SVLowerThresholdEquation()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
// If Conditional is disabled conditional.Run() returns always TRUE.
// Otherwise the return value depends on the Conditional equation result!
bool SVLowerThresholdEquation::onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
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

} //namespace SvOp
