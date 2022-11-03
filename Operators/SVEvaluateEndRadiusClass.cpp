//******************************************************************************
/// COPYRIGHT (c) 2016 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Class for End Radius Evaluation
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEvaluateEndRadiusClass.h"
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

SV_IMPLEMENT_CLASS( SVEvaluateEndRadiusClass, SvPb::EvaluateEndRadiusClassId)

SVEvaluateEndRadiusClass::SVEvaluateEndRadiusClass( SVObjectClass* pOwner, int StringResourceID )
: SVEvaluate( pOwner, StringResourceID )
{
	// Identify yourself
	m_ObjectTypeInfo.m_SubType = SvPb::SVEvaluateEndRadiusObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputMathResult, SvPb::OutputEvaluateEndRadiusResultEId, IDS_OBJECTNAME_RESULT, false, SvOi::SVResetItemNone, false );

	// Set Embedded defaults
	m_outputMathResult.SetDefaultValue( 0.0, true );
}

SVEvaluateEndRadiusClass::~SVEvaluateEndRadiusClass()
{
}

} //namespace SvOp
