//******************************************************************************
/// COPYRIGHT (c) 2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Class for End Angle Evaluation
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEvaluateEndAngleClass.h"
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

SV_IMPLEMENT_CLASS( SVEvaluateEndAngleClass, SvPb::EvaluateEndAngleClassId)

SVEvaluateEndAngleClass::SVEvaluateEndAngleClass( SVObjectClass* pOwner, int StringResourceID )
: SVEvaluate( pOwner, StringResourceID )
{
	// Identify yourself
	m_ObjectTypeInfo.m_SubType = SvPb::SVEvaluateEndAngleObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputMathResult, SvPb::OutputEvaluateEndAngleResultEId, IDS_OBJECTNAME_RESULT, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_outputMathResult.SetDefaultValue( 0.0, true );
}

SVEvaluateEndAngleClass::~SVEvaluateEndAngleClass()
{
}

} //namespace SvOp
