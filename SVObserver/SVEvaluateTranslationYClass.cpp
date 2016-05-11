//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for Y Translation Evaluation
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEvaluateTranslationYClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVEvaluateTranslationYClass, SVEvaluateTranslationYClassGuid )

SVEvaluateTranslationYClass::SVEvaluateTranslationYClass( SVObjectClass* pOwner, int StringResourceID )
: SVEvaluateClass( pOwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateTranslationYObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputMathResult, SVOutputEvaluateTranslationYResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	m_outputMathResult.SetDefaultValue( 0.0, true );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVEvaluateTranslationYClass::~SVEvaluateTranslationYClass()
{
}
