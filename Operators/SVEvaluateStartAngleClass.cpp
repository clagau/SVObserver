//******************************************************************************
/// COPYRIGHT (c) 2016 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Class for Start Angle Evaluation
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEvaluateStartAngleClass.h"
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

SV_IMPLEMENT_CLASS( SVEvaluateStartAngleClass, SvPb::EvaluateStartAngleClassId)

SVEvaluateStartAngleClass::SVEvaluateStartAngleClass( SVObjectClass* pOwner, int StringResourceID )
: SVEvaluate( pOwner, StringResourceID )
{
	// Identify yourself
	m_ObjectTypeInfo.m_SubType = SvPb::SVEvaluateStartAngleObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputMathResult, SvPb::OutputEvaluateStartAngleResultEId, IDS_OBJECTNAME_RESULT, false, SvOi::SVResetItemNone, false);

	// Set Embedded defaults
	m_outputMathResult.SetDefaultValue( 0.0, true );
}

SVEvaluateStartAngleClass::~SVEvaluateStartAngleClass()
{
}

} //namespace SvOp
