//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for Center X Evaluation
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEvaluateCenterXClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVClsIds.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVEvaluateCenterXClass, SvPb::EvaluateCenterXClassId)

SVEvaluateCenterXClass::SVEvaluateCenterXClass( SVObjectClass* pOwner, int StringResourceID )
: SVEvaluateClass( pOwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVEvaluateCenterXObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputMathResult, SVOutputEvaluateCenterXResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_outputMathResult.SetDefaultValue( 0.0, true );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVEvaluateCenterXClass::~SVEvaluateCenterXClass()
{
}

} //namespace SvOp
