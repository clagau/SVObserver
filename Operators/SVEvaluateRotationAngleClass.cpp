//******************************************************************************
/// COPYRIGHT (c) 2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Class for Rotation Angle Evaluation
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEvaluateRotationAngleClass.h"
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

SV_IMPLEMENT_CLASS( SVEvaluateRotationAngleClass, SvPb::EvaluateRotationAngleClassId)

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
SVEvaluateRotationAngleClass::SVEvaluateRotationAngleClass( SVObjectClass* pOwner, int StringResourceID )
: SVEvaluate( pOwner, StringResourceID ) 
{
	// Identify yourself
	m_ObjectTypeInfo.m_SubType = SvPb::SVEvaluateRotationAngleObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputMathResult, SvPb::OutputEvaluateRotationAngleResultEId, IDS_OBJECTNAME_RESULT, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_outputMathResult.SetDefaultValue( 0.0, true );
}

SVEvaluateRotationAngleClass::~SVEvaluateRotationAngleClass()
{
}

} //namespace SvOp
