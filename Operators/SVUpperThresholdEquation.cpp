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
#include "SVObjectLibrary/SVClsIds.h"
#include "SVStatusLibrary/SVRunStatus.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVUpperThresholdEquationClass, SvPb::UpperThresholdEquationClassId);

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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVEquationObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType	= SvPb::SVUpperThresholdEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// Register Embedded Objects
	RegisterEmbeddedObject( &result, SVUpperThresholdEquationResultObjectGuid, IDS_OBJECTNAME_UPPER_THRESHOLD_RESULT, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	result.SetDefaultValue( 0.0, true );
	result.setSaveValueFlag(false);

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
bool SVUpperThresholdEquationClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
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


bool SVUpperThresholdEquationClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVEquationClass::CreateObject(rCreateStructure);

	// Set/Reset printable Flags
	result.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	return bOk;
}

} //namespace SvOp
