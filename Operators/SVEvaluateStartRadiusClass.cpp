//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for Start Radius Evaluation
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEvaluateStartRadiusClass.h"
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

SV_IMPLEMENT_CLASS( SVEvaluateStartRadiusClass, SvPb::EvaluateStartRadiusClassId)

SVEvaluateStartRadiusClass::SVEvaluateStartRadiusClass( SVObjectClass* POwner, int StringResourceID )
					       :SVEvaluateClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVEvaluateStartRadiusObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputMathResult, SVOutputEvaluateStartRadiusResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_outputMathResult.SetDefaultValue( 0.0, true );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVEvaluateStartRadiusClass::~SVEvaluateStartRadiusClass()
{
}

} //namespace SvOp
