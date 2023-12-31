//******************************************************************************
/// COPYRIGHT (c) 2016 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Class for X Translation Evaluation
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEvaluateTranslationXClass.h"
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

SV_IMPLEMENT_CLASS( SVEvaluateTranslationXClass, SvPb::EvaluateTranslationXClassId)

SVEvaluateTranslationXClass::SVEvaluateTranslationXClass( SVObjectClass* pOwner, int StringResourceID )
: SVEvaluate( pOwner, StringResourceID ) 
{
	// Identify yourself
	m_ObjectTypeInfo.m_SubType = SvPb::SVEvaluateTranslationXObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputMathResult, SvPb::OutputEvaluateTranslationXResultEId, IDS_OBJECTNAME_RESULT, false, SvOi::SVResetItemNone, false);

	// Set Embedded defaults
	m_outputMathResult.SetDefaultValue( 0.0, true );
}

SVEvaluateTranslationXClass::~SVEvaluateTranslationXClass()
{
}

} //namespace SvOp
