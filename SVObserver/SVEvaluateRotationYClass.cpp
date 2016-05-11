//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for Y Rotation Evaluation
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEvaluateRotationYClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVEvaluateRotationYClass, SVEvaluateRotationYClassGuid )

SVEvaluateRotationYClass::SVEvaluateRotationYClass( SVObjectClass* pOwner, int StringResourceID )
: SVEvaluateClass( pOwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateRotationYObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputMathResult, SVOutputEvaluateRotationYResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	m_outputMathResult.SetDefaultValue( 0.0, true );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVEvaluateRotationYClass::~SVEvaluateRotationYClass()
{
}
