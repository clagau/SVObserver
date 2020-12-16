//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPolarTransform
//* .File Name       : $Workfile:   SVPolarTransform.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:56:12  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVPolarTransform.h"
#include "SVValueObjectLibrary\SVDoubleValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVPolarTransform, SvPb::PolarTransformClassId)

SVPolarTransform::SVPolarTransform( SVObjectClass* POwner, int StringResourceID )
				 :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVPolarTransformObjectType;

	// Identify our input type needs...

	// Center X
	m_inputCenterXResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateCenterXResultEId);
	registerInputObject( &m_inputCenterXResult, _T( "PolarTransformCenterXResult" ), SvPb::CenterXResultInputEId);
	m_inputCenterXResult.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	// Center Y
	m_inputCenterYResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateCenterYResultEId);
	registerInputObject( &m_inputCenterYResult, _T( "PolarTransformCenterYResult" ), SvPb::CenterYResultInputEId);
	m_inputCenterYResult.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	// Start Radius
	m_inputStartRadiusResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateStartRadiusResultEId);
	registerInputObject( &m_inputStartRadiusResult, _T( "PolarTransformStartRadiusResult" ), SvPb::StartRadiusResultInputEId);
	m_inputStartRadiusResult.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	// End Radius
	m_inputEndRadiusResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateEndRadiusResultEId);
	registerInputObject( &m_inputEndRadiusResult, _T( "PolarTransformEndRadiusResult" ), SvPb::EndRadiusResultInputEId);
	m_inputEndRadiusResult.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;
	
	// Start Angle
	m_inputStartAngleResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateStartAngleResultEId);
	registerInputObject( &m_inputStartAngleResult, _T( "PolarTransformStartAngleResult" ), SvPb::StartAngleResultInputEId);
	m_inputStartAngleResult.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	// End Angle
	m_inputEndAngleResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::OutputEvaluateEndAngleResultEId);
	registerInputObject( &m_inputEndAngleResult, _T( "PolarTransformEndAngleResult" ), SvPb::EndAngleResultInputEId);
	m_inputEndAngleResult.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;
}

SVPolarTransform::~SVPolarTransform()
{
	SVPolarTransform::CloseObject();
}

bool SVPolarTransform::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	std::initializer_list<SvOl::InputObject*> inputList
	{
		&m_inputCenterXResult,
		&m_inputCenterYResult,
		&m_inputEndAngleResult,
		&m_inputEndRadiusResult,
		&m_inputStartAngleResult,
		&m_inputStartRadiusResult
	};

	for(auto* pEntry : inputList)
	{
		pEntry->validateInput();
		// pEntry cannot be nullptr as the InputList are member variable addresses
		if(nullptr == pEntry->getInput<SvVol::SVDoubleValueObjectClass>())
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}
	}
	return Result;
}

} //namespace SvOp
