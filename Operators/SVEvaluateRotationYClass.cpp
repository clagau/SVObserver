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

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVEvaluateRotationYClass, SvPb::EvaluateRotationYClassId)

SVEvaluateRotationYClass::SVEvaluateRotationYClass( SVObjectClass* pOwner, int StringResourceID )
: SVEvaluate( pOwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVEvaluateRotationYObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputMathResult, SvPb::OutputEvaluateRotationYResultEId, IDS_OBJECTNAME_RESULT, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_outputMathResult.SetDefaultValue( 0.0, true );
}

SVEvaluateRotationYClass::~SVEvaluateRotationYClass()
{
}

} //namespace SvOp
