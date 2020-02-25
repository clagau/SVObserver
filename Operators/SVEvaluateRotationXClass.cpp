//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for X Rotation Evaluation
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEvaluateRotationXClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

SV_IMPLEMENT_CLASS( SVEvaluateRotationXClass, SvPb::EvaluateRotationXClassId)

SVEvaluateRotationXClass::SVEvaluateRotationXClass( SVObjectClass* POwner, int StringResourceID )
: SVEvaluateClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVEvaluateRotationXObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputMathResult, SvPb::OutputEvaluateRotationXResultEId, IDS_OBJECTNAME_RESULT, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_outputMathResult.SetDefaultValue( 0.0, true );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVEvaluateRotationXClass::~SVEvaluateRotationXClass()
{
}

} //namespace SvOp
