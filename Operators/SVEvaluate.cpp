//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEvaluate
//* .File Name       : $Workfile:   SVEvaluate.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:21:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEvaluate.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVEvaluateClass, SVEvaluateClassGuid )

SVEvaluateClass::SVEvaluateClass( SVObjectClass* POwner, int StringResourceID )
: SVMathContainerClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVEvaluateObjectType;

	// Identify our input type needs...

	// Register Embedded Objects

	// Set Embedded defaults

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
	m_outputMathResult.setSaveValueFlag(false);
}

SVEvaluateClass::~SVEvaluateClass()
{
	CloseObject();
}

bool SVEvaluateClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVMathContainerClass::CreateObject(rCreateStructure);

	// Set / Reset Printable flag
	m_outputMathResult.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}

SvVol::SVDoubleValueObjectClass* SVEvaluateClass::getOutputMathResult()
{
	return &m_outputMathResult;
}

bool SVEvaluateClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	SvVol::SVDoubleValueObjectClass* pResult = getOutputMathResult();
	
	// All inputs and outputs must be validated first
	if( __super::onRun( rRunStatus, pErrorMessages ) )
	{
		SvVol::SVDoubleValueObjectClass* pInputResult = SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_inputMathResult, true);

		double Value( 0.0 );
		if( S_OK != pInputResult->GetValue(Value) || S_OK != pResult->SetValue(Value))
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			SetInvalid();
			rRunStatus.SetInvalid();
			return false;
		}

		return true;
	}
	return false;
}

} //namespace SvOp