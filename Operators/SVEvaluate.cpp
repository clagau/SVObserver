//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "InspectionEngine/RunStatus.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS( SVEvaluate, SVEvaluateClassGuid )

SVEvaluate::SVEvaluate( SVObjectClass* POwner, int StringResourceID )
: SVMathContainer( POwner, StringResourceID ) 
{
	// Identify yourself
	m_ObjectTypeInfo.m_SubType = SvPb::SVEvaluateObjectType;
	m_outputMathResult.setSaveValueFlag(false);
}

SVEvaluate::~SVEvaluate()
{
	SVEvaluate::CloseObject();
}

bool SVEvaluate::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVMathContainer::CreateObject(rCreateStructure);

	// Set / Reset Printable flag
	m_outputMathResult.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}

SvVol::SVDoubleValueObjectClass* SVEvaluate::getOutputMathResult()
{
	return &m_outputMathResult;
}

bool SVEvaluate::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	SvVol::SVDoubleValueObjectClass* pResult = getOutputMathResult();
	
	// All inputs and outputs must be validated first
	if( __super::onRun( rRunStatus, pErrorMessages ) )
	{
		SvVol::SVDoubleValueObjectClass* pInputResult = m_inputMathResult.getInput<SvVol::SVDoubleValueObjectClass>(true);

		double Value( 0.0 );
		if( S_OK != pInputResult->GetValue(Value) || S_OK != pResult->SetValue(Value))
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
				pErrorMessages->push_back(Msg);
			}
			rRunStatus.SetInvalid();
			return false;
		}

		return true;
	}
	return false;
}

} //namespace SvOp
