//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for Rotation Angle Evaluation
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEvaluateRotationAngleClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVEvaluateRotationAngleClass, SVEvaluateRotationAngleClassGuid )

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
SVEvaluateRotationAngleClass::SVEvaluateRotationAngleClass( SVObjectClass* pOwner, int StringResourceID )
: SVEvaluateClass( pOwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateRotationAngleObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputMathResult, SVOutputEvaluateRotationAngleResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	m_outputMathResult.SetDefaultValue( 0.0, true );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVEvaluateRotationAngleClass::~SVEvaluateRotationAngleClass()
{
}
