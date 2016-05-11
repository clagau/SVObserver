//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for Center Y Evaluation
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEvaluateCenterYClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVEvaluateCenterYClass, SVEvaluateCenterYClassGuid )

SVEvaluateCenterYClass::SVEvaluateCenterYClass( SVObjectClass* pOwner, int StringResourceID )
: SVEvaluateClass( pOwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateCenterYObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputMathResult, SVOutputEvaluateCenterYResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	m_outputMathResult.SetDefaultValue( 0.0, true );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVEvaluateCenterYClass::~SVEvaluateCenterYClass()
{
}
