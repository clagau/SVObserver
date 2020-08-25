//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAutoThresholdEquation.cpp
//* .File Name       : $Workfile:   SVAutoThresholdEquation.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:32:14  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"

#include "SVAutoThresholdEquation.h"
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


SV_IMPLEMENT_CLASS( SVAutoThresholdEquation, SvPb::AutoThresholdEquationClassId);

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVAutoThresholdEquation::SVAutoThresholdEquation( SVObjectClass* POwner, int StringResourceID )
							  :SVEquation( POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVAutoThresholdEquation
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void SVAutoThresholdEquation::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVEquationObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType	= SvPb::SVAutoThresholdEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// Register Embedded Objects
	RegisterEmbeddedObject( &result, SvPb::AutoThresholdEquationResultEId, IDS_OBJECTNAME_AUTO_THRESHOLD_RESULT, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	result.SetDefaultValue( 0.0 );
	result.setSaveValueFlag(false);

	// Set default inputs and outputs
	addDefaultInputObjects();
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
SVAutoThresholdEquation::~SVAutoThresholdEquation()
{
}


bool SVAutoThresholdEquation::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVEquation::CreateObject(rCreateStructure);

	// Set/Reset printable Flags
	result.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
// If Conditional is disabled conditional.Run() returns always TRUE.
// Otherwise the return value depends on the Conditional equation result!
bool SVAutoThresholdEquation::onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	double Value = 0.0;

	bool retVal = __super::onRun( rRunStatus, pErrorMessages );
	
	if( !retVal )
	{
		rRunStatus.SetInvalid();
	}
	else
	{
		if( HasCondition() && IsEnabled() )
		{
			Value = getResult();
		}
	}
	result.SetValue(Value);

	return retVal;
}

} //namespace SvOp
